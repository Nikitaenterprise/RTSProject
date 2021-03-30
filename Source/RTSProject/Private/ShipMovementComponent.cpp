#include "ShipMovementComponent.h"



#include "AnglesFunctions.h"
#include "Ship.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UShipMovementComponent::UShipMovementComponent(const FObjectInitializer& ObjectInitializer)
{
	bOrientRotationToMovement = false;
	RotationRate = FRotator(15, 30, 0);
	bUseRVOAvoidance = true;
	AvoidanceWeight = 0.5;
	NavAgentProps.bCanCrouch = false;
	NavAgentProps.bCanJump = false;
	NavAgentProps.bCanSwim = false;
	NavAgentProps.bCanFly = false;
	bUseAccelerationForPaths = true;
}

void UShipMovementComponent::RequestTurnTo(const FRotator _TargetRotation)
{
	FLatentActionInfo LInfo = FLatentActionInfo();
	LInfo.CallbackTarget = this;

	UKismetSystemLibrary::MoveComponentTo(
		OwnerShip->GetCapsuleComponent(),
		GetActorLocation(),
		_TargetRotation,
		true,
		true,
		OwnerShip->DeltaTime * TurnAngleSpeed,
		true,
		EMoveComponentAction::Move,
		LInfo);
}

void UShipMovementComponent::RequestSimpleMove(const FVector _TargetLocation)
{
	OwnerShip->bShouldMove = true;
	FVector Direction = _TargetLocation - GetActorLocation();
	Direction.Normalize();
	FVector ForwardVector = OwnerShip->GetActorForwardVector();
	ForwardVector.Normalize();

	float angle = AnglesFunctions::FindAngleBetweenVectorsOn2D(Direction, ForwardVector);
	if (angle > 5)
	{
		bool clockwise = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(Direction, ForwardVector);
		if (clockwise)
		{
			OwnerShip->SetActorRotation(FRotator(0, (OwnerShip->GetActorRotation().Yaw - (OwnerShip->DeltaTime * angle)), 0), ETeleportType::None);
		}
		else if (!clockwise)
		{
			OwnerShip->SetActorRotation(FRotator(0, (OwnerShip->GetActorRotation().Yaw + (OwnerShip->DeltaTime * angle)), 0), ETeleportType::None);
		}

		FVector2D NewForward(OwnerShip->GetActorForwardVector().X, OwnerShip->GetActorForwardVector().Y);

		NewForward = NewForward * TurnForwardSpeed * OwnerShip->DeltaTime;

		FVector2D NewLocation(GetActorLocation().X, GetActorLocation().Y);
		FVector location((NewLocation + NewForward).X, (NewLocation + NewForward).Y, GetActorLocation().Z);
		OwnerShip->SetActorLocation(location, false, nullptr, ETeleportType::None);
	}
	else
	{
		FVector2D NewForward(OwnerShip->GetActorForwardVector().X, OwnerShip->GetActorForwardVector().Y);

		NewForward = NewForward * ForwardSpeed * OwnerShip->DeltaTime;

		FVector2D NewLocation(GetActorLocation().X, GetActorLocation().Y);
		FVector location((NewLocation + NewForward).X, (NewLocation + NewForward).Y, GetActorLocation().Z);
		OwnerShip->SetActorLocation(location, false, nullptr, ETeleportType::None);
	}

	//if ((_TargetLocation - GetActorLocation()).SizeSquared() > 400) OwnerShip->bShouldMove = false;
}


