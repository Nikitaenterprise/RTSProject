#pragma once

#include "UnitIndicatorAdditionalDescriptionWidget.h"
#include "ResourceAdditionalDescriptionWidget.generated.h"


class UTextBlock;

UCLASS()
class RTSPROJECT_API UResourceAdditionalDescriptionWidget : public UUnitIndicatorAdditionalDescriptionWidget
{
	GENERATED_BODY()

protected:
	virtual void BindAttributes() override;

	void OnAttributeChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResourceCapacity {nullptr};
};
