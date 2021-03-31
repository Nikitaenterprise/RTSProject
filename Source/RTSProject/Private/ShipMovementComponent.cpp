#include "ShipMovementComponent.h"
#include "RTSPlayerController.h"
#include "AnglesFunctions.h"
#include "Ship.h"
#include "RTSAIController.h"



#include "AIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UShipMovementComponent::UShipMovementComponent(const FObjectInitializer& ObjectInitializer)
{
	/*bOrientRotationToMovement = false;
	RotationRate = FRotator(15, 30, 0);
	bUseRVOAvoidance = true;
	AvoidanceWeight = 0.5;*/
	NavAgentProps.bCanCrouch = false;
	NavAgentProps.bCanJump = false;
	NavAgentProps.bCanSwim = false;
	NavAgentProps.bCanFly = false;
	bUseAccelerationForPaths = true;
}

void UShipMovementComponent::Initialize()
{
	
	RTSAIController = Cast<ARTSAIController>(OwnerShip->GetController());
	if(!RTSAIController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Red, TEXT("RTSAIController in ShipMovement is nullptr"));
	}
}

void UShipMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime)) return;

	FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f) * DeltaTime * 150.0f;
	if (!DesiredMovementThisFrame.IsNearlyZero())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);

		// If we bumped into something, try to slide along it
		/*if (Hit.IsValidBlockingHit())
		{
			SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
		}*/
	}
}

void UShipMovementComponent::RequestTurnTo(const FRotator _TargetRotation)
{
	FLatentActionInfo LInfo = FLatentActionInfo();
	LInfo.CallbackTarget = this;

	/*UKismetSystemLibrary::MoveComponentTo(
		OwnerShip->GetCapsuleComponent(),
		GetActorLocation(),
		_TargetRotation,
		true,
		true,
		OwnerShip->DeltaTime * TurnAngleSpeed,
		true,
		EMoveComponentAction::Move,
		LInfo);*/
}

void UShipMovementComponent::RequestSimpleMove(const FVector _TargetLocation)
{
	OwnerShip->bShouldMove = true;
	FVector Direction = _TargetLocation - GetActorLocation();
	Direction.Normalize();
	FVector ForwardVector = OwnerShip->GetActorForwardVector();
	ForwardVector.Normalize();
	
	FVector2D NewLocation;
	FVector location;

	const float angle = AnglesFunctions::FindAngleBetweenVectorsOn2D(Direction, ForwardVector);
	if (angle > 5)
	{
		const bool clockwise = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(Direction, ForwardVector);
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

		NewLocation = FVector2D(GetActorLocation().X, GetActorLocation().Y);
		location = FVector((NewLocation + NewForward).X, (NewLocation + NewForward).Y, GetActorLocation().Z);
		OwnerShip->SetActorLocation(location, false, nullptr, ETeleportType::None);
	}
	else
	{
		FVector2D NewForward(OwnerShip->GetActorForwardVector().X, OwnerShip->GetActorForwardVector().Y);

		NewForward = NewForward * ForwardSpeed * OwnerShip->DeltaTime;

		NewLocation = FVector2D(GetActorLocation().X, GetActorLocation().Y);
		location = FVector((NewLocation + NewForward).X, (NewLocation + NewForward).Y, GetActorLocation().Z);
		OwnerShip->SetActorLocation(location, false, nullptr, ETeleportType::None);
	}

	//if ((_TargetLocation - GetActorLocation()).SizeSquared() > 400) OwnerShip->bShouldMove = false;
}

bool UShipMovementComponent::RequestNavMoving(const FVector _TargetLocation)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(
										GetWorld(),
										OwnerShip->GetActorLocation(),
										_TargetLocation);
	if (!NavPath)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.1, FColor::Red, TEXT("Failed to create navigation"));
		return false;
	}
	
	NavPathCoords = NavPath->PathPoints;

	RTSAIController->MoveToLocation(_TargetLocation, 
										50,
										false, 
										true,
										true, 
										true,
										nullptr, 
										true);

	return true;
}
