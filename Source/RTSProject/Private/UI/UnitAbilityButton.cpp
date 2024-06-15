#include "UI/UnitAbilityButton.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/AbilityEntitlement.h"
#include "GAS/EntitlementUIData.h"


void UUnitAbilityButton::SetupAbilityButton(UAbilitySystemComponent* InAbilitySystemComponent,
	const FGameplayAbilitySpec& InGameplayAbilitySpec,
	const UAbilityEntitlement* Entitlement)
{
	if (InAbilitySystemComponent == nullptr)
	{
		return;
	}
	AbilitySystemComponent = InAbilitySystemComponent;
	
	Spec = InGameplayAbilitySpec;

	if (Entitlement == nullptr)
	{
		return;
	}
	
	if (const auto UIData = Entitlement->UIData)
	{
		if (UIData.Get() == nullptr)
		{
			UIData.LoadSynchronous();
		}
	
		AbilityImage->SetBrush(UIData.Get()->Icon);
	}
	
	Description->SetText(Entitlement->Description);
}

void UUnitAbilityButton::NativeConstruct()
{
	Super::NativeConstruct();

	Button->OnClicked.AddUniqueDynamic(this, &ThisClass::OnAbilityActivated);
}

void UUnitAbilityButton::OnAbilityActivated()
{
	if (AbilitySystemComponent == nullptr)
	{
		return;
	}

	AbilitySystemComponent->TryActivateAbility(Spec.Handle);
}