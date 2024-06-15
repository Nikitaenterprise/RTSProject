#pragma once

#include "LineSegments.h"
#include "UObject/Object.h"
#include "Interfaces/UnitMovementSystem.h"
#include "ComplexUnitMovementSystem.generated.h"

class UCapsuleComponent;
class UPawnMovementComponent;

UCLASS()
class RTSPROJECT_API UComplexUnitMovementSystem : public UObject, public IUnitMovementSystem
{
	GENERATED_BODY()

public:
	UComplexUnitMovementSystem(const FObjectInitializer& ObjectInitializer);
	
	// IUnitMovementSystem Begin
	virtual void InitializeMovementSystem(AActor* InOwnerActor, UPawnMovementComponent* InMovementComponent) override;
	virtual const FRotator& GetRotatorForTick() override { return Rotator; }
	virtual const FVector& GetLocationForTick() override { return LocationForTick; }
	virtual bool RequestMoveTo(const FVector& Location) override;
	virtual bool IsUsingCapsuleCollision() override;
	virtual bool ShouldMove() override { return bShouldMove; }
	virtual bool IsMovingInPlane() override { return true; }
	virtual void CalculatePosition(float DeltaTime) override;
	// IUnitMovementSystem End
	
protected:
	// IUnitMovementSystem Begin
	virtual void MakePath() override;
	// IUnitMovementSystem End
	
	bool RequestNavMoving(const FVector& TargetLocation);
	void TurnOnCapsuleCollision(const bool TurnOn) const;
	void ProcessStraightLine(float DeltaTime);
	void ProcessArcLine(float DeltaTime);
	void ReverseLineSegments();
	void CalculateForwardSpeed();
	void CalculateYawSpeed();
	void CalculateRoll();

	UPROPERTY()
	UWorld* World {nullptr};
	UPROPERTY()
	AActor* OwnerActor {nullptr};
	UPROPERTY()
	UPawnMovementComponent* MovementComponent {nullptr};
	UPROPERTY()
	UCapsuleComponent* CapsuleComponent {nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComplexUnitMovementSystem")
	int Mass = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComplexUnitMovementSystem")
	float AcceptanceRadius = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComplexUnitMovementSystem")
	float AccelerationForwardRate = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComplexUnitMovementSystem")
	float AccelerationYawRate = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComplexUnitMovementSystem")
	float AccelerationRollRate = 2;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ComplexUnitMovementSystem")
	float CurrentForwardSpeed = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ComplexUnitMovementSystem")
	float CurrentYawSpeed = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ComplexUnitMovementSystem")
	float CurrentRollSpeed = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComplexUnitMovementSystem")
	float MaxForwardSpeed = 500;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComplexUnitMovementSystem")
	float MaxYawSpeed = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComplexUnitMovementSystem")
	float MaxRollSpeed = 5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ComplexUnitMovementSystem")
	float TurnForwardSpeed = MaxForwardSpeed * 0.3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComplexUnitMovementSystem")
	float MaxRollAngle = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComplexUnitMovementSystem")
	float MinTurnRadius = 300;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComplexUnitMovementSystem")
	FRotator RotationRate = FRotator(15, 15, 15);

	FVector LocationForTick = FVector::ZeroVector;
	FRotator RotationForTick = FRotator::ZeroRotator;
	FVector LocationToMove;
	FRotator Rotator;
	TArray<FLineSegment*> LineSegments;
	FLineSegment* CurrentLine = nullptr;
	EShipAccelerationState AccelerationState = EShipAccelerationState::FULL_STOP;
	EShipYawState TurnState = EShipYawState::NO_TURNING;
	EShipRollState1 RollState = EShipRollState1::NO_ROLLING;
	
	bool bShouldMove = false;
	bool bRequestedMove = false;
	bool bInitialMove = false;
	
	TArray<FVector> NavigationPathCoordinates;
};
