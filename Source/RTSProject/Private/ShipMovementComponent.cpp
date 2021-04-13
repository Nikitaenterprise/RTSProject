#include "ShipMovementComponent.h"

//#include "RTSPlayerController.h"
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
	// Ship shouldn't move
	PointMoveTo = OwnerShip->GetActorLocation();
}

void UShipMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime)) return;
	DeltaTime /= 10;
	
	FVector DesiredMovementThisFrame = FVector::ZeroVector;
	FVector DirectionToDestination = FVector::ZeroVector;
	float DistanceToPoint = 0;
	
	if (bShouldMove)
	{
		if (CurrentLine == nullptr || OwnerShip->GetActorLocation().Equals(CurrentLine->EndPosition, AcceptanceRadius))
		{
			if (LineSegments.Num() == 0)
			{
				bShouldMove = false;
				GEngine->AddOnScreenDebugMessage(-1, 10.01, FColor::Red, TEXT("LineSegments in empty in ShipMovementComponent"));
				return;
			}
			CurrentLine = LineSegments.Pop(false);
		}
		
		DirectionToDestination = CurrentLine->EndPosition - OwnerShip->GetActorLocation();
		DistanceToPoint = DirectionToDestination.Size();
		const FVector ForwardVector = OwnerShip->GetActorForwardVector();
		const float ForwardAndDirToDestinationAngle = UKismetMathLibrary::RadiansToDegrees(
			AnglesFunctions::FindAngleBetweenVectorsOn2D(
				DirectionToDestination.GetSafeNormal(),
				ForwardVector.GetSafeNormal()));
		const bool bClockwiseRotation = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(
			DirectionToDestination.GetSafeNormal(),
			ForwardVector.GetSafeNormal());

		float FrameDistance = CurrentForwardSpeed * DeltaTime;
		FVector Position;

		ArcLine* ArcSegment = nullptr;
		StraightLine* StraightSegment = nullptr;
		
		
		// If Segment is an arc
		if (CurrentLine->LineType == ARC_LINE)
		{
			ArcSegment = static_cast<ArcLine*>(CurrentLine);
			// Determine current angle on arc(theta) by adding or
			// subtracting(distance / r) to the starting angle
			// depending on whether turning to the left or right

			/*const float AngleOnArc = ArcSegment->StartingAngle + FrameDistance / MinTurnRadius * (ArcSegment->bCounterClockwiseRotation ? 1 : -1);
			Position.X = OwnerShip->GetActorLocation().X + ArcSegment->CircleCenter.X + MinTurnRadius * UKismetMathLibrary::DegCos(AngleOnArc);
			Position.Y = OwnerShip->GetActorLocation().Y - ArcSegment->CircleCenter.Y - MinTurnRadius * UKismetMathLibrary::DegSin(AngleOnArc);
			Position.Z = 0;*/
			const float CurrentAngle = UKismetMathLibrary::DegAtan2(-OwnerShip->GetActorLocation().Y, OwnerShip->GetActorLocation().X);
			const float AngleOnArc = CurrentAngle + FrameDistance / MinTurnRadius * (ArcSegment->bCounterClockwiseRotation ? 1 : -1);
			Position.X = MinTurnRadius * (UKismetMathLibrary::DegCos(AngleOnArc) - UKismetMathLibrary::DegCos(CurrentAngle));
			Position.Y = MinTurnRadius * (UKismetMathLibrary::DegSin(AngleOnArc) + UKismetMathLibrary::DegSin(CurrentAngle));
			Position.Z = 0;
			// Determine current direction(direction) by adding or
			// subtracting 90 to theta, depending on left / right
			//Rotator.Yaw = AngleOnArc + (ArcSegment->bCounterClockwiseRotation ? 90 : -90) * DeltaTime * CurrentYawSpeed;
			Rotator.Yaw = AngleOnArc;
			
			FString out = "";
			out += FString("\n FrameDistance= ") + FString::SanitizeFloat(FrameDistance);
			out += FString("\n AngleOnArc= ") + FString::SanitizeFloat(AngleOnArc);
			out += FString("\n Position= ") + Position.ToString();
			out += FString("\n Rotator.Yaw= ") + FString::SanitizeFloat(Rotator.Yaw);
			GEngine->AddOnScreenDebugMessage(-1, 0.01, FColor::Green, out);
		}
		// If Segment is a straight line
		else if (CurrentLine->LineType == STRAIGHT_LINE)
		{
			StraightSegment = static_cast<StraightLine*>(CurrentLine);
			Position.X = StraightSegment->StartPosition.X + MinTurnRadius * UKismetMathLibrary::DegCos(StraightSegment->Angle);
			Position.Y = StraightSegment->StartPosition.Y - MinTurnRadius * UKismetMathLibrary::DegSin(StraightSegment->Angle);
			Rotator.Yaw = OwnerShip->GetActorRotation().Yaw;// StraightSegment->Angle;
		}

		AddInputVector(Position);
		
		if (CurrentLine->LineType == ARC_LINE)
		{
			AccelerationState = Accelerating;
			TurnState = TurningWhileMoving;
		}
		if (CurrentLine->LineType == STRAIGHT_LINE)
		{
			if (DistanceToPoint >= 10 * AcceptanceRadius)
			{
				AccelerationState = Accelerating;
			}
			if (DistanceToPoint < 10 * AcceptanceRadius)
			{
				AccelerationState = Decelerating;
			}
			if (DistanceToPoint < 5 * AcceptanceRadius)
			{
				AccelerationState = Braking;
			}
			if (DistanceToPoint < AcceptanceRadius)
			{
				AccelerationState = FullStop;
			}
		}
		/*if (abs(ForwardAndDirToDestinationAngle) > 15)
		{
			RollState = Rolling;
		}
		else
		{
			RollState = RollToZero;
		}*/
		
		CalculateYawSpeed();
		//Rotator.Yaw = OwnerShip->GetActorRotation().Yaw + DeltaTime * (bClockwiseRotation ? -1 : 1) * ForwardAndDirToDestinationAngle * CurrentYawSpeed;

		CalculateRoll();
		//float deg = UKismetMathLibrary::DegAtan( CurrentForwardSpeed / MinTurnRadius * CurrentYawSpeed * (bClockwiseRotation ? -1 : 1));
		//deg = deg > MaxRollAngle ? MaxRollAngle : deg < -MaxRollAngle ? -MaxRollAngle : deg;
		//Rotator.Roll = OwnerShip->GetActorRotation().Roll + deg * DeltaTime * CurrentRollSpeed - CurrentRollSpeed * OwnerShip->GetActorRotation().Roll * DeltaTime;
		//// Set roll value in between +- MaxRollAngle
		//Rotator.Roll = Rotator.Roll > MaxRollAngle ? MaxRollAngle : (Rotator.Roll < -MaxRollAngle ? -MaxRollAngle : Rotator.Roll);

		CalculateForwardSpeed();
		DesiredMovementThisFrame = ConsumeInputVector() * DeltaTime * CurrentForwardSpeed;
		
		FString out = "";
		out += FString("\nLineType: ") + FString(ELineSegmentStr[CurrentLine->LineType]);
		out += FString("\nAccelerationState: ") + FString(EShipAccelerationStateStr[AccelerationState]) + FString(" ") + FString::SanitizeFloat(CurrentForwardSpeed);
		out += FString("\nTurnState: ") + FString(EShipYawStateStr[TurnState]) + FString(" ") + FString::SanitizeFloat(CurrentYawSpeed);
		out += FString("\nRollState: ") + FString(EShipRollStateStr[RollState]) + FString(" ") + FString::SanitizeFloat(OwnerShip->GetActorRotation().Roll);
		out += FString("\nDegAtan: ") + FString::SanitizeFloat(UKismetMathLibrary::DegAtan(CurrentForwardSpeed * CurrentYawSpeed * (bClockwiseRotation ? -1 : 1)));
		out += FString("\nForwardAndDirToDestinationAngle= ") + FString::SanitizeFloat(ForwardAndDirToDestinationAngle);
		//out += FString("\nClockwiseRotation ") + (bClockwiseRotation ? FString("false") : FString("true"));
		//out += FString("\nPointMoveTo " + PointMoveTo.ToString());
		//out += FString("\nActorLocation " + OwnerShip->GetActorLocation().ToString());
		//out += FString("\nInitialMove ") + (bInitialMove ? FString("true") : FString("false"));
		//out += FString("\nCenterOfTurningCircle " + CircleCenter.ToString());
		//out += FString("\nAngleToCenterOfTurningCircle " + FString::SanitizeFloat(AngleToCenterOfTurningCircle));
		//out += FString("\nDistanceFromCircleCenterToDestination " + FString::SanitizeFloat(DistanceFromCircleCenterToDestination));
		//out += FString("\nLengthOfStraightPart " + FString::SanitizeFloat(LengthOfStraightPart));
		//out += FString("\ntheta " + FString::SanitizeFloat(theta) + " phi " + FString::SanitizeFloat(phi) + " angle " + FString::SanitizeFloat(ForwardAndDirToDestinationAngle));
		//out += FString("\nLeavingCirclePoint " + LeavingCirclePoint.ToString());
		//GEngine->AddOnScreenDebugMessage(-1, 0.01, FColor::Green, out);
		 
	}

	FHitResult Hit;
	SafeMoveUpdatedComponent(DesiredMovementThisFrame, Rotator, true, Hit);

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

