#include "ShipMovementComponent.h"

#include "AnglesFunctions.h"
#include "Ship.h"
#include "RTSAIController.h"

#include "AIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
//#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "RTSPlayerController.h"
#include "Components/CapsuleComponent.h"
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
	// Ship shouldn't move
	PointMoveTo = OwnerShip->GetActorLocation();
}

void UShipMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime)) return;

	
	FVector R = FVector::ZeroVector;
	FVector DirectionToDestination = FVector::ZeroVector;
	float DistanceToPoint = 0;
	
	if (1)
	{
		if (CurrentLine == nullptr || OwnerShip->GetActorLocation().Equals(CurrentLine->EndPosition, AcceptanceRadius))
		{
			if (LineSegments.Num() == 0)
			{
				bShouldMove = false;
				//GEngine->AddOnScreenDebugMessage(-1, 10.01, FColor::Red, TEXT("LineSegments in empty in ShipMovementComponent"));
				return;
			}
			CurrentLine = LineSegments.Pop(false);
		}
		
				
		FVector DeltaR = FVector::ZeroVector;
		switch(CurrentLine->LineType)
		{
		case ARC_LINE:
		{
			ArcLine* ArcSegment = static_cast<ArcLine*>(CurrentLine);
			// Determine current angle on arc (AngleOnCircle) by adding or
			// subtracting 90 degrees to the starting angle
			// depending on whether turning to the right or left
			const float AngleOnCircle = OwnerShip->GetActorRotation().Yaw + (ArcSegment->bClockwiseRotation ? -90 : 90);
			// Determine delta angle (DeltaYaw) which is added in this tick
			// da = omega*dt, omega = v/R, dr = v*dt, dr = 2R*sin(da/2) 
			const float DeltaYaw = UKismetMathLibrary::DegAsin(CurrentForwardSpeed / MinTurnRadius * DeltaTime / 2 ) * 2 * (ArcSegment->bClockwiseRotation ? 1 : -1);
			// Calculate dr = CircleCenter - r + position on circle 
			//DeltaR.X = ArcSegment->CircleCenter.X - OwnerShip->GetActorLocation().X + MinTurnRadius * UKismetMathLibrary::DegCos(AngleOnCircle + DeltaYaw);
			//DeltaR.Y = ArcSegment->CircleCenter.Y - OwnerShip->GetActorLocation().Y + MinTurnRadius * UKismetMathLibrary::DegSin(AngleOnCircle + DeltaYaw);

			// Determine current direction (Rotator.Yaw) by adding dA
			// to actor's rotation
			Rotator.Yaw = OwnerShip->GetActorRotation().Yaw + DeltaYaw;
				
			// Calculating banked turn
			// tan(DeltaRoll) = v^2 / r / g
			float DeltaRoll = UKismetMathLibrary::DegAtan(CurrentForwardSpeed * CurrentForwardSpeed * DeltaTime * DeltaTime / MinTurnRadius / 9.81 * (ArcSegment->bClockwiseRotation ? 1 : -1));
			// Divide by 2 for slower banked turn
			DeltaRoll /= 2;
			Rotator.Roll = OwnerShip->GetActorRotation().Roll + DeltaRoll;
			// Set roll value in between +- MaxRollAngle
			Rotator.Roll = Rotator.Roll > MaxRollAngle ? MaxRollAngle : (Rotator.Roll < -MaxRollAngle ? -MaxRollAngle : Rotator.Roll);
				

				
			// Determine acceleration state
			AccelerationState = ACCELERATING;
			TurnState = TURNING_WHILE_MOVING;
			RollState = ROLLING;
			
			DrawDebugCircle(GetWorld(), 
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
			break;
		}
		case STRAIGHT_LINE:
		{
			StraightLine* StraightSegment = static_cast<StraightLine*>(CurrentLine);
			const float YawToDestination = UKismetMathLibrary::RadiansToDegrees(
				AnglesFunctions::FindAngleBetweenVectorsOn2D(
					OwnerShip->GetActorForwardVector(),
					(StraightSegment->EndPosition - OwnerShip->GetActorLocation()).GetSafeNormal()));
			const bool bClockwiseRotation = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(
				OwnerShip->GetActorForwardVector(),
				(StraightSegment->EndPosition - OwnerShip->GetActorLocation()).GetSafeNormal());
				
			float DeltaYaw = YawToDestination * CurrentYawSpeed * DeltaTime;
			DeltaYaw = DeltaYaw > 0.5 ? 0.5 : DeltaYaw;
			DeltaYaw *= bClockwiseRotation ? 1 : -1;
				
			DistanceToPoint = (StraightSegment->EndPosition - OwnerShip->GetActorLocation()).Size();
				
			FString out = "";
			out += FString("\n YawToDestination= ") + FString::SanitizeFloat(YawToDestination);
			out += FString("\n ActorRotation= ") + FString::SanitizeFloat(OwnerShip->GetActorRotation().Yaw);
			out += FString("\n DeltaYaw= ") + FString::SanitizeFloat(DeltaYaw);
			out += FString("\n bClockwiseRotation= ") + (bClockwiseRotation ? TEXT("true") : TEXT("false"));
			GEngine->AddOnScreenDebugMessage(-1, 0.01, FColor::Green, out);
			
			// Angle between point and forward vector is >45
			if(YawToDestination >= 45)
			{
				// Situation when ship is standing
				if (CurrentForwardSpeed == 0)
				{
					TurnState = TURNING_WHILE_STANDING;
				}else
				// Situation when ship is accelerating 
				if (CurrentForwardSpeed >= 0.5 * MaxForwardSpeed)
				{
					AccelerationState = DECELERATING;
					TurnState = TURNING_WHILE_MOVING;
				}else
				if (CurrentForwardSpeed < 0.5 * MaxForwardSpeed && CurrentForwardSpeed >= 0.2 * MaxForwardSpeed)
				{
					AccelerationState = ACCELERATING;
					TurnState = TURNING_WHILE_MOVING;
				}else
				if (CurrentForwardSpeed < 0.2 * MaxForwardSpeed)
				{
					AccelerationState = CONSTANT_VELOCITY;
					TurnState = TURNING_WHILE_MOVING;
				}
			}else
			// Angle between point and forward vector is <45
			if (YawToDestination < 45 && YawToDestination > 1)
			{
				// If ship is turning while standing still
				if (CurrentForwardSpeed == 0 && TurnState == TURNING_WHILE_STANDING)
				{
					// Ship can start moving and turning at the same time
					AccelerationState = ACCELERATING;
				}
				else
				if (CurrentForwardSpeed < 0.5 * MaxForwardSpeed && CurrentForwardSpeed >= 0.2 * MaxForwardSpeed)
				{
					AccelerationState = ACCELERATING;
				}
				else
				if (CurrentForwardSpeed > 0.5 * MaxForwardSpeed)
				{
					AccelerationState = CONSTANT_VELOCITY;
				}
				TurnState = TURNING_WHILE_MOVING;
				RollState = ROLLING;
			}else
			if (YawToDestination <= 1)
			{
				if (CurrentForwardSpeed == 0)
				{
					AccelerationForwardRate = ACCELERATING;
				}else
				if (CurrentForwardSpeed < 0.5 * MaxForwardSpeed && CurrentForwardSpeed >= 0.2 * MaxForwardSpeed)
				{
					AccelerationState = ACCELERATING;
				}
				else
				if (CurrentForwardSpeed < 0.2 * MaxForwardSpeed)
				{
					AccelerationState = CONSTANT_VELOCITY;
				}
				
				TurnState = NO_TURNING;
				RollState = ROLL_TO_ZERO;
				DeltaYaw = 0;
			}
				
			
				
			Rotator.Yaw = OwnerShip->GetActorRotation().Yaw + DeltaYaw;

			if (DistanceToPoint < 15 * AcceptanceRadius)
			{
				AccelerationState = DECELERATING;
			}
			if (DistanceToPoint < 10 * AcceptanceRadius)
			{
				AccelerationState = BRAKING;
			}
			if (DistanceToPoint <= AcceptanceRadius)
			{
				AccelerationState = FULL_STOP;
				TurnState = NO_TURNING;
				RollState = ROLL_TO_ZERO;
			}
			
			break;
		}
		}
		
		if (RollState == ROLL_TO_ZERO)
		{
			// If ship has some roll angle then it should be counter-rolled
			const float CurrentRoll = OwnerShip->GetActorRotation().Roll;
			if (abs(CurrentRoll) > 0)
			{
				float DeltaRoll = CurrentRoll > 0 ? -1 : 1;
				DeltaRoll *= CurrentRollSpeed * DeltaTime;
				Rotator.Roll = OwnerShip->GetActorRotation().Roll + DeltaRoll;
			}
			else
			{
				Rotator.Roll = 0;
				RollState = NO_ROLLING;
			}
		}
		
		DeltaR = OwnerShip->GetActorForwardVector() * DeltaTime * CurrentForwardSpeed;
		DeltaR.Z = 0;
		AddInputVector(DeltaR);
		
		CalculateForwardSpeed();
		CalculateYawSpeed();
		CalculateRoll();
		
		R = ConsumeInputVector();
		
		FString out = "";
		out += FString("\nLineType: ") + FString(ELineSegmentStr[CurrentLine->LineType]);
		out += FString("\nAccelerationState: ") + FString(EShipAccelerationStateStr[AccelerationState]);
		out += FString("\nTurnState: ") + FString(EShipYawStateStr[TurnState]) + FString(" ");
		out += FString("\nRollState: ") + FString(EShipRollStateStr[RollState]) + FString(" ");
		GEngine->AddOnScreenDebugMessage(-1, 0.01, FColor::Green, out);
		 
		FHitResult Hit;
		SafeMoveUpdatedComponent(R, Rotator, true, Hit);
	}


}

