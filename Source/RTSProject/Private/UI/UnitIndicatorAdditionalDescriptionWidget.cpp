#include "UI/UnitIndicatorAdditionalDescriptionWidget.h"

#include "Actors/Units/BaseUnitWithAbility.h"

void UUnitIndicatorAdditionalDescriptionWidget::SetBaseUnitOwner(ABaseUnit* InBaseUnit)
{
	BaseUnit = InBaseUnit;

	const auto* UnitWithAbility = Cast<ABaseUnitWithAbility>(BaseUnit);
	if (UnitWithAbility == nullptr)
	{
		return;
	}

	AbilitySystemComponent = UnitWithAbility->GetAbilitySystemComponent();
	if (AbilitySystemComponent == nullptr)
	{
		return;
	}
	
	BindAttributes();
}
