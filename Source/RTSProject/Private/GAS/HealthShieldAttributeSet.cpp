#include "GAS/HealthShieldAttributeSet.h"

void UHealthShieldAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetCurrentHealthAttribute())
	{
		if (NewValue > 0)
		{
			return;
		}
		NewValue = 0;
		// broadcast if health < 0
		OnHealthZeroed.ExecuteIfBound();
	}
	else if (Attribute == GetCurrentShieldAttribute())
	{
		if (NewValue > 0)
		{
			return;
		}
		NewValue = 0;
		// broadcast if shield < 0
		OnShieldZeroed.ExecuteIfBound();
	}
}