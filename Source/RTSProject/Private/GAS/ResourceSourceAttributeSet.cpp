#include "GAS/ResourceSourceAttributeSet.h"

void UResourceSourceAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetResourceCapacityAttribute())
	{
		if (NewValue > 0)
		{
			return;
		}
		NewValue = 0;
		// broadcast if health < 0
		OnResourceCapacityZeroed.ExecuteIfBound();
	}
}