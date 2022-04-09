#include "UI/HealthShieldWidget.h"
#include "UMG/Public/Components/ProgressBar.h"
#include "GAS/HealthShieldAttributeSet.h"

void UHealthShieldWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateHealthShieldValues();
}

void UHealthShieldWidget::UpdateHealthShieldValues()
{
	UpdateHealthValue();
	UpdateShieldValue();
}

void UHealthShieldWidget::UpdateHealthValue()
{
	if (IsValid(HealthShieldAttributeSet))
	{
		HealthPercent = HealthShieldAttributeSet->GetCurrentHealth() / HealthShieldAttributeSet->GetMaxHealth();
		if (IsValid(HealthProgressBar))
		{
			HealthProgressBar->Percent = HealthPercent;
		}
	}
}

void UHealthShieldWidget::UpdateShieldValue()
{
	if (IsValid(HealthShieldAttributeSet))
	{
		ShieldPercent = HealthShieldAttributeSet->GetCurrentShield() / HealthShieldAttributeSet->GetMaxShield();
		if (IsValid(ShieldProgressBar))
		{
			ShieldProgressBar->Percent = ShieldPercent;
		}
	}
}

float UHealthShieldWidget::GetHealthPercent()
{
	UpdateHealthValue();
	return HealthPercent;
}

float UHealthShieldWidget::GetShieldPercent()
{
	UpdateShieldValue();
	return ShieldPercent;
}
