#include "ShipMovementComponent.h"

//#include "RTSPlayerController.h"
#include "AnglesFunctions.h"
#include "Ship.h"
#include "RTSAIController.h"



#include "AIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
//#include "Components/CapsuleComponent.h"
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
	this->DeltaTime = DeltaTime;
	
	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime)) return;

	
	FVector DesiredMovementThisFrame = FVector::ZeroVector;
	FVector DirectionToDestination = FVector::ZeroVector;
	float DistanceToPoint = 0;
	
	if (bShouldMove)
	{
		DirectionToDestination = PointMoveTo - OwnerShip->GetActorLocation();
		DistanceToPoint = DirectionToDestination.Size();

		
		
		FVector ForwardVector = OwnerShip->GetActorForwardVector();

		
		const float ForwardAndDirToDestinationAngle = UKismetMathLibrary::RadiansToDegrees(
			AnglesFunctions::FindAngleBetweenVectorsOn2D(
				DirectionToDestination.GetSafeNormal(), 
				ForwardVector.GetSafeNormal()));

		const bool bClockwiseRotation = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(
			DirectionToDestination.GetSafeNormal(), 
			ForwardVector.GetSafeNormal());

		float AngleToCenterOfTurningCircle = OwnerShip->GetActorForwardVector().GetSafeNormal().Rotation().Yaw + (bClockwiseRotation ? -90 : 90);
		
		FVector2D CircleCenter = FVector2D::ZeroVector;
		CircleCenter.X = OwnerShip->GetActorLocation().X + MinTurnRadius * cos(AngleToCenterOfTurningCircle);
		CircleCenter.Y = OwnerShip->GetActorLocation().Y + MinTurnRadius * sin(AngleToCenterOfTurningCircle);

		float dx = PointMoveTo.X - CircleCenter.X;
		float dy = PointMoveTo.Y - CircleCenter.Y;
		FVector2D h = FVector2D(PointMoveTo) - CircleCenter;
		float DistanceFromCircleCenterToDestination = h.Size();//sqrt(dx * dx + dy * dy);

		float LengthOfStraightPart = sqrt(DistanceFromCircleCenterToDestination * DistanceFromCircleCenterToDestination - MinTurnRadius * MinTurnRadius);

		float theta = UKismetMathLibrary::DegAcos(MinTurnRadius / DistanceFromCircleCenterToDestination);
		float phi = UKismetMathLibrary::DegAtan(h.Y / h.X);

		FVector2D LeavingCirclePoint = FVector2D(
			// if rotating left then phi + theta, else phi - theta
			CircleCenter.X - MinTurnRadius * cos(phi + (bClockwiseRotation ? -theta : theta)),
			CircleCenter.Y - MinTurnRadius * sin(phi + (bClockwiseRotation ? -theta : theta))
		);
		
		AddInputVector(ForwardVector);

		bInitialMove = false;
		// If destination is within the turning circle and ship is not moving 
		if (AccelerationState == FullStop && DistanceFromCircleCenterToDestination < MinTurnRadius)
		{
			// If not looking at destination rotate while standing still
			if(ForwardAndDirToDestinationAngle > 5)
			{
				AccelerationState = FullStop;
				TurnState = TurningWhileStanding;
				RollState = Rolling;
			}
			// If looking at destination then move straight
			else
			{
				AccelerationState = Accelerating;
				TurnState = NoTurning;
				RollState = RollToZero;
			}
		}
		// If destination is not within the turning circle then move with rotation
		else
		{
			if (ForwardAndDirToDestinationAngle < 1)
			{
				TurnState = NoTurning;
			}
			else
			{
				TurnState = TurningWhileMoving;
			}
		}
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
		if (abs(ForwardAndDirToDestinationAngle) > 15)
		{
			RollState = Rolling;
		}
		else
		{
			RollState = RollToZero;
		}
		
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
		Rotator.Yaw = OwnerShip->GetActorRotation().Yaw + DeltaTime * (bClockwiseRotation ? -1 : 1) * ForwardAndDirToDestinationAngle * CurrentYawSpeed;

		bool bCounterRoll = false;
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
		float deg = UKismetMathLibrary::DegAtan( CurrentForwardSpeed / MinTurnRadius * CurrentYawSpeed * (bClockwiseRotation ? -1 : 1));
		deg = deg > MaxRollAngle ? MaxRollAngle : deg < -MaxRollAngle ? -MaxRollAngle : deg;
		Rotator.Roll = OwnerShip->GetActorRotation().Roll + deg * DeltaTime * CurrentRollSpeed - CurrentRollSpeed * OwnerShip->GetActorRotation().Roll * DeltaTime;
		// Set roll value in between +- MaxRollAngle
		Rotator.Roll = Rotator.Roll > MaxRollAngle ? MaxRollAngle : (Rotator.Roll < -MaxRollAngle ? -MaxRollAngle : Rotator.Roll);

		
		switch(AccelerationState)
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
		DesiredMovementThisFrame = ConsumeInputVector() * DeltaTime * CurrentForwardSpeed;
		
		FString out = "";
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
		GEngine->AddOnScreenDebugMessage(-1, 0.01, FColor::Green, out);
		 
	}

	/*if (bUseRVOAvoidance)
	{
		CalculateAvoidanceVelocity(DeltaTime);
		UpdateAvoidance(DeltaTime);
	}*/
	
	//if(DistanceToPoint >= AcceptanceRadius)
	if (!DesiredMovementThisFrame.IsNearlyZero())
	//if (AccelerationState != FullStop && TurnState != NoTurning && RollState != NoRolling)
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(DesiredMovementThisFrame, Rotator, true, Hit);

		// If we bumped into something, try to slide along it
		/*if (Hit.IsValidBlockingHit())
		{
			SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
		}*/
	}
	else
	{
		GetPoint();
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

bool UShipMovementComponent::RequestNavMoving(const FVector _TargetLocation)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

	
	
	for(auto& Actor : PlayerController->PlayersActors)
	{
		AShip* Ship = Cast<AShip>(Actor);
		if (Ship && Ship != OwnerShip)
		{
			Ship->CapsuleComponent->SetCanEverAffectNavigation(true);
			//GEngine->AddOnScreenDebugMessage(-1, 2.1, FColor::Green, FString::Printf(TEXT("Capsule collision %hs"), Ship->CapsuleComponent->CanEverAffectNavigation() ? "on" : "off"));
		}
	}
	
	NavSys->Build();
	/*FNavAgentProperties& InitProp = GetNavAgentPropertiesRef();
	InitProp.AgentHeight = OwnerShip->CapsuleComponent->GetScaledCapsuleRadius() * 3000;
	InitProp.AgentRadius = OwnerShip->CapsuleComponent->GetScaledCapsuleHalfHeight() * 3000;*/
	
	/*FNavAgentProperties Prop;
	Prop.AgentHeight = OwnerShip->CapsuleComponent->GetScaledCapsuleRadius()*3000;
	Prop.AgentRadius = OwnerShip->CapsuleComponent->GetScaledCapsuleHalfHeight()*3000;*/
	//UpdateNavAgent(*OwnerShip->CapsuleComponent);
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

	out += "\nRadius " + FString::SanitizeFloat(GetNavAgentPropertiesRef().AgentRadius);
	out += " Height " + FString::SanitizeFloat(GetNavAgentPropertiesRef().AgentHeight);

	UNavigationPath* NavPath = NewObject<UNavigationPath>(NavSys);
	NavPath->SetPath(NavSys->FindPathSync(GetNavAgentPropertiesRef(), Query).Path);
	
	//GEngine->AddOnScreenDebugMessage(-1, 3.1, FColor::Purple, out);
	
	
	/*UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(
										GetWorld(),
										OwnerShip->GetActorLocation(),
										_TargetLocation);*/
	if (!NavPath)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.1, FColor::Red, TEXT("Failed to create navigation"));
		return false;
	}
	
	NavPathCoords = NavPath->PathPoints;
	ReverceNavPath();
	MakePathInXYPlane(OwnerShip->GetActorLocation().Z);

	for (auto& Actor : PlayerController->PlayersActors)
	{
		AShip* Ship = Cast<AShip>(Actor);
		if (Ship && Ship != OwnerShip)
		{
			Ship->CapsuleComponent->SetCanEverAffectNavigation(false);
			//GEngine->AddOnScreenDebugMessage(-1, 2.1, FColor::Green, FString::Printf(TEXT("Capsule collision is %hs"), Ship->CapsuleComponent->CanEverAffectNavigation() ? "on" : "off"));
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
	for (size_t i = 0; i < NavPathCoords.Num(); i++)
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
			LineSegments.Add(new StraightLine(Start, StartToEnd.Size(), ForwardAndDirToDestinationAngle));
		}
		else
		{
			bool bClockwiseRotation = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(
				StartToEnd.GetSafeNormal(),
				ForwardVector.GetSafeNormal());

			// First we calculate the location of point P, which is the center of our turning circle,
			// and is always radius r away from the starting point.
			// If we are turning right from our initial direction, that means P is at an angle
			// of (initial_direction - 90) from the origin, so
			float AngleToCenterOfTurningCircle = ForwardVector.GetSafeNormal().Rotation().Yaw + (bClockwiseRotation ? -90 : 90);
			FVector2D CircleCenter = FVector2D::ZeroVector;
			CircleCenter.X = OwnerShip->GetActorLocation().X + MinTurnRadius * cos(AngleToCenterOfTurningCircle);
			CircleCenter.Y = OwnerShip->GetActorLocation().Y + MinTurnRadius * sin(AngleToCenterOfTurningCircle);

			// Now that we know the location of the center point P,
			// we can calculate the distance from P to the destination
			FVector2D h = FVector2D(PointMoveTo) - CircleCenter;
			float DistanceFromCircleCenterToDestination = h.Size();

			// Now we can calculate the length of segment d, since we already know the lengths
			// of the other two sides of the right triangle, namely h and r.
			// We can also determine angle theta from the right-triangle relationship:
			float LengthOfStraightPart = sqrt(DistanceFromCircleCenterToDestination * DistanceFromCircleCenterToDestination - MinTurnRadius * MinTurnRadius);
			float theta = UKismetMathLibrary::DegAcos(MinTurnRadius / DistanceFromCircleCenterToDestination);

			// Finally, to figure out the point Q at which to leave the circle and start
			// on the straight line, we need to know the total angle phi + theta,
			// and  is easily determined as the angle from P to the destination:
			float phi = UKismetMathLibrary::DegAtan(h.Y / h.X);
			FVector2D LeavingCirclePoint = FVector2D(
				// if rotating left then phi + theta, else phi - theta
				CircleCenter.X - MinTurnRadius * cos(phi + (bClockwiseRotation ? -theta : theta)),
				CircleCenter.Y - MinTurnRadius * sin(phi + (bClockwiseRotation ? -theta : theta))
			);
			
			LineSegments.Add(new ArcLine(Start, 360-(90+ phi + (bClockwiseRotation ? -theta : theta))*MinTurnRadius, CircleCenter, ForwardAndDirToDestinationAngle, UKismetMathLibrary::DegreesToRadians(phi + theta), bClockwiseRotation));
		}
	}
}

