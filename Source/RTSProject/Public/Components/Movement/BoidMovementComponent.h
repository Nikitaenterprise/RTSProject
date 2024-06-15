#pragma once

#include "GameFramework/PawnMovementComponent.h"

#include "BoidMovementComponent.generated.h"

class IUnitMovementSystem;

UCLASS(ClassGroup=(Movement), meta=(BlueprintSpawnableComponent))
class RTSPROJECT_API UBoidMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
public:
	UBoidMovementComponent(const FObjectInitializer& ObjectInitializer);
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void AddInputVector(FVector WorldVector, bool bForce = false) override;
	virtual FVector ConsumeInputVector() override;

protected:
	UPROPERTY()
	TScriptInterface<IUnitMovementSystem> UnitMovingSystem {nullptr};

	// Normalized direction vector to shift this frame
	FVector InputVector = FVector(0, 0, 0);
};
