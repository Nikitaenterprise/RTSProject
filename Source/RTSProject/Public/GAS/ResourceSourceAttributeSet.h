﻿#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ResourceSourceAttributeSet.generated.h"

DECLARE_DELEGATE(FOnResourceCapacityZeroed);

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
	FOnResourceCapacityZeroed OnResourceCapacityZeroed;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData ResourceCapacity = 100.0f;
};