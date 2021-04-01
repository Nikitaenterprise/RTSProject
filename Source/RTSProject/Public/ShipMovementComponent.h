#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ShipMovementComponent.generated.h"

class ARTSPlayerController;
class ARTSAIController;
class AShip;

enum EShipMovementState
{
	Moving,
	Accelerating,
	Decelerating,
	Braking,
	Turning
};

UCLASS()
class RTSPROJECT_API UShipMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	float ForwardSpeed = 500;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	float TurnAngleSpeed = 5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Moving")
	float TurnForwardSpeed = ForwardSpeed * 0.3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	float MaxRollAngle = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	FRotator RotationRate = FRotator(15, 15, 15);
	
	TArray<FVector> NavPathCoords;

	AShip* OwnerShip = nullptr;
	ARTSPlayerController* PlayerController = nullptr;
	ARTSAIController* RTSAIController = nullptr;
private:


	FVector InputVector = FVector(0, 0, 0);
	
	FVector PointMoveTo;
	FRotator Rotator;

	EShipMovementState MovementState;
	
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
	
	void RequestTurnTo(const FRotator _TargetRotation);
	void CalculateMove();
	bool RequestNavMoving(const FVector _TargetLocation);
	inline void MakePathInXYPlane(float _setZToThisValue);
	inline void GetPoint();
};
