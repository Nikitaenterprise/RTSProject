// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Movement/ShipPathFollowingComponent.h"

#include "DrawDebugHelpers.h"
#include "AI/ShipAIController.h"
#include "Components/Movement/ShipMovementComponent.h"

void UShipPathFollowingComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	SplinePath = NewObject<USplineComponent>(GetOwner(), USplineComponent::StaticClass(), NAME_None, RF_Transient);
	SplinePath->RegisterComponent();
	SplinePath->CreationMethod = EComponentCreationMethod::Instance;
	SplinePath->SetMobility(EComponentMobility::Movable);
}

FAIRequestID UShipPathFollowingComponent::RequestMove(const FAIMoveRequest& RequestData, FNavPathSharedPtr InPath)
{
	const auto& Result =  Super::RequestMove(RequestData, InPath);

	CurrentDistanceAlongSpline = 0;
	GenerateSpline();
	
	return Result;
}

bool UShipPathFollowingComponent::UpdateMovementComponent(bool bForce)
{
	const bool Result = Super::UpdateMovementComponent(bForce);
	ShipMovementComponent = Cast<UShipMovementComponent>(MovementComp);
	return Result;
}

void UShipPathFollowingComponent::SetMoveSegment(int32 SegmentStartIndex)
{
	Super::SetMoveSegment(SegmentStartIndex);

	InitialDistanceToDestination = (ShipMovementComponent->GetActorLocation() - GetCurrentTargetLocation()).Size();
}

void UShipPathFollowingComponent::FollowPathSegment(float DeltaTime)
{
	/// Performs the actual movement by working in tandem with the AI’s movement component
	
	DrawDebugSpline(true);
	
	const FVector& Location = GetOwner()->GetActorLocation();
	const FVector& Destination = GetCurrentTargetLocation();
	const FVector& DirectionToDestination = Destination - Location;
	const float DistanceToDestination = DirectionToDestination.Size();
	
	if (Path && SplinePath && SplinePoints.Num() > 0)
	{
		const float MoveDistance = ShipMovementComponent->GetLastInputVector().Size();
		CurrentDistanceAlongSpline += MoveDistance;
		
		// Move towards the next smoothed path point
		const FVector& CurrentLocation = GetOwner()->GetActorLocation();
		const FVector& NextLocation = SplinePath->GetLocationAtDistanceAlongSpline(CurrentDistanceAlongSpline, ESplineCoordinateSpace::World);
		const FRotator& NextDirection = SplinePath->GetRotationAtDistanceAlongSpline(CurrentDistanceAlongSpline, ESplineCoordinateSpace::World);

		// Set the new location and rotation of the pawn
		//Cast<AShipAIController>(GetOwner())->GetPawn()->SetActorLocationAndRotation(NextLocation, NextDirection);

		// Remove the smoothed path point if we have reached it
		if (CurrentDistanceAlongSpline >= SplinePath->GetSplineLength())
		{
			OnPathFinished(FPathFollowingResult(EPathFollowingResult::Type::Success));
		}
	}
	else
	{
		// Call the base class implementation if we have no smoothed path points left
		Super::FollowPathSegment(DeltaTime);
	}
}

void UShipPathFollowingComponent::UpdatePathSegment()
{
	Super::UpdatePathSegment();

	/// Checks whether or not we have reached the end of a segment
	/// or if we reached our goal. Reaching the (intermediate) target
	/// point triggers an update of the move segment. If the AI has arrived
	/// at its goal destination, the movement status will be updated and the success dispatched.

	if (Path == nullptr || ShipMovementComponent == nullptr)
	{
		return;
	}
	
	const float DistanceLeft = (
		*Path->GetPathPointLocation(Path->GetPathPoints().Num() - 1)
		- ShipMovementComponent->GetActorLocation()
	).Size();

	if (DistanceLeft < AcceptanceRadius)
	{
		OnSegmentFinished();
		OnPathFinished(FPathFollowingResult(EPathFollowingResult::Type::Success));
	}
}

void UShipPathFollowingComponent::OnPathUpdated()
{
	Super::OnPathUpdated();
}

void UShipPathFollowingComponent::OnPathfindingQuery(FPathFindingQuery& Query)
{
	Super::OnPathfindingQuery(Query);
}

