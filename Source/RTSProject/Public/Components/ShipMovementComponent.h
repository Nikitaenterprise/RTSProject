#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "ShipMovementComponent.generated.h"

class ARTSPlayerController;
class ARTSAIController;
class AShip;

namespace MovementComponent
{
UENUM(BlueprintType)
enum class EShipAccelerationState : uint8
{
	FULL_STOP,
	ACCELERATING,
	DECELERATING,
	CONSTANT_VELOCITY,
	BRAKING,

	COUNT
};

UENUM()
enum class EShipYawState : uint8
{
	NO_TURNING,
	TURNING_WHILE_STANDING,
	TURNING_WHILE_MOVING,
	
	COUNT
};

UENUM()
enum class EShipRollState : uint8
{
	NO_ROLLING,
	ROLLING,
	ROLL_TO_ZERO,
	
	COUNT
};

UENUM()
enum class ELineSegment : uint8
{
	STRAIGHT_LINE,
	ARC_LINE,
	
	COUNT
};
	
class FLineSegment
{
public:
	FLineSegment(const FVector StartPosition, const FVector EndPosition, const float Length, const bool bClockwiseRotation) :
		StartPosition(StartPosition),
		EndPosition(EndPosition),
		Length(Length),
		bClockwiseRotation(bClockwiseRotation)
	{
		LineType = ELineSegment::COUNT;
	}
	
	ELineSegment LineType;
	FVector StartPosition = FVector::ZeroVector;
	FVector EndPosition = FVector::ZeroVector;
	float Length = 0;
	bool bClockwiseRotation = true;
};
	
class FStraightLine : public FLineSegment
{
public:
	FStraightLine(const FVector StartPosition, const FVector EndPosition, const float Length, const bool bClockwiseRotation, const float Angle) :
		FLineSegment(StartPosition, EndPosition, Length, bClockwiseRotation),
		Angle(Angle)
	{
		LineType = ELineSegment::STRAIGHT_LINE;
	}
	
	float Angle = 0;
};
class FArcLine : public FLineSegment
{
public:
	FArcLine(const FVector StartPosition, const FVector EndPosition, const float Length, const bool bClockwiseRotation, const FVector2D CircleCenter, const float StartingAngle, float TotalRadiansCover) :
		FLineSegment(StartPosition, EndPosition, Length, bClockwiseRotation),
		CircleCenter(CircleCenter),
		StartingAngle(StartingAngle),
		TotalRadiansCover(TotalRadiansCover)
	{
		LineType = ELineSegment::ARC_LINE;
	}

	FVector2D CircleCenter = FVector2D::ZeroVector;
	float StartingAngle = 0;
	float TotalRadiansCover = 0;
};
}

UCLASS()
class RTSPROJECT_API UShipMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	int Mass = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	float AcceptanceRadius = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	float AccelerationForwardRate = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	float AccelerationYawRate = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	float AccelerationRollRate = 2;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Moving")
	float CurrentForwardSpeed = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Moving")
	float CurrentYawSpeed = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Moving")
	float CurrentRollSpeed = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	float MaxForwardSpeed = 500;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	float MaxYawSpeed = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	float MaxRollSpeed = 5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Moving")
	float TurnForwardSpeed = MaxForwardSpeed * 0.3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	float MaxRollAngle = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	float MinTurnRadius = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	FRotator RotationRate = FRotator(15, 15, 15);

	FVector PointMoveTo;
	// Normalized direction vector to shift this frame
	FVector InputVector = FVector(0, 0, 0);
	FRotator Rotator;
	TArray<MovementComponent::FLineSegment*> LineSegments;
	MovementComponent::FLineSegment* CurrentLine = nullptr;
	MovementComponent::EShipAccelerationState AccelerationState = MovementComponent::EShipAccelerationState::FULL_STOP;
	MovementComponent::EShipYawState TurnState = MovementComponent::EShipYawState::NO_TURNING;
	MovementComponent::EShipRollState RollState = MovementComponent::EShipRollState::NO_ROLLING;
	
	bool bShouldMove = false;
	bool bRequestedMove = false;
	bool bInitialMove = false;
	
	TArray<FVector> NavPathCoords;
	UPROPERTY()
	AShip* Owner {nullptr};
	UPROPERTY()
	ARTSPlayerController* PlayerController {nullptr};
	UPROPERTY()
	ARTSAIController* RTSAIController {nullptr};
	
public:
	UShipMovementComponent(const FObjectInitializer& ObjectInitializer);
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void AddInputVector(FVector WorldVector, bool bForce = false) override;
	virtual FVector ConsumeInputVector() override;

	TArray<FVector> GetNavPathCoords() const { return NavPathCoords; }
	
	bool RequestNavMoving(const FVector TargetLocation);
	void TurnOnCapsuleCollision(const bool TurnOn) const;

protected:
	inline void ProcessStraightLine(float DeltaTime);
	inline void ProcessArcLine(float DeltaTime);
	
	inline void ReverseLineSegments();
	void BuildLineSegments();
	inline void MakePathInXYPlane(float SetZToThisValue);
	inline void CalculateForwardSpeed();
	inline void CalculateYawSpeed();
	inline void CalculateRoll();
};
