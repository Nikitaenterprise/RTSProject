#pragma once

#include "CoreMinimal.h"
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
		FullStop,
		Accelerating,
		Decelerating,
		Braking
	};
	static constexpr char* EShipAccelerationStateStr[] =
	{
		"FullStop",
		"Accelerating",
		"Decelerating",
		"Braking"
	};

	enum EShipYawState
	{
		NoTurning,
		TurningWhileStanding,
		TurningWhileMoving
	};
	static constexpr char* EShipYawStateStr[] =
	{
		"NoTurning",
		"TurningWhileStanding",
		"TurningWhileMoving"
	};

	enum EShipRollState
	{
		NoRolling,
		Rolling,
		RollToZero
	};
	static constexpr char* EShipRollStateStr[] =
	{
		"NoRolling",
		"Rolling",
		"RollToZero"
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

	AShip* OwnerShip = nullptr;
	ARTSPlayerController* PlayerController = nullptr;
	ARTSAIController* RTSAIController = nullptr;
	
private:
	
	class LineSegment
	{
	public:
		LineSegment(FVector StartPosition, float Length) :
			StartPosition(StartPosition),
			Length(Length) {}
		
		FVector StartPosition = FVector::ZeroVector;
		float Length = 0;
	};
	class StraightLine : public LineSegment
	{
	public:
		StraightLine(FVector StartPosition, float Length, float Angle) :
			LineSegment(StartPosition, Length),
			Angle(Angle) {}
		
		float Angle = 0;
	};
	class ArcLine : public LineSegment
	{
	public:
		ArcLine(FVector StartPosition, float Length, FVector2D CircleCenter, float StartingAngle, float TotalRadiansCover, bool bRotateClockwise) :
			LineSegment(StartPosition, Length),
			CircleCenter(CircleCenter),
			StartingAngle(StartingAngle),
			TotalRadiansCover(TotalRadiansCover),
			bRotateClockwise(bRotateClockwise) {}
		
		FVector2D CircleCenter = FVector2D::ZeroVector;
		float StartingAngle = 0;
		float TotalRadiansCover = 0;
		bool bRotateClockwise = true;
	};
	TArray<LineSegment> LineSegments;
	
	FVector InputVector = FVector(0, 0, 0);
	
	FVector PointMoveTo;
	FRotator Rotator;

	EShipAccelerationState AccelerationState = FullStop;
	EShipYawState TurnState = NoTurning;
	EShipRollState RollState = NoRolling;
	
	bool bShouldMove = false;
	bool bRequestedMove = false;
	bool bInitialMove = false;

	
public:
	UShipMovementComponent(const FObjectInitializer& ObjectInitializer);
	void Initialize();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void AddInputVector(FVector WorldVector, bool bForce = false) override;
	virtual FVector ConsumeInputVector() override;

	TArray<FVector> GetNavPathCoords() const { return NavPathCoords; }
	
	bool Walkable(FVector FirstPoint, FVector SecondPoint);
	void RequestTurnTo(const FRotator _TargetRotation);
	void CalculateMove();
	void CalculateRotation();
	bool RequestNavMoving(const FVector _TargetLocation);
	inline void ReverceNavPath();
	void BuildLineSegments();
	inline void MakePathInXYPlane(float _setZToThisValue);
	inline void GetPoint();
};
