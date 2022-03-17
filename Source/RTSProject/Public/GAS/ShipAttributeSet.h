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

protected:

	ATTRIBUTE_ACCESSORS(UShipAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(UShipAttributeSet, Shield)
	ATTRIBUTE_ACCESSORS(UShipAttributeSet, MoveSpeed)
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Health = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Shield = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MoveSpeed = 600.0f;

};
