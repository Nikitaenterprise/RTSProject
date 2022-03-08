#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ResourceSourceAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)       	\
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)           	\
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)

UCLASS()
class RTSPROJECT_API UResourceSourceAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	
	ATTRIBUTE_ACCESSORS(UResourceSourceAttributeSet, ResourceCapacity)
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UResourceSourceAttributeSet, InitResourceCapacity)
	GAMEPLAYATTRIBUTE_VALUE_GETTER(InitResourceCapacity)
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData ResourceCapacity = 100.0f;
	UPROPERTY(EditAnywhere, EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData InitResourceCapacity = 100.0f;
	
};