void UShipMovementComponent::AddInputVector(FVector WorldVector, bool bForce)
{
	InputVector += WorldVector;
}

FVector UShipMovementComponent::ConsumeInputVector()
{
	FVector ReturnVector = InputVector;
	InputVector = FVector::ZeroVector;
	return ReturnVector;
}

bool UShipMovementComponent::RequestNavMoving(const FVector _TargetLocation)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

	TurnOnCapsuleCollision(true);
	NavSys->Build();
	TurnOnCapsuleCollision(false);

	float r, h;
	OwnerShip->GetSimpleCollisionCylinder(r, h);
	
	FString out = "";
	out += "Radius " + FString::SanitizeFloat(r);
	out += " Height " + FString::SanitizeFloat(h);


	FPathFindingQuery Query;
	Query.StartLocation = OwnerShip->GetActorLocation();
	Query.EndLocation = _TargetLocation;
	
	Query.NavData = NavSys->GetDefaultNavDataInstance();
	Query.SetNavAgentProperties(GetNavAgentPropertiesRef());


	UNavigationPath* NavPath = NewObject<UNavigationPath>(NavSys);
	NavPath->SetPath(NavSys->FindPathSync(GetNavAgentPropertiesRef(), Query).Path);
	
	/*out += "\nRadius " + FString::SanitizeFloat(GetNavAgentPropertiesRef().AgentRadius);
	out += " Height " + FString::SanitizeFloat(GetNavAgentPropertiesRef().AgentHeight);
	GEngine->AddOnScreenDebugMessage(-1, 3.1, FColor::Purple, out);*/
	
	if (!NavPath)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.1, FColor::Red, TEXT("Failed to create navigation"));
		return false;
	}
	
	NavPathCoords = NavPath->PathPoints;
	MakePathInXYPlane(OwnerShip->GetActorLocation().Z);
	LineSegments.Empty();
	BuildLineSegments();
	ReverceLineSegments();

	CurrentLine = nullptr;
	bShouldMove = true;


	return true;
}

