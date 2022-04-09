#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "HealthShieldAttributeSet.generated.h"

DECLARE_DELEGATE(FOnHealthZeroed);
DECLARE_DELEGATE(FOnShieldZeroed);

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)       	\
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)           	\
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)
/**
 * 
 */
UCLASS()
class RTSPROJECT_API UHealthShieldAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	ATTRIBUTE_ACCESSORS(UHealthShieldAttributeSet, CurrentHealth)
	ATTRIBUTE_ACCESSORS(UHealthShieldAttributeSet, MaxHealth)
	FOnHealthZeroed OnHealthZeroed;
	ATTRIBUTE_ACCESSORS(UHealthShieldAttributeSet, CurrentShield)
	ATTRIBUTE_ACCESSORS(UHealthShieldAttributeSet, MaxShield)
	FOnShieldZeroed OnShieldZeroed;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData CurrentHealth = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData CurrentShield = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxHealth = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxShield = 100.0f;
};
