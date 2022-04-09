#pragma once

#include "Blueprint/UserWidget.h"
#include "HealthShieldWidget.generated.h"

class UHealthShieldAttributeSet;
class UProgressBar;

UCLASS()
class RTSPROJECT_API UHealthShieldWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta=(BindWidget))
	UProgressBar* ShieldProgressBar{nullptr};
	UPROPERTY(meta=(BindWidget))
	UProgressBar* HealthProgressBar{nullptr};

	UPROPERTY()
	UHealthShieldAttributeSet* HealthShieldAttributeSet = nullptr;
	float HealthPercent = 0;
	float ShieldPercent = 0;
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void SetHealthShieldAttributeSet(UHealthShieldAttributeSet* NewHealthShieldAttributeSet)
	{
		HealthShieldAttributeSet = NewHealthShieldAttributeSet;
	}

	void UpdateHealthShieldValues();
	void UpdateHealthValue();
	void UpdateShieldValue();
	UFUNCTION(BlueprintCallable)
	float GetHealthPercent();
	UFUNCTION(BlueprintCallable)
	float GetShieldPercent();
};