void UShipMovementComponent::TurnOnCapsuleCollision(const bool TurnOn) const
{
	for (auto& Actor : PlayerController->PlayersActors)
	{
		AShip* Ship = Cast<AShip>(Actor);
		if (Ship && Ship != OwnerShip)
		{
			Ship->CapsuleComponent->SetCanEverAffectNavigation(TurnOn);
		}
	}
}

void UShipMovementComponent::ReverceLineSegments()
{
	TArray<LineSegment*> TemporaryLineSegments;
	TemporaryLineSegments.Reserve(LineSegments.Num());
	for (size_t i = LineSegments.Num(); i > 0; i--)
	{
		TemporaryLineSegments.Add(LineSegments[i - 1]);
	}
	LineSegments = TemporaryLineSegments;
}

void UShipMovementComponent::BuildLineSegments()
{
	if (NavPathCoords.Num() == 0) return;
	
	LineSegments.Reserve(NavPathCoords.Num());
	for (size_t i = 0; i < NavPathCoords.Num() - 1; i++)
	{
		FVector Start = NavPathCoords[i];
		FVector End = NavPathCoords[i + 1];

		FVector StartToEnd = End - Start;
		
		FVector ForwardVector = OwnerShip->GetActorForwardVector();

		const float ForwardAndDirToDestinationAngle = UKismetMathLibrary::RadiansToDegrees(
			AnglesFunctions::FindAngleBetweenVectorsOn2D(
				ForwardVector.GetSafeNormal(),
				StartToEnd.GetSafeNormal()));
		const bool bClockwiseRotation = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(
			ForwardVector.GetSafeNormal(),
			StartToEnd.GetSafeNormal());

		if (ForwardAndDirToDestinationAngle < 15)
		{
			LineSegments.Add(new StraightLine(Start, End, StartToEnd.Size(), bClockwiseRotation, ForwardAndDirToDestinationAngle));
		}
		else
		{
			
			/*DrawDebugLine(GetWorld(), FVector(OwnerShip->GetActorLocation().X, OwnerShip->GetActorLocation().Y, 150), FVector(ForwardVector.X * 20, ForwardVector.Y * 20, 150), FColor::Red, false, 50, 0, 5);
			DrawDebugLine(GetWorld(), FVector(Start.X, Start.Y, 150), FVector(End.X, End.Y, 150), FColor::White, false, 50, 0, 5);
			
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, bClockwiseRotation ? TEXT("bClockwiseRotation=true") : TEXT("bClockwiseRotation=false"));
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, FString::Printf(TEXT("ForwardVector= %s"), *ForwardVector.ToString()));
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, FString::Printf(TEXT("ForwardAndDirToDestinationAngle= %s"), *FString::SanitizeFloat(ForwardAndDirToDestinationAngle)));*/
			
			
			// First we calculate the location of point P, which is the center of our turning circle,
			// and is always radius r away from the starting point.
			// If we are turning right from our initial direction, that means P is at an angle
			// of (initial_direction - 90) from the origin, so
			const float AngleToCenterOfTurningCircle = ForwardVector.Rotation().Yaw + (bClockwiseRotation ? 90 : -90);
			FVector2D CircleCenter;
			CircleCenter.X = OwnerShip->GetActorLocation().X + MinTurnRadius * UKismetMathLibrary::DegCos(AngleToCenterOfTurningCircle);
			CircleCenter.Y = OwnerShip->GetActorLocation().Y + MinTurnRadius * UKismetMathLibrary::DegSin(AngleToCenterOfTurningCircle);
			
			// Now that we know the location of the center point P,
			// we can calculate the distance from P to the destination
			FVector2D h = FVector2D(End) - CircleCenter;
			const float DistanceFromCircleCenterToDestination = h.Size();
			// If End point is inside Circle
			if (DistanceFromCircleCenterToDestination < MinTurnRadius)
			{
				LineSegments.Add(new StraightLine(Start, End, StartToEnd.Size(), bClockwiseRotation, ForwardAndDirToDestinationAngle));
				continue;
			}

			// Now we can calculate the length of straight part, since we already know the lengths
			// of the other two sides of the right triangle, namely h and radius.
			// We can also determine angle theta from the right-triangle relationship:
			const float LengthOfStraightPart = sqrt(DistanceFromCircleCenterToDestination * DistanceFromCircleCenterToDestination - MinTurnRadius * MinTurnRadius);
			float Theta = UKismetMathLibrary::DegAcos(MinTurnRadius / DistanceFromCircleCenterToDestination); 
			// if rotating left then -theta, else +theta
			Theta = bClockwiseRotation ? -Theta : Theta;
			
			// Finally, to figure out the point Q at which to leave the circle and start
			// on the straight line, we need to know the total angle phi + theta,
			// and  is easily determined as the angle from P to the destination:
			const float Phi = UKismetMathLibrary::DegAtan2(h.Y, h.X);
			
			const FVector LeavingCirclePoint = FVector(
				CircleCenter.X + MinTurnRadius * UKismetMathLibrary::DegCos(Phi + Theta),
				CircleCenter.Y + MinTurnRadius * UKismetMathLibrary::DegSin(Phi + Theta),
				OwnerShip->GetActorLocation().Z
			);
			// Total angle to cover while moving on circle
			const float AngleOnCircle = 270 - Phi - Theta;
			
			LineSegments.Add(new ArcLine(Start, LeavingCirclePoint, AngleOnCircle * MinTurnRadius, bClockwiseRotation, CircleCenter, OwnerShip->GetActorRotation().Yaw, UKismetMathLibrary::DegreesToRadians(Phi + Theta)));
			LineSegments.Add(new StraightLine(LeavingCirclePoint, End, LengthOfStraightPart, bClockwiseRotation, ForwardAndDirToDestinationAngle));
			i++;
			
		}
	}
}

