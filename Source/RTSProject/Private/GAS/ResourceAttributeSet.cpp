#include "GAS/ResourceAttributeSet.h"

void UResourceAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetResourceAmountAttribute())
	{
		if (NewValue > 0)
		{
			return;
		}
		// Broadcast if resource amount zeroed < 0
		OnResourceEmpty.ExecuteIfBound();
	}
}