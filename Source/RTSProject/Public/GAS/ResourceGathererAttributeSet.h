#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ResourceGathererAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)       	\
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)           	\
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)

UCLASS()
class RTSPROJECT_API UResourceGathererAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:

	ATTRIBUTE_ACCESSORS(UResourceGathererAttributeSet, CargoStorage)
	ATTRIBUTE_ACCESSORS(UResourceGathererAttributeSet, CargoStorageLimit)
	ATTRIBUTE_ACCESSORS(UResourceGathererAttributeSet, ResourceGatheringSpeed)
	
protected:

	// Current amount in storage
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData CargoStorage = 0.f;
	// Max storage capacity
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData CargoStorageLimit = 300.0f;
	// How much resource is gathered per cycle (tick, timer, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData ResourceGatheringSpeed = 50.0f;
	
	
};
