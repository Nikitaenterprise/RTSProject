#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ShipMovementComponent.generated.h"

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
class RTSPROJECT_API UShipMovementComponent : public UCharacterMovementComponent
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

	AShip* OwnerShip = nullptr;
	
	EShipMovementState MovementState;
	
public:
	UShipMovementComponent(const FObjectInitializer& ObjectInitializer);

	void RequestTurnTo(const FRotator _TargetRotation);
	void RequestSimpleMove(const FVector _TargetLocation);

};
