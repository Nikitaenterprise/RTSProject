#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "ShipMovementComponent.generated.h"

class ARTSPlayerController;
class ARTSAIController;
class AShip;

UCLASS()
class RTSPROJECT_API UShipMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:
	
	enum EShipAccelerationState
	{
		FULL_STOP,
		ACCELERATING,
		DECELERATING,
		CONSTANT_VELOCITY,
		BRAKING
	};
	static constexpr char* EShipAccelerationStateStr[] =
	{
		"FULL_STOP",
		"ACCELERATING",
		"DECELERATING",
		"CONSTANT_VELOCITY",
		"BRAKING"
	};

	enum EShipYawState
	{
		NO_TURNING,
		TURNING_WHILE_STANDING,
		TURNING_WHILE_MOVING
	};
	static constexpr char* EShipYawStateStr[] =
	{
		"NO_TURNING",
		"TURNING_WHILE_STANDING",
		"TURNING_WHILE_MOVING"
	};

	enum EShipRollState
	{
		NO_ROLLING,
		ROLLING,
		ROLL_TO_ZERO
	};
	static constexpr char* EShipRollStateStr[] =
	{
		"NO_ROLLING",
		"ROLLING",
		"ROLL_TO_ZERO"
	};

public:
	
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
	
	TArray<FVector> NavPathCoords;

	AShip* Owner = nullptr;
	ARTSPlayerController* PlayerController = nullptr;
	ARTSAIController* RTSAIController = nullptr;
	
private:
	enum ELineSegment
	{
		STRAIGHT_LINE,
		ARC_LINE
	};
	static constexpr char* ELineSegmentStr[] =
	{
		"StraightLine",
		"ArcLine"
	};
	class LineSegment
	{
	public:
		LineSegment(FVector StartPosition, FVector EndPosition, float Length, bool bClockwiseRotation) :
			StartPosition(StartPosition),
			EndPosition(EndPosition),
			Length(Length),
			bClockwiseRotation(bClockwiseRotation){}
		
		ELineSegment LineType;
		FVector StartPosition = FVector::ZeroVector;
		FVector EndPosition = FVector::ZeroVector;
		float Length = 0;
		bool bClockwiseRotation = true;
	};
	class StraightLine : public LineSegment
	{
	public:
		StraightLine(FVector StartPosition, FVector EndPosition, float Length, bool bClockwiseRotation, float Angle) :
			LineSegment(StartPosition, EndPosition, Length, bClockwiseRotation),
			Angle(Angle)
		{
			LineType = STRAIGHT_LINE;
		}
		
		float Angle = 0;
	};
	class ArcLine : public LineSegment
	{
	public:
		ArcLine(FVector StartPosition, FVector EndPosition, float Length, bool bClockwiseRotation, FVector2D CircleCenter, float StartingAngle, float TotalRadiansCover) :
			LineSegment(StartPosition, EndPosition, Length, bClockwiseRotation),
			CircleCenter(CircleCenter),
			StartingAngle(StartingAngle),
			TotalRadiansCover(TotalRadiansCover)
		{
			LineType = ARC_LINE;
		}

		FVector2D CircleCenter = FVector2D::ZeroVector;
		float StartingAngle = 0;
		float TotalRadiansCover = 0;
	};

	
	TArray<LineSegment*> LineSegments;
	LineSegment* CurrentLine = nullptr;

	// Normalized direction vector to shift this frame
	FVector InputVector = FVector(0, 0, 0);
	FRotator Rotator;
	
	FVector PointMoveTo;

	EShipAccelerationState AccelerationState = FULL_STOP;
	EShipYawState TurnState = NO_TURNING;
	EShipRollState RollState = NO_ROLLING;
	
	bool bShouldMove = false;
	bool bRequestedMove = false;
	bool bInitialMove = false;

public:
	
	UShipMovementComponent(const FObjectInitializer& ObjectInitializer);
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void AddInputVector(FVector WorldVector, bool bForce = false) override;
	virtual FVector ConsumeInputVector() override;

	TArray<FVector> GetNavPathCoords() const { return NavPathCoords; }
	
	bool RequestNavMoving(const FVector TargetLocation);
	void TurnOnCapsuleCollision(const bool TurnOn) const;

private:
	
	inline void ProcessStraightLine(float DeltaTime);
	inline void ProcessArcLine(float DeltaTime);
	
	inline void ReverceLineSegments();
	void BuildLineSegments();
	inline void MakePathInXYPlane(float SetZToThisValue);
	inline void CalculateForwardSpeed();
	inline void CalculateYawSpeed();
	inline void CalculateRoll();
};
