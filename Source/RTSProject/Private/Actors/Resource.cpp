#include "Actors/Resource.h"

#include <functional>
#include "Kismet/GameplayStatics.h"
#include "Actors/ResourceManager.h"
#include "GAS/ResourceSourceAttributeSet.h"

AResource::AResource()
{
 	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ResourceType = EResourceType::None;
}

void AResource::BeginPlay()
{
	Super::BeginPlay();

	const auto TestResourceManager = Cast<AResourceManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AResourceManager::StaticClass()));
	if (!IsValid(TestResourceManager))
	{
		UE_LOG(LogTemp, Error, TEXT("AResourceManager is nullptr in AResource::BeginPlay()"));
		return;
	}
	ResourceManager = TestResourceManager;
	ResourceManager->AddResource(this);
	
	//AbilitySystemComponent->AddSet<UResourceSourceAttributeSet>();
	ResourceSourceAttributeSet = NewObject<UResourceSourceAttributeSet>(this);
	AbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(ResourceSourceAttributeSet);
	
	CalculateResource(std::bind(&ThisClass::SetupInitialCapacity, this));
	
	// Bind resource capacity check
	ResourceCapacityDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		ResourceSourceAttributeSet->GetResourceCapacityAttribute()).AddUObject(this, &ThisClass::CheckCapacity);
}

void AResource::CheckCapacity(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0)
	{
		ResourceManager->RemoveResource(this);
		Destroy();
	}
}

float AResource::SetupInitialCapacity()
{
	return InitialCapacity;
}

void AResource::CalculateResource(TFunction<float()> Func)
{
	UGameplayEffect* GameplayEffect = NewObject<UGameplayEffect>();
	GameplayEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo GameplayModifierInfo;
	GameplayModifierInfo.Attribute = ResourceSourceAttributeSet->GetResourceCapacityAttribute();
	GameplayModifierInfo.ModifierOp = EGameplayModOp::Override;	
	GameplayModifierInfo.ModifierMagnitude = FScalableFloat(Func());
	GameplayEffect->Modifiers.Add(GameplayModifierInfo);
	AbilitySystemComponent->ApplyGameplayEffectToSelf(GameplayEffect, 0, AbilitySystemComponent->MakeEffectContext());
}