void UShipMovementComponent::FollowPath()
{
	float FrameDistance = CurrentForwardSpeed * DeltaTime;
	FVector Position;
	float Direction;
	float theta;
	for (const auto& Segment : LineSegments)
	{
		// If unit is somewhere on this line segment
		if (FrameDistance < Segment->Length)
		{
			ArcLine* ArcSegment = Cast<ArcLine>(Segment);
			StraightLine* StraightSegment = Cast<StraightLine>(Segment);
			// If Segment is an arc
			if(ArcSegment)
			{
				// Determine current angle on arc(theta) by adding or
				// subtracting(distance / r) to the starting angle
				// depending on whether turning to the left or right
				theta = OwnerShip->GetActorRotation().Yaw +
					(ArcSegment->bClockwiseRotation ? FrameDistance / MinTurnRadius : -FrameDistance / MinTurnRadius);
				
				Position.X = ArcSegment->CircleCenter.X + MinTurnRadius * cos(theta);
				Position.Y = ArcSegment->CircleCenter.Y + MinTurnRadius * sin(theta);

				// Determine current direction(direction) by adding or
				// subtracting 90 to theta, depending on left / right
				Direction = theta + ArcSegment->bClockwiseRotation ? 90 : -90;
			}else
			// If Segment is a straight line
			if(StraightSegment)
			{
				Position.X = StraightSegment->StartPosition.X + FrameDistance + MinTurnRadius * cos(StraightSegment->Angle);
				Position.Y = StraightSegment->StartPosition.Y + FrameDistance + MinTurnRadius * sin(StraightSegment->Angle);
				Direction = theta;
				break;
			}
		}
		else
		{
			FrameDistance -= Segment->Length;
		}
	}
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