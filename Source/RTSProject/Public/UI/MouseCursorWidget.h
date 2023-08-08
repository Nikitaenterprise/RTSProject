#pragma once

#include "Blueprint/UserWidget.h"
#include "MouseCursorWidget.generated.h"

UENUM()
enum class ECursorType : uint8
{
	Default,
	Ability,
	Order
};

class UImage;
class UEntitlementUIData;

UCLASS()
class RTSPROJECT_API UMouseCursorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetCursorToDefault() const;

	UFUNCTION(BlueprintCallable)
	void SetCursorFromAbility(UEntitlementUIData* UIData) const;

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UImage* CursorImage {nullptr};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FSlateBrush DefaultCursor;
};
