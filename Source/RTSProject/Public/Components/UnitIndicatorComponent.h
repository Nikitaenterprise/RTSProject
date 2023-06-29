#pragma once

#include "Components/WidgetComponent.h"
#include "UnitIndicatorComponent.generated.h"


class UUnitIndicatorWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSPROJECT_API UUnitIndicatorComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UUnitIndicatorComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	UUnitIndicatorWidget* UnitIndicatorWidget {nullptr};
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUnitIndicatorAdditionalDescriptionWidget> AdditionalContextWidgetClass;
	
	UPROPERTY()
	UUnitIndicatorAdditionalDescriptionWidget* AdditionalContextWidget {nullptr};
};
