#include "Systems/UnitMovement/ComplexUnitMovementSystem.h"

#include "DrawDebugHelpers.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Actors/Units/Ship.h"
#include "Components/CapsuleComponent.h"
#include "Core/RTSPlayerController.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Miscellaneous/AnglesFunctions.h"
#include "Systems/RTSPlayerState.h"

UComplexUnitMovementSystem::UComplexUnitMovementSystem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UComplexUnitMovementSystem::InitializeMovementSystem(AActor* InOwnerActor, UPawnMovementComponent* InMovementComponent)
{
	OwnerActor = InOwnerActor;
	MovementComponent = InMovementComponent;
	World = OwnerActor->GetWorld();
	// Ship shouldn't move
	LocationToMove = OwnerActor->GetActorLocation();
}

bool UComplexUnitMovementSystem::RequestMoveTo(const FVector& Location)
{
	RequestNavMoving(Location);
	MakePath();
	return true;
}

bool UComplexUnitMovementSystem::IsUsingCapsuleCollision()
{
	if (const auto* Ship = Cast<AShip>(OwnerActor))
	{
		CapsuleComponent = Ship->GetCapsuleComponent();
		return true;
	}
	return false;
}

void UComplexUnitMovementSystem::MakePath()
{
	LineSegments.Empty();
	LineBuilder::BuildLineSegments(
		LineSegments,
		NavigationPathCoordinates,
		OwnerActor->GetActorLocation(),
		OwnerActor->GetActorRotation(),
		OwnerActor->GetActorForwardVector(),
		MinTurnRadius);
	ReverseLineSegments();

	CurrentLine = nullptr;
	bShouldMove = true;
}

void UComplexUnitMovementSystem::CalculatePosition(float DeltaTime)
{
	if (CurrentLine == nullptr || OwnerActor->GetActorLocation().Equals(CurrentLine->EndPosition, AcceptanceRadius))
	{
		if (LineSegments.Num() == 0)
		{
			bShouldMove = false;
			//GEngine->AddOnScreenDebugMessage(-1, 10.01, FColor::Red, TEXT("LineSegments in empty in ShipMovementComponent"));
			return;
		}
		CurrentLine = LineSegments.Pop(false);
	}
	
	switch(CurrentLine->LineType)
	{
	case ELineSegment::STRAIGHT_LINE:		ProcessStraightLine(DeltaTime);		break;
	case ELineSegment::ARC_LINE:			ProcessArcLine(DeltaTime);			break;
	default:																	checkNoEntry();
	}
	
	if (RollState == EShipRollState1::ROLL_TO_ZERO)
	{
		// If ship has some roll angle then it should be counter-rolled
		const float CurrentRoll = OwnerActor->GetActorRotation().Roll;
		if (FMath::Abs(CurrentRoll) > 0)
		{
			float DeltaRoll = CurrentRoll > 0 ? -1 : 1;
			DeltaRoll *= CurrentRollSpeed * DeltaTime;
			Rotator.Roll = OwnerActor->GetActorRotation().Roll + DeltaRoll;
		}
		else
		{
			Rotator.Roll = 0;
			RollState = EShipRollState1::NO_ROLLING;
		}
	}
	
	LocationForTick = OwnerActor->GetActorForwardVector() * DeltaTime * CurrentForwardSpeed;
	LocationForTick.Z = 0;
	
	CalculateForwardSpeed();
	CalculateYawSpeed();
	CalculateRoll();
		
	/*FString out = "";
	out += FString("\nLineType: ") + FString(ELineSegmentStr[CurrentLine->LineType]);
	out += FString("\nAccelerationState: ") + FString(EShipAccelerationStateStr[AccelerationState]);
	out += FString("\nTurnState: ") + FString(EShipYawStateStr[TurnState]) + FString(" ");
	out += FString("\nRollState: ") + FString(EShipRollStateStr[RollState]) + FString(" ");
	GEngine->AddOnScreenDebugMessage(-1, 0.01, FColor::Green, out);*/
}

