#include "Actors/Resource.h"

#include <functional>
#include "Kismet/GameplayStatics.h"
#include "Actors/ResourceManager.h"
#include "Components/StaticMeshComponent.h"
#include "GAS/ResourceSourceAttributeSet.h"

AResource::AResource()
{
 	PrimaryActorTick.bCanEverTick = true;
	
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
	
	//AbilitySystemComponent->AddSet<UResourceSourceAttributeSet>();
	ResourceSourceAttributeSet = NewObject<UResourceSourceAttributeSet>(this);
	AbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(ResourceSourceAttributeSet);
	
	CalculateResource(std::bind(&ThisClass::InitialCapacity, this));
	
	// Bind resource capacity check
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		ResourceSourceAttributeSet->GetResourceCapacityAttribute()).AddUObject(this, &ThisClass::CheckCapacity);
}

void AResource::CheckCapacity(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0)
	{
		Destroy();
	}
}

float AResource::InitialCapacity()
{
	UE_LOG(LogTemp, Warning, TEXT("GetInitialCapacity() from base class was used"));
	return 0;
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