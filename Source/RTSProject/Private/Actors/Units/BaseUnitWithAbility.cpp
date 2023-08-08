#include "Actors/Units/BaseUnitWithAbility.h"

#include "AbilitySystemComponent.h"
#include "Core/RTSPlayerController.h"
#include "GAS/AbilityEntitlement.h"
#include "GAS/EntitlementUIData.h"
#include "GAS/HealthShieldAttributeSet.h"
#include "UI/GameHUD.h"
#include "UI/MouseCursorWidget.h"


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

	for (const auto SoftAbilityEntitlement : UnitAbilityEntitlements)
	{
		if (SoftAbilityEntitlement.Get() == nullptr)
		{
			SoftAbilityEntitlement.LoadSynchronous();
		}
		
		if (auto* AbilityEntitlement = SoftAbilityEntitlement.Get())
		{
			for (const auto Ability : AbilityEntitlement->GameplayAbilities)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, INDEX_NONE, this));
			}

			for (const auto Ability : AbilityEntitlement->AdditionalAbilities)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, INDEX_NONE, this));
			}
		}
	}

	const FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	for (const auto DefaultGameplayEffect : DefaultGameplayEffects)
	{
		AbilitySystemComponent->ApplyGameplayEffectToSelf(DefaultGameplayEffect->GetDefaultObject<UGameplayEffect>(), 0.0f, Context);
	}
}

void ABaseUnitWithAbility::BeginPlay()
{
	Super::BeginPlay();
	
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
}

void ABaseUnitWithAbility::Selected_Implementation(bool bInIsSelected)
{
	Super::Selected_Implementation(bInIsSelected);
	
	if (bIsSelected)
	{
		for (const auto SoftAbilityEntitlement : UnitAbilityEntitlements)
		{
			if (auto* AbilityEntitlement = SoftAbilityEntitlement.Get())
			{
				for (const auto Ability : AbilityEntitlement->GameplayAbilities)
				{
					const FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(Ability);
					if (Spec && Spec->IsActive())
					{
						MouseCursorWidget->SetCursorFromAbility(AbilityEntitlement->CursorData.Get());
					}
				}
			}
		}
	}
}