void UComplexUnitMovementSystem::ProcessStraightLine(float DeltaTime)
{
	const float YawToDestination = AnglesFunctions::FindAngleBetweenVectorsOn2D(
		OwnerActor->GetActorForwardVector(),
		(CurrentLine->EndPosition - OwnerActor->GetActorLocation()).GetSafeNormal());
	const bool bClockwiseRotation = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(
		OwnerActor->GetActorForwardVector(),
		(CurrentLine->EndPosition - OwnerActor->GetActorLocation()).GetSafeNormal());

	float DeltaYaw = YawToDestination * CurrentYawSpeed * DeltaTime;
	DeltaYaw = DeltaYaw > 0.5 ? 0.5 : DeltaYaw;
	DeltaYaw *= bClockwiseRotation ? 1 : -1;

	const float DistanceToPoint = (CurrentLine->EndPosition - OwnerActor->GetActorLocation()).Size();

	/*FString out = "";
	out += FString("\n YawToDestination= ") + FString::SanitizeFloat(YawToDestination);
	out += FString("\n ActorRotation= ") + FString::SanitizeFloat(Owner->GetActorRotation().Yaw);
	out += FString("\n DeltaYaw= ") + FString::SanitizeFloat(DeltaYaw);
	out += FString("\n bClockwiseRotation= ") + (bClockwiseRotation ? TEXT("true") : TEXT("false"));
	GEngine->AddOnScreenDebugMessage(-1, 0.01, FColor::Green, out);*/

	// Angle between point and forward vector is >45
	if (YawToDestination >= 45)
	{
		// Ship is standing
		if (CurrentForwardSpeed == 0)
		{
			TurnState = EShipYawState::TURNING_WHILE_STANDING;
		}
		// Ship is accelerating 
		else if (CurrentForwardSpeed >= 0.5 * MaxForwardSpeed)
		{
			AccelerationState = EShipAccelerationState::DECELERATING;
			TurnState = EShipYawState::TURNING_WHILE_MOVING;
		}
		else if (CurrentForwardSpeed < 0.5 * MaxForwardSpeed && CurrentForwardSpeed >= 0.2 * MaxForwardSpeed)
		{
			AccelerationState = EShipAccelerationState::ACCELERATING;
			TurnState = EShipYawState::TURNING_WHILE_MOVING;
		}
		else if (CurrentForwardSpeed < 0.2 * MaxForwardSpeed)
		{
			AccelerationState = EShipAccelerationState::CONSTANT_VELOCITY;
			TurnState = EShipYawState::TURNING_WHILE_MOVING;
		}
	}
	// Angle between point and forward vector is <45
	else if (YawToDestination < 45 && YawToDestination > 1)
	{
		// If ship is turning while standing still
		if (CurrentForwardSpeed == 0 && TurnState == EShipYawState::TURNING_WHILE_STANDING)
		{
			// Ship can start moving and turning at the same time
			AccelerationState = EShipAccelerationState::ACCELERATING;
		}
		else if (CurrentForwardSpeed < 0.5 * MaxForwardSpeed && CurrentForwardSpeed >= 0.2 * MaxForwardSpeed)
		{
			AccelerationState = EShipAccelerationState::ACCELERATING;
		}
		else if (CurrentForwardSpeed > 0.5 * MaxForwardSpeed)
		{
			AccelerationState = EShipAccelerationState::CONSTANT_VELOCITY;
		}
		TurnState = EShipYawState::TURNING_WHILE_MOVING;
		RollState = EShipRollState1::ROLLING;
	}
	else if (YawToDestination <= 1)
	{
		if (CurrentForwardSpeed == 0)
		{
			AccelerationState = EShipAccelerationState::ACCELERATING;
		}
		else if (CurrentForwardSpeed < 0.5 * MaxForwardSpeed && CurrentForwardSpeed >= 0.2 * MaxForwardSpeed)
		{
			AccelerationState = EShipAccelerationState::ACCELERATING;
		}
		else if (CurrentForwardSpeed < 0.2 * MaxForwardSpeed)
		{
			AccelerationState = EShipAccelerationState::CONSTANT_VELOCITY;
		}

		TurnState = EShipYawState::NO_TURNING;
		RollState = EShipRollState1::ROLL_TO_ZERO;
		DeltaYaw = 0;
	}

	Rotator.Yaw = OwnerActor->GetActorRotation().Yaw + DeltaYaw;

	if (DistanceToPoint < 15 * AcceptanceRadius)
	{
		AccelerationState = EShipAccelerationState::DECELERATING;
	}
	if (DistanceToPoint < 10 * AcceptanceRadius)
	{
		AccelerationState = EShipAccelerationState::BRAKING;
	}
	if (DistanceToPoint <= AcceptanceRadius)
	{
		AccelerationState = EShipAccelerationState::FULL_STOP;
		TurnState = EShipYawState::NO_TURNING;
		RollState = EShipRollState1::ROLL_TO_ZERO;
	}
}

