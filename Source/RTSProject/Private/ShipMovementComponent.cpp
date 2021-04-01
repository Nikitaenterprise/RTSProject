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
#include "Kismet/KismetMathLibrary.h"

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

	GetPoint();
	
	if (bShouldMove)
	{
		FVector Direction = PointMoveTo - OwnerShip->GetActorLocation();
		Direction.Normalize();
		AddInputVector(Direction);
		
		FVector ForwardVector = OwnerShip->GetActorForwardVector();
		ForwardVector.Normalize();

		FVector DesiredMovementThisFrame;
		
		const float angle = AnglesFunctions::FindAngleBetweenVectorsOn2D(Direction, ForwardVector);
		if (bInitialMove)
		{
			bInitialMove = false;
		}
		else
		{
			
		}
		if (angle > 5)
		{
			const bool clockwise = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(Direction, ForwardVector);
			if (clockwise)
			{
				Rotator = FRotator(0,
					OwnerShip->GetActorRotation().Yaw - DeltaTime * angle,
					0);
			}
			else if (!clockwise)
			{
				Rotator = FRotator(0, 
					OwnerShip->GetActorRotation().Yaw + DeltaTime * angle,
					0);
			}
			/*FinalVector = FVector
			(
				GetActorLocation().X + OwnerShip->GetActorForwardVector().X * TurnForwardSpeed * DeltaTime,
				GetActorLocation().Y + OwnerShip->GetActorForwardVector().Y * TurnForwardSpeed * DeltaTime,
				0
			);*/
			DesiredMovementThisFrame = ConsumeInputVector() * DeltaTime * TurnForwardSpeed;
			DesiredMovementThisFrame.Z = 0;
		}
		else
		{
			DesiredMovementThisFrame = ConsumeInputVector() * DeltaTime * ForwardSpeed;
			DesiredMovementThisFrame.Z = 0;
			/*FinalVector = FVector
			(
				GetActorLocation().X + OwnerShip->GetActorForwardVector().X * ForwardSpeed * DeltaTime,
				GetActorLocation().Y + OwnerShip->GetActorForwardVector().Y * ForwardSpeed * DeltaTime,
				0
			);*/
		}
		
		
		
		//if ((_TargetLocation - GetActorLocation()).SizeSquared() > 400) OwnerShip->bShouldMove = false;

		
		
		 
		if (!DesiredMovementThisFrame.IsNearlyZero())
		{
			FHitResult Hit;
			SafeMoveUpdatedComponent(DesiredMovementThisFrame, Rotator, true, Hit);

			// If we bumped into something, try to slide along it
			/*if (Hit.IsValidBlockingHit())
			{
				SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
			}*/
		}
	}
}

void UShipMovementComponent::AddInputVector(FVector WorldVector, bool bForce)
{
	WorldVector.Normalize();
	InputVector += WorldVector;
}

FVector UShipMovementComponent::ConsumeInputVector()
{
	FVector ReturnVector = InputVector;
	InputVector = FVector::ZeroVector;
	return ReturnVector;
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

void UShipMovementComponent::CalculateMove()
{
	
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
	MakePathInXYPlane(OwnerShip->GetActorLocation().Z);
	//GEngine->AddOnScreenDebugMessage(-1, 1.1, FColor::Yellow, OwnerShip->GetActorLocation().ToString());

	bRequestedMove = true;
	bInitialMove = true;
	return true;
}

void UShipMovementComponent::MakePathInXYPlane(float _setZToThisValue)
{
	for (auto& point: NavPathCoords)
	{
		/*FString str1 = point.ToString();
		GEngine->AddOnScreenDebugMessage(-1, 1.1, FColor::Green, str1);*/
		point.Z = _setZToThisValue;
	}
}

void UShipMovementComponent::GetPoint()
{
	// If near destination
	if(OwnerShip->GetActorLocation().Equals(PointMoveTo, 0.1) || bRequestedMove)
	{
		// Get next destination point
		if(NavPathCoords.Num()>0)
		{
			PointMoveTo = NavPathCoords.Pop(false);
			bShouldMove = true;
			
		}
		// Else af finish position
		else
		{
			bShouldMove = false;
			bRequestedMove = false;
		}
	}
	
}