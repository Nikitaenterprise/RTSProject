#pragma once

#include "Blueprint/UserWidget.h"
#include "UnitAbilitiesPanel.generated.h"


class USelectionRectangleWidget;
class UUnitAbilityButton;
class UUniformGridPanel;

UCLASS()
class RTSPROJECT_API UUnitAbilitiesPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	void SubscribeToUpdate(USelectionRectangleWidget* SelectionRectangleWidget);
	
protected:
	UFUNCTION()
	void OnUnitsSelected(const TArray<AActor*>& SelectedUnits);

	UFUNCTION()
	void OnUnitsDeselected();

	void NativePreConstruct() override;
	
	void RefreshAbilitiesPanel();

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* GridPanel {nullptr};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUnitAbilityButton> AbilityButtonSubclass;

	UPROPERTY()
	TArray<UUnitAbilityButton*> AbilityButtons;

	UPROPERTY(EditDefaultsOnly)
	int MaxColumns = 4;

	UPROPERTY(EditDefaultsOnly)
	int MaxRows = 5;
};