void UComplexUnitMovementSystem::ProcessArcLine(float DeltaTime)
{
	FArcLine* ArcSegment = static_cast<FArcLine*>(CurrentLine);
	// Determine current angle on arc (AngleOnCircle) by adding or
	// subtracting 90 degrees to the starting angle
	// depending on whether turning to the right or left
	const float AngleOnCircle = OwnerActor->GetActorRotation().Yaw + (ArcSegment->bClockwiseRotation ? -90 : 90);

	// Determine delta angle (DeltaYaw) which is added in this tick
	// da = omega*dt, omega = v/R, dr = v*dt, dr = 2R*sin(da/2) 
	const float DeltaYaw = UKismetMathLibrary::DegAsin(CurrentForwardSpeed / MinTurnRadius * DeltaTime / 2) * 2 * (ArcSegment->bClockwiseRotation ? 1 : -1);

	// Calculate dr = CircleCenter - r + position on circle 
	//DeltaR.X = ArcSegment->CircleCenter.X - OwnerShip->GetActorLocation().X + MinTurnRadius * UKismetMathLibrary::DegCos(AngleOnCircle + DeltaYaw);
	//DeltaR.Y = ArcSegment->CircleCenter.Y - OwnerShip->GetActorLocation().Y + MinTurnRadius * UKismetMathLibrary::DegSin(AngleOnCircle + DeltaYaw);

	// Determine current direction (Rotator.Yaw) by adding dA
	// to actor's rotation
	Rotator.Yaw = OwnerActor->GetActorRotation().Yaw + DeltaYaw;

	// Calculating banked turn
	// tan(DeltaRoll) = v^2 / r / g
	float DeltaRoll = UKismetMathLibrary::DegAtan(CurrentForwardSpeed * CurrentForwardSpeed * DeltaTime * DeltaTime / MinTurnRadius / 9.81 * (ArcSegment->bClockwiseRotation ? 1 : -1));
	// Divide by 2 for slower banked turn
	DeltaRoll /= 2;
	Rotator.Roll = OwnerActor->GetActorRotation().Roll + DeltaRoll;
	// Set roll value in between +-MaxRollAngle
	Rotator.Roll = Rotator.Roll > MaxRollAngle ? MaxRollAngle : (Rotator.Roll < -MaxRollAngle ? -MaxRollAngle : Rotator.Roll);
	
	// Determine acceleration state
	AccelerationState = EShipAccelerationState::ACCELERATING;
	TurnState = EShipYawState::TURNING_WHILE_MOVING;
	RollState = EShipRollState1::ROLLING;

	DrawDebugCircle(
		World,
		FVector(ArcSegment->CircleCenter.X, ArcSegment->CircleCenter.Y, 150),
		MinTurnRadius,
		36,
		FColor::Purple,
		false,
		0.1,
		0,
		5,
		FVector(0, 1, 0),
		FVector(1, 0, 0));

	/*FString out = "";
	out += FString("\n AngleOnCircle= ") + FString::SanitizeFloat(AngleOnCircle);
	out += FString("\n CurrentPosition= ") + FVector2D(OwnerShip->GetActorLocation()).ToString();
	out += FString("\n dA= ") + FString::SanitizeFloat(da);
	out += FString("\n Rotator.Yaw= ") + FString::SanitizeFloat(Rotator.Yaw);
	GEngine->AddOnScreenDebugMessage(-1, 0.01, FColor::Green, out);*/
}

bool UComplexUnitMovementSystem::RequestNavMoving(const FVector& TargetLocation)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);

	if (CapsuleComponent && CapsuleComponent->bDynamicObstacle == false)
	{
		FString ErrorMsg = "Dynamic obstacle in capsule component in " + OwnerActor->GetName() + " is false";
		ErrorMsg += "\nNavigation path would be incorrect";
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, *ErrorMsg);
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
	}
	
	TurnOnCapsuleCollision(true);
	NavSys->Build();
	TurnOnCapsuleCollision(false);

	/*float r, h;
	Owner->GetSimpleCollisionCylinder(r, h);
	FString out = "";
	out += "Radius " + FString::SanitizeFloat(r);
	out += " Height " + FString::SanitizeFloat(h);*/
	
	FPathFindingQuery Query;
	Query.StartLocation = OwnerActor->GetActorLocation();
	Query.EndLocation = TargetLocation;
	Query.NavData = NavSys->GetDefaultNavDataInstance();
	Query.SetNavAgentProperties(MovementComponent->GetNavAgentPropertiesRef());
	
	UNavigationPath* NavPath = NewObject<UNavigationPath>(NavSys);
	NavPath->SetPath(NavSys->FindPathSync(MovementComponent->GetNavAgentPropertiesRef(), Query).Path);
	
	if (!NavPath)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.1, FColor::Red, TEXT("Failed to create navigation"));
		UE_LOG(LogTemp, Error, TEXT("Failed to create navigation"));
		return false;
	}
	
	NavigationPathCoordinates = NavPath->PathPoints;
	// Make path in XY plane
	for (auto& Point : NavigationPathCoordinates)       
	{                                       
		Point.Z = OwnerActor->GetActorLocation().Z;          
	}
	
	return true;
}