FVector UShipPathFollowingComponent::GetNextDestinationPoint(const float DeltaDistance) const
{
	if (SplinePath)
	{
		const float NextDestinationAlongSpline = FMath::Floor(CurrentDistanceAlongSpline + DeltaDistance);
		return SplinePath->GetLocationAtDistanceAlongSpline(NextDestinationAlongSpline, ESplineCoordinateSpace::World);
	}
	return FVector::ZeroVector;
}

FRotator UShipPathFollowingComponent::GetNextRotationPoint(const float DeltaDistance) const
{
	if (SplinePath)
	{
		const float NextDestinationAlongSpline = FMath::Floor(CurrentDistanceAlongSpline + DeltaDistance);
		return SplinePath->GetRotationAtDistanceAlongSpline(NextDestinationAlongSpline, ESplineCoordinateSpace::World);
	}
	return FRotator::ZeroRotator;
}

void UShipPathFollowingComponent::GenerateSpline()
{
	SplinePath->ClearSplinePoints(false);
	SplinePoints.Empty();
	PathPoints = Path->GetPathPoints();
	if (PathPoints.Num() == 0)
	{
		return;
	}

	const FVector& CurrentDirection = GetOwner()->GetActorForwardVector();
	const FVector& CurrentLocation = GetOwner()->GetActorLocation();

	for (auto& Point : PathPoints)
	{
		Point.Location.Z = CurrentLocation.Z;
	}

	const float TangentModifier = ShipMovementComponent->GetMaxTurnRadius();
	SplinePoints.Emplace(0, PathPoints[0].Location, TangentModifier  * CurrentDirection, TangentModifier  * CurrentDirection);
	SplinePoints[0].Type = ESplinePointType::CurveCustomTangent;
	
	FVector NewDirection = CurrentDirection;
	for (int32 i = 1; i < PathPoints.Num(); i++)
	{
		// Calculate the direction from the previous point to the current point
		FVector Direction = PathPoints[i].Location - PathPoints[i-1].Location;
		Direction.Normalize();

		// Calculate the tangent for the current point based on the previous direction and the current direction
		FVector Tangent = (Direction + (Direction - NewDirection)) * TangentModifier;

		// Add the spline point with the calculated tangent
		SplinePoints.Emplace(i, PathPoints[i].Location, Tangent, Tangent);
		SplinePoints[i].Type = ESplinePointType::CurveCustomTangent;

		// Update the current direction for the next iteration
		NewDirection = Direction;
	}
	
	SplinePath->SetClosedLoop(false);
	SplinePath->AddPoints(SplinePoints, true);
}

void UShipPathFollowingComponent::DrawDebugSpline(const bool bDraw)
{
	if (Path && bDraw && SplinePath)
	{
		Path->DebugDraw(MyNavData, FColor::White, nullptr, false, 10, 0);
		// Draw the start point of the current path segment we are traveling.
		FNavPathPoint CurrentPathPoint{};
		FNavigationPath::GetPathPoint(&Path->AsShared().Get(), GetCurrentPathIndex(), CurrentPathPoint);
		//DrawDebugLine(GetWorld(), CurrentPathPoint.Location, CurrentPathPoint.Location + FVector(0.f, 0.f, 200.f), FColor::Blue);
		DrawDebugSphere(GetWorld(), CurrentPathPoint.Location + FVector(0.f, 0.f, 200.f), 25.f, 16, FColor::Blue);

		// Draw the end point of the current path segment we are traveling.
		FNavPathPoint NextPathPoint{};
		FNavigationPath::GetPathPoint(&Path->AsShared().Get(), GetNextPathIndex(), NextPathPoint);
		//DrawDebugLine(GetWorld(), NextPathPoint.Location, NextPathPoint.Location + FVector(0.f, 0.f, 200.f), FColor::Green);
		DrawDebugSphere(GetWorld(), NextPathPoint.Location + FVector(0.f, 0.f, 200.f), 25.f, 16, FColor::Green);

		DrawDebugSphere(GetWorld(), Path->GetGoalLocation() + FVector(0.f, 0.f, 200.f), 25.f, 16, FColor::Red);

		SplinePath->SetDrawDebug(true);
	}
}
