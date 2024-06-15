#pragma once

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "ResourceAttributeSet.generated.h"

UCLASS()
class RTSPROJECT_API UResourceAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)       	\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)           	\
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)

	DECLARE_DELEGATE(FOnResourceEmpty);

public:
	ATTRIBUTE_ACCESSORS(UResourceAttributeSet, ResourceAmount)
	ATTRIBUTE_ACCESSORS(UResourceAttributeSet, ResourceCapacity)
	ATTRIBUTE_ACCESSORS(UResourceAttributeSet, ResourceTransferringSpeed)

	FOnResourceEmpty OnResourceEmpty;

protected:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	// Current resource amount
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData ResourceAmount;

	// Maximum resource
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData ResourceCapacity;

	// How much resource is transferred per cycle (tick, timer, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData ResourceTransferringSpeed;
};