bool UShipMovementComponent::RequestNavMoving(const FVector _TargetLocation)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

	for(auto& Actor : PlayerController->PlayersActors)
	{
		AShip* Ship = Cast<AShip>(Actor);
		if (Ship && Ship != OwnerShip)
		{
			Ship->CapsuleComponent->SetCanEverAffectNavigation(true);
		}
	}
	
	NavSys->Build();

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
	//ReverceNavPath();
	MakePathInXYPlane(OwnerShip->GetActorLocation().Z);
	BuildLineSegments();

	for (auto& Actor : PlayerController->PlayersActors)
	{
		AShip* Ship = Cast<AShip>(Actor);
		if (Ship && Ship != OwnerShip)
		{
			Ship->CapsuleComponent->SetCanEverAffectNavigation(false);
		}
	}
	
	bRequestedMove = true;
	bInitialMove = true;
	return true;
}

void UShipMovementComponent::ReverceNavPath()
{
	if(NavPathCoords.Num() == 0) return;
	TArray<FVector> tmp;
	tmp.Reserve(NavPathCoords.Num());
	for(size_t i = NavPathCoords.Num();i>0;i--)
	{
		tmp.Add(NavPathCoords[i-1]);
	}
	NavPathCoords = tmp;
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
				StartToEnd.GetSafeNormal(),
				ForwardVector.GetSafeNormal()));

		if (ForwardAndDirToDestinationAngle < 1)
		{
			LineSegments.Add(new StraightLine(Start, End, StartToEnd.Size(), ForwardAndDirToDestinationAngle));
		}
		else
		{
			const bool bCounterClockwiseRotation = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(
				StartToEnd.GetSafeNormal(),
				ForwardVector.GetSafeNormal());

			// First we calculate the location of point P, which is the center of our turning circle,
			// and is always radius r away from the starting point.
			// If we are turning right from our initial direction, that means P is at an angle
			// of (initial_direction - 90) from the origin, so
			const float AngleToCenterOfTurningCircle = ForwardVector.GetSafeNormal().Rotation().Yaw + (bCounterClockwiseRotation ? 90 : -90);
			FVector2D CircleCenter;
			CircleCenter.X = OwnerShip->GetActorLocation().X + MinTurnRadius * UKismetMathLibrary::DegCos(AngleToCenterOfTurningCircle);
			CircleCenter.Y = OwnerShip->GetActorLocation().Y - MinTurnRadius * UKismetMathLibrary::DegSin(AngleToCenterOfTurningCircle);

			// Now that we know the location of the center point P,
			// we can calculate the distance from P to the destination
			FVector2D h = FVector2D(End) - CircleCenter;
			const float DistanceFromCircleCenterToDestination = h.Size();

			// Now we can calculate the length of straight part, since we already know the lengths
			// of the other two sides of the right triangle, namely h and radius.
			// We can also determine angle theta from the right-triangle relationship:
			const float LengthOfStraightPart = sqrt(DistanceFromCircleCenterToDestination * DistanceFromCircleCenterToDestination - MinTurnRadius * MinTurnRadius);
			float Theta = UKismetMathLibrary::DegAcos(MinTurnRadius / DistanceFromCircleCenterToDestination); 
			// if rotating left then -theta, else +theta
			Theta = bCounterClockwiseRotation ? -Theta : Theta;
			
			// Finally, to figure out the point Q at which to leave the circle and start
			// on the straight line, we need to know the total angle phi + theta,
			// and  is easily determined as the angle from P to the destination:
			const float Phi = UKismetMathLibrary::DegAtan2(-h.Y, h.X);
			
			const FVector LeavingCirclePoint = FVector(
				CircleCenter.X + MinTurnRadius * UKismetMathLibrary::DegCos(Phi + Theta),
				CircleCenter.Y - MinTurnRadius * UKismetMathLibrary::DegSin(Phi + Theta),
				OwnerShip->GetActorLocation().Z
			);
			
			const float AngleOnCircle = 270 - Phi - Theta;
			
			DrawDebugCircle(GetWorld(), FVector(CircleCenter.X, CircleCenter.Y, 150), MinTurnRadius, 36, FColor::Purple, false, 50, 0, 5, FVector(0,1,0), FVector(1,0,0));
			DrawDebugLine(GetWorld(), FVector(LeavingCirclePoint.X, LeavingCirclePoint.Y, 150), FVector(End.X, End.Y, 150), FColor::Purple, false, 50, 0, 5);
			DrawDebugPoint(GetWorld(), FVector(Start.X, Start.Y, 150), 5, FColor::Red, false, 50);
			
			LineSegments.Add(new ArcLine(Start, LeavingCirclePoint, AngleOnCircle * MinTurnRadius, CircleCenter, OwnerShip->GetActorRotation().Yaw, UKismetMathLibrary::DegreesToRadians(Phi + Theta), bCounterClockwiseRotation));
			LineSegments.Add(new StraightLine(LeavingCirclePoint, End, LengthOfStraightPart, AngleOnCircle));
			i++;
			
		}
	}
	// Reverce line segments
	TArray<LineSegment*> tmp;
	tmp.Reserve(LineSegments.Num());
	for (size_t i = LineSegments.Num(); i > 0; i--)
	{
		tmp.Add(LineSegments[i - 1]);
	}
	LineSegments = tmp;
	bShouldMove = true;
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
			//GEngine->AddOnScreenDebugMessage(-1, 5.1, FColor::Yellow, FString::Printf(TEXT("Move started from %s to %s"), *from, *to));
			
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

