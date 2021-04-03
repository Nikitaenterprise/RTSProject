#include "ShipMovementComponent.h"

//#include "RTSPlayerController.h"
#include "AnglesFunctions.h"
#include "Ship.h"
#include "RTSAIController.h"



#include "AIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
//#include "Components/CapsuleComponent.h"
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
	
	if (1)//bShouldMove)
	{
		FVector Direction = PointMoveTo - OwnerShip->GetActorLocation();
		const float DistanceToPoint = Direction.Size();

		AddInputVector(Direction);
		
		FVector ForwardVector = OwnerShip->GetActorForwardVector();

		FVector DesiredMovementThisFrame;
		
		const float angle = AnglesFunctions::FindAngleBetweenVectorsOn2D(Direction, ForwardVector);
		if (1)//bInitialMove)
		{
			bInitialMove = false;
			FVector2D CenterOfTurningCircle = FVector2D::ZeroVector;
			const float AngleToCenterOfTurningCircle = OwnerShip->GetActorForwardVector().GetSafeNormal().Rotation().Yaw - 90;
			CenterOfTurningCircle.X = OwnerShip->GetActorLocation().X + MaxTurnRadius * cos(AngleToCenterOfTurningCircle);
			CenterOfTurningCircle.Y = OwnerShip->GetActorLocation().Y + MaxTurnRadius * sin(AngleToCenterOfTurningCircle);
			float LengthOfStraightPart = sqrt(DistanceToPoint * DistanceToPoint - MaxTurnRadius * MaxTurnRadius);
			float theta = UKismetMathLibrary::DegAcos(MaxTurnRadius / DistanceToPoint);
			float phi = UKismetMathLibrary::DegAtan(Direction.Y / Direction.X);
			FVector2D LeavingCirclePoint = FVector2D(
				CenterOfTurningCircle.X - MaxTurnRadius * cos(phi + theta),
				CenterOfTurningCircle.Y - MaxTurnRadius * sin(phi + theta)
			);

			FString out = "";
			out += FString("\nPointMoveTo " + PointMoveTo.ToString());
			out += FString("\nActorLocation " + OwnerShip->GetActorLocation().ToString());
			out += FString("\nbInitialMove " + bInitialMove ? "true" : "false");
			out += FString("\nCenterOfTurningCircle " + CenterOfTurningCircle.ToString());
			out += FString("\nAngleToCenterOfTurningCircle " + FString::SanitizeFloat(AngleToCenterOfTurningCircle));
			out += FString("\nDistanceToPoint " + FString::SanitizeFloat(DistanceToPoint));
			out += FString("\nLengthOfStraightPart " + FString::SanitizeFloat(LengthOfStraightPart));
			out += FString("\ntheta " + FString::SanitizeFloat(theta) + " phi " + FString::SanitizeFloat(phi));
			out += FString("\nLeavingCirclePoint " + LeavingCirclePoint.ToString());
			GEngine->AddOnScreenDebugMessage(-1, 0.01, FColor::Green, out);
			
			if (DistanceToPoint < MaxTurnRadius)
			{
				
			}
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
	for (auto& point : NavPathCoords)
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
			
			FString from = OwnerShip->GetActorLocation().ToString();
			FString to = PointMoveTo.ToString();
			GEngine->AddOnScreenDebugMessage(-1, 5.1, FColor::Yellow, FString::Printf(TEXT("Move started from %s to %s"), *from, *to));
			
		}
		// Else af finish position
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.1, FColor::Yellow, TEXT("Finished move"));
			bShouldMove = false;
			bRequestedMove = false;
		}
	}
	
}