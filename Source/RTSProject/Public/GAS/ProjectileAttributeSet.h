#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ProjectileAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)       	\
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)           	\
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)

UCLASS()
class RTSPROJECT_API UProjectileAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	ATTRIBUTE_ACCESSORS(UProjectileAttributeSet, MaxLifeTime);
	ATTRIBUTE_ACCESSORS(UProjectileAttributeSet, MaxSpeed);
	ATTRIBUTE_ACCESSORS(UProjectileAttributeSet, Damage);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxLifeTime = 50;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxSpeed = 5000;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Damage = 25;
};
