#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ShipAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)       	\
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)           	\
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)

UCLASS()
class RTSPROJECT_API UShipAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	ATTRIBUTE_ACCESSORS(UShipAttributeSet, CurrentMoveSpeed)
	ATTRIBUTE_ACCESSORS(UShipAttributeSet, MaxMoveSpeed)
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData CurrentMoveSpeed = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxMoveSpeed = 600.0f;
};
