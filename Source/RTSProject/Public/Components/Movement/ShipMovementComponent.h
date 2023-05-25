#pragma once

#include "GameFramework/PawnMovementComponent.h"

#include "ShipMovementComponent.generated.h"

class ARTSPlayerController;
class ARTSAIController;
class AShip;
class UShipPathFollowingComponent;

UENUM()
enum class EShipMovementState : uint8
{
	Idle,
	Accelerate,
	Moving,
	Decelerate,
	Stopping
};

UENUM()
enum class EShipRollState : uint8
{
	No_Rolling,
	Rolling,
	Roll_To_Zero,
};

UCLASS(ClassGroup=(Movement), meta=(BlueprintSpawnableComponent))
class RTSPROJECT_API UShipMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
 	UShipMovementComponent(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void AddInputVector(FVector WorldVector, bool bForce = false) override;
	virtual FVector ConsumeInputVector() override;

	float GetMaxTurnRadius() const { return MaxTurnRadius; }

protected:
	void StartMoving();
	void CalculateSpeed(const float DistanceToTarget, const float DeltaTime);
	void CalculateRoll(FRotator& NextRotator, const FVector& NextLocation, const float DeltaTime);
	
	EShipMovementState MovementState;
	EShipRollState RollState = EShipRollState::No_Rolling;
	FRotator Rotator;
	
	UPROPERTY(EditAnywhere)
	float MaxVelocity = 1000.0f;

	UPROPERTY(EditAnywhere)
	float AccelerationRate = 70.0f;

	UPROPERTY(EditAnywhere)
	float AccelerationRollRate = 2;

	UPROPERTY(EditAnywhere)
	float DecelerationRate = 100.0f;

	UPROPERTY(EditAnywhere)
	float MaxSpeed = 800.f;

	float MinSpeedBeforeStopping = MaxSpeed / 10;

	UPROPERTY(EditAnywhere)
	float MaxRollSpeed = 5;

	float CurrentSpeed = 0.f;
	float CurrentRollSpeed = 0.f;

	UPROPERTY(EditAnywhere)
	float MinSpeed = 200.f;

	UPROPERTY(EditAnywhere)
	float TurnSpeed = 45.0f;

	UPROPERTY(EditAnywhere)
	float MaxTurnRadius = 300;

	UPROPERTY(EditAnywhere)
	float MaxRollAngle = 20;

	UPROPERTY(EditAnywhere)
	float DecelerationDistance = 500;
	
	// Normalized direction vector to shift this frame
	FVector InputVector = FVector(0, 0, 0);

	bool bStartedDecelerating = false;

	UPROPERTY()
	UShipPathFollowingComponent* PathFollowingComponent {nullptr};
};