#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "TurretMovementComponent.generated.h"


UCLASS()
class RTSPROJECT_API UTurretMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

public:
	UTurretMovementComponent(const FObjectInitializer& ObjectInitializer);
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
