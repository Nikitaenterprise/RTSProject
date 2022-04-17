#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "TurretAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)       	\
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)           	\
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)

UCLASS()
class RTSPROJECT_API UTurretAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	ATTRIBUTE_ACCESSORS(UTurretAttributeSet, FireRate);
	ATTRIBUTE_ACCESSORS(UTurretAttributeSet, ChanceToFire);
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData FireRate = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData ChanceToFire = 1.0;
};
