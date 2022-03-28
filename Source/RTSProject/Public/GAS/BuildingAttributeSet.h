#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BuildingAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)       	\
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)           	\
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)

DECLARE_DELEGATE(FOnHealthZeroed);

UCLASS()
class RTSPROJECT_API UBuildingAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	ATTRIBUTE_ACCESSORS(UBuildingAttributeSet, Health)
	void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue);
	ATTRIBUTE_ACCESSORS(UBuildingAttributeSet, Shield)
	ATTRIBUTE_ACCESSORS(UBuildingAttributeSet, BuildingSpeed);
	FOnHealthZeroed OnHealthZeroed;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Health = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Shield = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData BuildingSpeed = 5.0f;
};
