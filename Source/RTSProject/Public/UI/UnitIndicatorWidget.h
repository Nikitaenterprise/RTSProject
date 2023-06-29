#pragma once

#include "Blueprint/UserWidget.h"
#include "UnitIndicatorWidget.generated.h"

class ABaseUnit;
class UUnitIndicatorAdditionalDescriptionWidget;
class UHealthShieldWidget;
class UImage;
class USizeBox;
class UTextBlock;

UCLASS()
class RTSPROJECT_API UUnitIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetVisibility(ESlateVisibility InVisibility) override;

	void SetBaseUnitOwner(ABaseUnit* InOwnerBaseUnit);
	void SetAdditionalContext(UUnitIndicatorAdditionalDescriptionWidget* InAdditionalContext);
	
protected:
	UPROPERTY(meta = (BindWidget))
	USizeBox* SelectionGroupIndicator {nullptr};
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GroupText {nullptr};
	
	UPROPERTY(meta = (BindWidget))
	UHealthShieldWidget* HealthShieldWidget {nullptr};

	UPROPERTY(meta = (BindWidget))
	USizeBox* IconScaleBox {nullptr};
	
	UPROPERTY(meta = (BindWidget))
	UImage* Icon {nullptr};

	UPROPERTY(meta = (BindWidget))
	UNamedSlot* AdditionalContextSlot {nullptr};
	
	UPROPERTY()
	UUnitIndicatorAdditionalDescriptionWidget* AdditionalContext {nullptr};

	UPROPERTY()
	ABaseUnit* OwnerBaseUnit {nullptr};
};
