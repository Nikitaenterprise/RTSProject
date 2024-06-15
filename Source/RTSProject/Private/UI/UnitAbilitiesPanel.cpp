#include "UI/UnitAbilitiesPanel.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "GAS/AbilityEntitlement.h"
#include "UI/SelectionRectangleWidget.h"
#include "UI/UnitAbilityButton.h"

void UUnitAbilitiesPanel::SubscribeToUpdate(USelectionRectangleWidget* SelectionRectangleWidget)
{
	if (SelectionRectangleWidget == nullptr)
	{
		return;
	}

	SelectionRectangleWidget->OnSelectionEnded.BindUObject(this, &ThisClass::OnUnitsSelected);
	SelectionRectangleWidget->OnUnitsDeselected.BindUObject(this, &UUnitAbilitiesPanel::OnUnitsDeselected);
}

void UUnitAbilitiesPanel::OnUnitsSelected(const TArray<AActor*>& SelectedUnits)
{
	TArray<ABaseUnitWithAbility*> UnitsWithAbility;
	UnitsWithAbility.Reserve(SelectedUnits.Num());
	
	for (auto& Unit : SelectedUnits)
	{
		auto* UnitWithAbility = Cast<ABaseUnitWithAbility>(Unit);
		if (UnitWithAbility == nullptr)
		{
			continue;
		}
		UnitsWithAbility.AddUnique(UnitWithAbility);
	}

	AbilityButtons.Reset(UnitsWithAbility.Num());
	
	for (const auto& Unit : UnitsWithAbility)
	{
		auto* AbilitySystemComponent = Unit->GetAbilitySystemComponent();
		if (AbilitySystemComponent == nullptr)
		{
			continue;
		}
		
		for (const auto& SoftEntitlement : Unit->GetUnitAbilityEntitlements())
		{
			const auto* Entitlement = SoftEntitlement.Get();
			if (Entitlement == nullptr)
			{
				continue;
			}

			if (Entitlement->GameplayAbilities.Num() == 0)
			{
				continue;
			}
			
			for (const auto& Ability : AbilitySystemComponent->GetActivatableAbilities())
			{
				// Check if this ability is contained inside GameplayAbilities and can be activated from UI
				if (Entitlement->GameplayAbilities.Contains(Ability.Ability->GetClass()))
				{
					auto* AbilityButton = CreateWidget<UUnitAbilityButton>(GridPanel, AbilityButtonSubclass);
					AbilityButton->SetupAbilityButton(AbilitySystemComponent, Ability, Entitlement);
					AbilityButton->AddToViewport();
					AbilityButtons.AddUnique(AbilityButton);	
				}
			}
		}
	}

	RefreshAbilitiesPanel();
}

void UUnitAbilitiesPanel::OnUnitsDeselected()
{
	GridPanel->ClearChildren();
	AbilityButtons.Empty();
}

void UUnitAbilitiesPanel::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime() == false)
	{
		AbilityButtons.Empty();
		return;
	}
	
	for (int Index = 0; Index < 5; Index++)
	{
		AbilityButtons.AddUnique(CreateWidget<UUnitAbilityButton>(GridPanel, AbilityButtonSubclass));
	}
	RefreshAbilitiesPanel();
}

void UUnitAbilitiesPanel::RefreshAbilitiesPanel()
{
	if (AbilityButtons.Num() == 0)
	{
		return;
	}

	GridPanel->ClearChildren();
	
	for (int Index = 0; Index < AbilityButtons.Num(); Index++)
	{
		const auto* AbilityButton = AbilityButtons[Index];
		if (AbilityButton == nullptr)
		{
			continue;
		}
		const int Column = Index % MaxColumns;
		const int Row = (Index - Column) / MaxColumns;

		auto* UniformGridSlot = GridPanel->AddChildToUniformGrid(AbilityButtons[Index], Row, Column);
		if (UniformGridSlot == nullptr)
		{
			continue;
		}
		
		UniformGridSlot->SetVerticalAlignment(VAlign_Center);
		UniformGridSlot->SetHorizontalAlignment(HAlign_Center);
	}
}