void UShipMovementComponent::MakePathInXYPlane(float _SetZToThisValue)
{
	for (auto& point : NavPathCoords)
	{
		point.Z = _SetZToThisValue;
	}
}

void UShipMovementComponent::CalculateForwardSpeed()
{
	switch (AccelerationState)
	{
	case FULL_STOP:
		CurrentForwardSpeed = 0;
		break;
	case ACCELERATING:
		if (CurrentForwardSpeed < MaxForwardSpeed)
		{
			CurrentForwardSpeed += AccelerationForwardRate;
		}
		else
		{
			CurrentForwardSpeed = MaxForwardSpeed;
		}
		break;
	case DECELERATING:
		if (CurrentForwardSpeed > 0)
		{
			CurrentForwardSpeed -= AccelerationForwardRate;
		}else
		if (CurrentForwardSpeed <= 0)
		{
			CurrentForwardSpeed = 0;
		}
		break;
	case CONSTANT_VELOCITY:
		break;
	case BRAKING:
		if (CurrentForwardSpeed > 0)
		{
			CurrentForwardSpeed -= 2 * AccelerationForwardRate;
		}else
		if (CurrentForwardSpeed <= 0)
		{
			CurrentForwardSpeed = 0;
		}
		break;
	}
}

void UShipMovementComponent::CalculateYawSpeed()
{
	switch (TurnState)
	{
	case NO_TURNING:
		if (CurrentYawSpeed > 0)
		{
			CurrentYawSpeed -= AccelerationYawRate;
		}
		else
		{
			CurrentYawSpeed = 0;
		}
		break;
	case TURNING_WHILE_MOVING:
		if (CurrentYawSpeed < MaxYawSpeed)
		{
			CurrentYawSpeed += AccelerationYawRate;
		}
		else
		{
			CurrentYawSpeed = MaxYawSpeed;
		}
		break;
	case TURNING_WHILE_STANDING:
		if (CurrentYawSpeed < MaxYawSpeed)
		{
			CurrentYawSpeed += 0.5 * AccelerationYawRate;
		}
		else
		{
			CurrentYawSpeed = MaxYawSpeed;
		}
		break;
	}
}

void UShipMovementComponent::CalculateRoll()
{
	switch (RollState)
	{
	case NO_ROLLING:
		if (CurrentRollSpeed > 0)
		{
			CurrentRollSpeed -= AccelerationRollRate;
		}
		else
		{
			CurrentRollSpeed = 0;
		}
		break;
	case ROLL_TO_ZERO:
		break;
	case ROLLING:
		if (CurrentRollSpeed < MaxRollSpeed)
		{
			CurrentRollSpeed += AccelerationRollRate;
		}
		else
		{
			CurrentRollSpeed = MaxRollSpeed;
		}
		break;
	}
}
