#include "Components/ResourceComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Actors/Resources/ResourceManager.h"
#include "GAS/ResourceSourceAttributeSet.h"

UResourceComponent::UResourceComponent()
{
	ResourceType = EResourceType::None;
}

void UResourceComponent::BeginPlay()
{
	Super::BeginPlay();

	const auto TestResourceManager = Cast<AResourceManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AResourceManager::StaticClass()));
	if (!IsValid(TestResourceManager))
	{
		UE_LOG(LogTemp, Error, TEXT("AResourceManager is nullptr in AResource::BeginPlay()"));
		return;
	}
	ResourceManager = TestResourceManager;
	ResourceManager->AddResource(GetOwner());
	switch (ResourceType)
	{
	case EResourceType::AsteroidField:
		ResourceManager->AddAsteroidField(Cast<AAsteroidField>(GetOwner()));
		break;
	default:
		break;
	}

	const auto TestAbilitySystemComponent = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();
	if (!TestAbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor %s has no UAbilitySystemComponent"), *GetOwner()->GetName());
		return;
	}
	AbilitySystemComponent = TestAbilitySystemComponent;
	ResourceSourceAttributeSet = NewObject<UResourceSourceAttributeSet>(this);
	ResourceSourceAttributeSet->OnResourceCapacityZeroed.BindLambda([This = TWeakObjectPtr<ThisClass>(this)]()
	{
		if(This.IsValid())
		{
			if (!This->bCapacityWasInitialized)
			{
				return;
			}
			This->ResourceManager->RemoveResource(This->GetOwner());
			This->GetOwner()->Destroy();
		}
	});
	AbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(ResourceSourceAttributeSet);
}

void UResourceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ResourceManager)
	{
		ResourceManager->RemoveResource(GetOwner());
		switch (ResourceType)
		{
		case EResourceType::AsteroidField:
			ResourceManager->RemoveAsteroidField(Cast<AAsteroidField>(GetOwner()));
			break;
		default:
			break;
		}
	}
	Super::EndPlay(EndPlayReason);
}

void UResourceComponent::InitializeCapacity(TFunction<float()> CalculationFunction)
{
	ModifyResource(CalculationFunction);
	bCapacityWasInitialized = true;
}

void UResourceComponent::ModifyResource(TFunction<float()> CalculationFunction)
{
	if (!CalculationFunction)
	{
		UE_LOG(LogTemp, Warning, TEXT("CalculationFunction is nullptr in UResourceComponent::CalculateResource()"));
		return;
	}
	if (!IsValid(AbilitySystemComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilitySystemComponent is nullptr in UResourceComponent::CalculateResource()"));
		return;
	}
	UGameplayEffect* GameplayEffect = NewObject<UGameplayEffect>();
	GameplayEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo GameplayModifierInfo;
	GameplayModifierInfo.Attribute = ResourceSourceAttributeSet->GetResourceCapacityAttribute();
	GameplayModifierInfo.ModifierOp = EGameplayModOp::Override;
	GameplayModifierInfo.ModifierMagnitude = FScalableFloat(CalculationFunction());
	GameplayEffect->Modifiers.Add(GameplayModifierInfo);
	AbilitySystemComponent->ApplyGameplayEffectToSelf(GameplayEffect, 0, AbilitySystemComponent->MakeEffectContext());
}