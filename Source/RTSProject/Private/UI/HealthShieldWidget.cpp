#include "UI/HealthShieldWidget.h"
#include "Components/ProgressBar.h"
#include "GAS/HealthShieldAttributeSet.h"


void UHealthShieldWidget::BindAttributes()
{
	Super::BindAttributes();

	auto* HealthShieldAttributeSet = Cast<UHealthShieldAttributeSet>(AbilitySystemComponent->GetSet<UHealthShieldAttributeSet>());
	if (HealthShieldAttributeSet == nullptr)
	{
		return;
	}
	
	const auto MaxHealthAttribute = HealthShieldAttributeSet->GetMaxHealthAttribute();
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MaxHealthAttribute).AddUObject(this, &ThisClass::UpdateMaxHealthValue);
	
	const auto HealthAttribute = HealthShieldAttributeSet->GetCurrentHealthAttribute();
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthAttribute).AddUObject(this, &ThisClass::UpdateHealthValue);

	const auto MaxShieldAttribute = HealthShieldAttributeSet->GetMaxShieldAttribute();
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MaxShieldAttribute).AddUObject(this, &ThisClass::UpdateMaxShieldValue);

	const auto ShieldAttribute = HealthShieldAttributeSet->GetCurrentShieldAttribute();
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(ShieldAttribute).AddUObject(this, &ThisClass::UpdateShieldValue);

	HealthProgressBar->SetPercent(HealthShieldAttributeSet->GetCurrentHealth() / HealthShieldAttributeSet->GetMaxHealth());
	ShieldProgressBar->SetPercent(HealthShieldAttributeSet->GetCurrentShield() / HealthShieldAttributeSet->GetMaxShield());
}

void UHealthShieldWidget::UpdateHealthValue(const FOnAttributeChangeData& Data)
{
	HealthProgressBar->SetPercent(Data.NewValue / MaxHealth);
}

void UHealthShieldWidget::UpdateMaxHealthValue(const FOnAttributeChangeData& Data)
{
	MaxHealth = Data.NewValue;
}

void UHealthShieldWidget::UpdateShieldValue(const FOnAttributeChangeData& Data)
{
	ShieldProgressBar->SetPercent(Data.NewValue / MaxShield);
}

void UHealthShieldWidget::UpdateMaxShieldValue(const FOnAttributeChangeData& Data)
{
	MaxShield = Data.NewValue;
}
