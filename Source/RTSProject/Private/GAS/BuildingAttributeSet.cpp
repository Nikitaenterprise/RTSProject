#include "GAS/BuildingAttributeSet.h"
void UBuildingAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute())
	{
		if (NewValue > 0)
		{
			return;
		}
		NewValue = 0;
		// broadcast if health < 0
		OnHealthZeroed.ExecuteIfBound();
	}
}