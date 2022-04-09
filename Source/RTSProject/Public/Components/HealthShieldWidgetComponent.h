#pragma once

#include "Components/WidgetComponent.h"
#include "GAS/HealthShieldAttributeSet.h"
#include "HealthShieldWidgetComponent.generated.h"

class UHealthShieldWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSPROJECT_API UHealthShieldWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UHealthShieldWidgetComponent();
	virtual void BeginPlay() override;
	UHealthShieldWidget* GetHealthShieldBarWidget() const { return HealthShieldWidget; }
	void SetHealthShieldAttributeSet(UHealthShieldAttributeSet* HealthShieldAttributeSet);
protected:
	UHealthShieldWidget* HealthShieldWidget{nullptr};
};
