#pragma once

#include "UnitIndicatorAdditionalDescriptionWidget.h"
#include "HealthShieldWidget.generated.h"

class UProgressBar;

UCLASS()
class RTSPROJECT_API UHealthShieldWidget : public UUnitIndicatorAdditionalDescriptionWidget
{
	GENERATED_BODY()

protected:
	virtual void BindAttributes() override;

	void UpdateHealthValue(const FOnAttributeChangeData& Data);
	void UpdateMaxHealthValue(const FOnAttributeChangeData& Data);
	
	void UpdateShieldValue(const FOnAttributeChangeData& Data);
	void UpdateMaxShieldValue(const FOnAttributeChangeData& Data);
	
	UPROPERTY(meta=(BindWidget))
	UProgressBar* ShieldProgressBar {nullptr};
	
	UPROPERTY(meta=(BindWidget))
	UProgressBar* HealthProgressBar {nullptr};

	float Health = 0;
	float Shield = 0;
	float MaxHealth = 0;
	float MaxShield = 0;
};
