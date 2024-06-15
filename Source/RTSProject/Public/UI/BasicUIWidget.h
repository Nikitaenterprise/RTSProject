#pragma once

#include "Blueprint/UserWidget.h"
#include "BasicUIWidget.generated.h"

class USelectionRectangleWidget;
class UUnitAbilitiesPanel;
class ARTSPlayerController;

UCLASS()
class RTSPROJECT_API UBasicUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ARTSPlayerController* PlayerController = nullptr;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	bool IsShipsAreSelected() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsBuildingsAreSelected() const;

	UFUNCTION(BlueprintCallable)
	bool IsResourceStorageSelected() const;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USelectionRectangleWidget* SelectionRectangle {nullptr};
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UUnitAbilitiesPanel* UnitAbilitiesPanel {nullptr};
};
