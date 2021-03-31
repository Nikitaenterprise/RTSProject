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
	
	AShip* OwnerShip = nullptr;
	ARTSPlayerController* PlayerController = nullptr;
	ARTSAIController* RTSAIController = nullptr;
	
	EShipMovementState MovementState;
	TArray<FVector> NavPathCoords;
	
public:
	UShipMovementComponent(const FObjectInitializer& ObjectInitializer);
	void Initialize();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RequestTurnTo(const FRotator _TargetRotation);
	void RequestSimpleMove(const FVector _TargetLocation);
	bool RequestNavMoving(const FVector _TargetLocation);
};