void UComplexUnitMovementSystem::TurnOnCapsuleCollision(const bool TurnOn) const
{
	for (auto& Actor : Cast<AShip>(OwnerActor)->GetPlayerState<ARTSPlayerState>()->GetPlayersUnits())
	{
		AShip* Ship = Cast<AShip>(Actor);
		if (Ship && Ship != OwnerActor)
		{
			if (Ship->GetCapsuleComponent()->bDynamicObstacle == false)
			{
				FString ErrorMsg = "Dynamic obstacle in capsule component in " + Ship->GetName() + " is false";
				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, *ErrorMsg);
				UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
			}
			Ship->GetCapsuleComponent()->SetCanEverAffectNavigation(TurnOn);
		}
	}
}

void UComplexUnitMovementSystem::ReverseLineSegments()
{
	TArray<FLineSegment*> TemporaryLineSegments;
	TemporaryLineSegments.Reserve(LineSegments.Num());
	for (size_t i = LineSegments.Num(); i > 0; i--)
	{
		TemporaryLineSegments.Add(LineSegments[i - 1]);
	}
	LineSegments = TemporaryLineSegments;
}

void UComplexUnitMovementSystem::CalculateForwardSpeed()
{
	switch (AccelerationState)
	{
	case EShipAccelerationState::FULL_STOP:
		CurrentForwardSpeed = 0;
		break;
	case EShipAccelerationState::ACCELERATING:
		if (CurrentForwardSpeed < MaxForwardSpeed)
		{
			CurrentForwardSpeed += AccelerationForwardRate;
		}
		else
		{
			CurrentForwardSpeed = MaxForwardSpeed;
		}
		break;
	case EShipAccelerationState::DECELERATING:
		if (CurrentForwardSpeed > 0)
		{
			CurrentForwardSpeed -= AccelerationForwardRate;
		}else
		if (CurrentForwardSpeed <= 0)
		{
			CurrentForwardSpeed = 0;
		}
		break;
	case EShipAccelerationState::CONSTANT_VELOCITY:
		break;
	case EShipAccelerationState::BRAKING:
		if (CurrentForwardSpeed > 0)
		{
			CurrentForwardSpeed -= 2 * AccelerationForwardRate;
		}else
		if (CurrentForwardSpeed <= 0)
		{
			CurrentForwardSpeed = 0;
		}
		break;
	default:
		checkNoEntry();
	}
}

void UComplexUnitMovementSystem::CalculateYawSpeed()
{
	switch (TurnState)
	{
	case EShipYawState::NO_TURNING:
		if (CurrentYawSpeed > 0)
		{
			CurrentYawSpeed -= AccelerationYawRate;
		}
		else
		{
			CurrentYawSpeed = 0;
		}
		break;
	case EShipYawState::TURNING_WHILE_MOVING:
		if (CurrentYawSpeed < MaxYawSpeed)
		{
			CurrentYawSpeed += AccelerationYawRate;
		}
		else
		{
			CurrentYawSpeed = MaxYawSpeed;
		}
		break;
	case EShipYawState::TURNING_WHILE_STANDING:
		if (CurrentYawSpeed < MaxYawSpeed)
		{
			CurrentYawSpeed += 0.5 * AccelerationYawRate;
		}
		else
		{
			CurrentYawSpeed = MaxYawSpeed;
		}
		break;
	default:
		checkNoEntry();
	}
}

void UComplexUnitMovementSystem::CalculateRoll()
{
	switch (RollState)
	{
	case EShipRollState1::NO_ROLLING:
		if (CurrentRollSpeed > 0)
		{
			CurrentRollSpeed -= AccelerationRollRate;
		}
		else
		{
			CurrentRollSpeed = 0;
		}
		break;
	case EShipRollState1::ROLL_TO_ZERO:
		break;
	case EShipRollState1::ROLLING:
		if (CurrentRollSpeed < MaxRollSpeed)
		{
			CurrentRollSpeed += AccelerationRollRate;
		}
		else
		{
			CurrentRollSpeed = MaxRollSpeed;
		}
		break;
	default:
		checkNoEntry();
	}
}