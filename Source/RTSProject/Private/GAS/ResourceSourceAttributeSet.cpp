#include "GAS/ResourceSourceAttributeSet.h"

void UResourceSourceAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetResourceCapacityAttribute())
	{
		NewValue = NewValue > 0 ? NewValue : 0;
	}
}