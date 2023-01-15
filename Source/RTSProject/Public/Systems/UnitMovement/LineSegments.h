#pragma once

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

class LineBuilder
{
public:
	static void BuildLineSegments(
		TArray<FLineSegment*>& OutLineSegments,
		const TArray<FVector>& Locations,
		const FVector& StartingLocation,
		const FRotator& StartingRotation,
		const FVector& ForwardVector,
		float MinTurnRadius);
};