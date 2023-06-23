#include "Actors/Units/BaseUnitWithAbility.h"

#include "AbilitySystemComponent.h"
#include "GAS/HealthShieldAttributeSet.h"


ABaseUnitWithAbility::ABaseUnitWithAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

void ABaseUnitWithAbility::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	for (const auto& Attribute : Attributes)
	{
		AbilitySystemComponent->InitStats(Attribute, nullptr);
	}
	UnitAttributeSets = AbilitySystemComponent->GetSpawnedAttributes();
}

void ABaseUnitWithAbility::BeginPlay()
{
	Super::BeginPlay();

	for (const auto UnitAbility : UnitAbilities)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(UnitAbility, 1, INDEX_NONE, this));
	}
	
	AbilitySystemComponent->GetSpawnedAttributes().FindItemByClass(&HealthShieldAttributeSet);
	if (HealthShieldAttributeSet)
	{
		HealthShieldAttributeSet->OnHealthZeroed.BindLambda([This = TWeakObjectPtr<ThisClass>(this)]()
		{
			if (This.IsValid())
			{
				This->Destroy();
			}
		});
	}
	
	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	for (const auto DefaultGameplayEffect : DefaultGameplayEffects)
	{
		AbilitySystemComponent->ApplyGameplayEffectToSelf(DefaultGameplayEffect->GetDefaultObject<UGameplayEffect>(), 0.0f, Context);
	}
}
