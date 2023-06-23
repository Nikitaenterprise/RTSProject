#include "Components/ResourceComponent.h"

#include "AbilitySystemInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/Resources/ResourceManager.h"
#include "Core/RTSGameMode.h"
#include "GAS/ResourceSourceAttributeSet.h"

UResourceComponent::UResourceComponent()
{
	ResourceType = EResourceType::None;
}

void UResourceComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ResourceManager = ARTSGameMode::GetRTSGameMode(GetWorld())->GetResourceManager();
	if (IsValid(ResourceManager) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AResourceManager is nullptr in UResourceComponent::BeginPlay()"));
		return;
	}
	ResourceManager->AddResource(GetOwner());
	switch (ResourceType)
	{
	case EResourceType::AsteroidField:
		ResourceManager->AddAsteroidField(Cast<AAsteroidField>(GetOwner()));
		break;
	case EResourceType::Asteroid:
		break;
	default:
		break;
	}

	UAbilitySystemComponent* OwnerAbilitySystem = GetOwnerAbilitySystemComponent();
	if (OwnerAbilitySystem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UResourceComponent::BeginPlay OwnerAbilitySystem is nullptr"));
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "UResourceComponent::BeginPlay OwnerAbilitySystem is nullptr");
		return;
	}
	
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
	ResourceCapacityDelegate = OwnerAbilitySystem->GetGameplayAttributeValueChangeDelegate(ResourceSourceAttributeSet->GetResourceCapacityAttribute());
	OwnerAbilitySystem->GetSpawnedAttributes_Mutable().AddUnique(ResourceSourceAttributeSet);
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

void UResourceComponent::InitializeCapacity(const TFunction<float()>& CalculationFunction)
{
	ModifyResource(CalculationFunction);
	bCapacityWasInitialized = true;
}

void UResourceComponent::ModifyResource(const TFunction<float()>& CalculationFunction)
{
	if (!CalculationFunction)
	{
		UE_LOG(LogTemp, Warning, TEXT("CalculationFunction is nullptr in UResourceComponent::CalculateResource()"));
		return;
	}

	UAbilitySystemComponent* OwnerAbilitySystem = GetOwnerAbilitySystemComponent();
	if (OwnerAbilitySystem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UResourceComponent::ModifyResource OwnerAbilitySystem is nullptr"));
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "UResourceComponent::ModifyResource OwnerAbilitySystem is nullptr");
		return;
	}
	
	UGameplayEffect* GameplayEffect = NewObject<UGameplayEffect>();
	GameplayEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo GameplayModifierInfo;
	GameplayModifierInfo.Attribute = ResourceSourceAttributeSet->GetResourceCapacityAttribute();
	GameplayModifierInfo.ModifierOp = EGameplayModOp::Override;
	GameplayModifierInfo.ModifierMagnitude = FScalableFloat(CalculationFunction());
	GameplayEffect->Modifiers.Add(GameplayModifierInfo);
	OwnerAbilitySystem->ApplyGameplayEffectToSelf(GameplayEffect, 0, OwnerAbilitySystem->MakeEffectContext());
}

UAbilitySystemComponent* UResourceComponent::GetOwnerAbilitySystemComponent() const
{
	auto* Owner = GetOwner();
	if (Owner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UResourceComponent::GetOwnerAbilitySystemComponent owner is nullptr"));
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "UResourceComponent::GetOwnerAbilitySystemComponent owner is nullptr");
		return nullptr;
	}
	
	if (Cast<IAbilitySystemInterface>(Owner) == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UResourceComponent::GetOwnerAbilitySystemComponent owner doesn't implement IAbilitySystemInterface"));
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "UResourceComponent::GetOwnerAbilitySystemComponent owner doesn't implement IAbilitySystemInterface");
		return nullptr;
	}
	return Cast<IAbilitySystemInterface>(Owner)->GetAbilitySystemComponent();
}
