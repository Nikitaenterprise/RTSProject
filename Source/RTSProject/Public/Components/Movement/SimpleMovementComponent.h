#pragma once

#include "GameFramework/PawnMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "SimpleMovementComponent.generated.h"

class AAIController;

UENUM()
enum class ESmallShipMovementState : uint8
{
	Idle,
	Accelerate,
	Moving,
	Decelerate,
	Stopping
};

UCLASS(ClassGroup=(Movement), meta=(BlueprintSpawnableComponent))
class RTSPROJECT_API USimpleMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	USimpleMovementComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void AddInputVector(FVector WorldVector, bool bForce = false) override;
	virtual FVector ConsumeInputVector() override;
	
protected:
	virtual void BeginPlay() override;
	void StartMoving();

	void CalculateSpeed(const float DistanceToTarget, const float DeltaTime);

	UPROPERTY(EditAnywhere)
	float AccelerationRate = 70.0f;
	
	UPROPERTY(EditAnywhere)
	float DecelerationRate = 100.0f;

	UPROPERTY(EditAnywhere)
	float MaxVelocity = 800.f;
	
	UPROPERTY(EditAnywhere)
	float MinSpeed = 200.f;

	UPROPERTY(EditAnywhere)
	float DecelerationDistance = 500;
	
	// Normalized direction vector to shift this frame
	FVector InputVector = FVector(0, 0, 0);

	float MinSpeedBeforeStopping = MaxVelocity / 10;

	float CurrentVelocity = 0.f;

	float DistanceToFinish;

	ESmallShipMovementState MovementState;

	UPROPERTY()
	AAIController* AIController = nullptr;
	
	UPROPERTY()
	UPathFollowingComponent* PathFollowingComponent = nullptr;
};
