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
	ATTRIBUTE_ACCESSORS(UProjectileAttributeSet, Speed);
	ATTRIBUTE_ACCESSORS(UProjectileAttributeSet, Damage);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxLifeTime = 5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Speed = 300;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Damage = 25;
};
