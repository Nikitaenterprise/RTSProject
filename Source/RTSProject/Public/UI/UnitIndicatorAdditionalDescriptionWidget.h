#pragma once

#include "Blueprint/UserWidget.h"
#include "UnitIndicatorAdditionalDescriptionWidget.generated.h"


class ABaseUnit;
class UAbilitySystemComponent;
class UGameplayAbilitySet;

UCLASS()
class RTSPROJECT_API UUnitIndicatorAdditionalDescriptionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetBaseUnitOwner(ABaseUnit* InBaseUnit);

protected:
	virtual void BindAttributes() {};
	
	UPROPERTY()
	ABaseUnit* BaseUnit {nullptr};

	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent {nullptr};
};