void UShipMovementComponent::CalculateForwardSpeed()
{
	switch (AccelerationState)
	{
	case FullStop:
		CurrentForwardSpeed = 0;
		break;
	case Accelerating:
		if (CurrentForwardSpeed < MaxForwardSpeed)
		{
			CurrentForwardSpeed += AccelerationForwardRate;
		}
		else
		{
			CurrentForwardSpeed = MaxForwardSpeed;
		}
		break;
	case Decelerating:
		if (CurrentForwardSpeed > 0)
		{
			CurrentForwardSpeed -= AccelerationForwardRate;
		}
		else
		{
			CurrentForwardSpeed = 0;
		}
		break;
	case Braking:
		if (CurrentForwardSpeed > 0)
		{
			CurrentForwardSpeed -= 5 * AccelerationForwardRate;
		}
		else
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
	case NoTurning:
		if (CurrentYawSpeed > 0)
		{
			CurrentYawSpeed -= AccelerationYawRate;
		}
		else
		{
			CurrentYawSpeed = 0;
		}
		break;
	case TurningWhileMoving:
		if (CurrentYawSpeed < MaxYawSpeed)
		{
			CurrentYawSpeed += AccelerationYawRate;
		}
		else
		{
			CurrentYawSpeed = MaxYawSpeed;
		}
		break;
	case TurningWhileStanding:
		if (CurrentYawSpeed < MaxYawSpeed)
		{
			CurrentYawSpeed += AccelerationYawRate;
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
	case NoRolling:
		if (CurrentRollSpeed > 0)
		{
			CurrentRollSpeed -= AccelerationRollRate;
		}
		else
		{
			CurrentRollSpeed = 0;
		}
		break;
	case RollToZero:
		break;
	case Rolling:
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
