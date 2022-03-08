#pragma once

#include "Blueprint/UserWidget.h"
#include "HealthShieldBarHUD.generated.h"


UCLASS()
class RTSPROJECT_API UHealthShieldBarHUD : public UUserWidget
{
	GENERATED_BODY()

private:

	const float* HealthPercent = nullptr;
	const float* ShieldPercent = nullptr;

public:
	
	void BindHealthShieldValues(const float* Health, const float* Shield) { BindHealthValue(Health); BindShieldValue(Shield); }
	void BindHealthValue(const float* Health) { Health ? HealthPercent = Health : HealthPercent = nullptr; }
	void BindShieldValue(const float* Shield) { Shield ? ShieldPercent = Shield : ShieldPercent = nullptr; }
	void UnbindHealthShieldValues() { UnbindHealthValue(); UnbindShieldValue(); }
	void UnbindHealthValue() { HealthPercent = nullptr; }
	void UnbindShieldValue() { ShieldPercent = nullptr; }
	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const { return *HealthPercent; }
	UFUNCTION(BlueprintCallable)
	float GetShieldPercent() const { return *ShieldPercent; }
};