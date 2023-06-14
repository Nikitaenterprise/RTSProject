#include "Components/Movement/SimpleMovementComponent.h"

#include "AIController.h"
#include "Miscellaneous/AnglesFunctions.h"

USimpleMovementComponent::USimpleMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USimpleMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PawnOwner == nullptr ||	UpdatedComponent == nullptr ||	ShouldSkipUpdate(DeltaTime))
	{
		return;
	}
	
	if (PathFollowingComponent == nullptr || PathFollowingComponent->HasValidPath() == false)
	{
		return;
	}

	const uint32 NextPathIndex = PathFollowingComponent->GetNextPathIndex();
	const auto& Path = PathFollowingComponent->GetPath().Get();
	const auto& PathPoints = Path->GetPathPoints();
	
	FVector DestinationLocation;
	if (PathPoints.Num() != 0)
	{
		DestinationLocation = PathPoints[NextPathIndex];
	}
	else
	{
		DestinationLocation = PathFollowingComponent->GetPathDestination();
	}
	const FVector& CurrentLocation = GetOwner()->GetActorLocation();
	
	const float DistanceToTarget = FVector::Dist(CurrentLocation, DestinationLocation);
	
	DistanceToFinish = Path->GetLengthFromPosition(CurrentLocation, PathPoints.Num() - 1);
	
	CalculateSpeed(DistanceToTarget, DeltaTime);
	
	const float DeltaDistance = CurrentVelocity * DeltaTime;
	
	const FVector& Direction = (DestinationLocation - CurrentLocation).GetSafeNormal();
	const FVector& DeltaMove = Direction * DeltaDistance;
	const FRotator CurrentRotator = GetOwner()->GetActorRotation();
	const FRotator Rotation = AnglesFunctions::FindYawRotatorOn2D(GetOwner()->GetActorForwardVector(), Direction);
	
	AddInputVector(DeltaMove);
	FHitResult Hit;
	SafeMoveUpdatedComponent(ConsumeInputVector(), CurrentRotator + Rotation, true, Hit);
}

void USimpleMovementComponent::AddInputVector(FVector WorldVector, bool bForce)
{
	Super::AddInputVector(WorldVector, bForce);
	InputVector += WorldVector;
}

FVector USimpleMovementComponent::ConsumeInputVector()
{
	Super::ConsumeInputVector();
	const FVector ReturnVector = InputVector;
	InputVector = FVector::ZeroVector;
	return ReturnVector;
}

void USimpleMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	const auto* Pawn = Cast<APawn>(GetOwner());
	if (Pawn == nullptr)
	{
		return;
	}
	
	AIController = Cast<AAIController>(Pawn->GetController());
	if (AIController == nullptr)
	{
		return;
	}
	
	PathFollowingComponent = Cast<UPathFollowingComponent>(AIController->GetPathFollowingComponent());
}

void USimpleMovementComponent::StartMoving()
{
	switch(PathFollowingComponent->GetStatus())
	{
	case EPathFollowingStatus::Idle:
		MovementState = ESmallShipMovementState::Idle;
		break;
		
	case EPathFollowingStatus::Moving:
	{
		const FVector& CurrentLocation = GetOwner()->GetActorLocation();
		const FVector& DestinationLocation = PathFollowingComponent->GetPathDestination();
		if (FVector::Dist(CurrentLocation, DestinationLocation) > DecelerationDistance)
		{
			MovementState = ESmallShipMovementState::Accelerate;
		}
		else
		{
			MovementState = ESmallShipMovementState::Decelerate;
		}
		break;
	}
		
	case EPathFollowingStatus::Paused:
		MovementState = ESmallShipMovementState::Stopping;
		break;
		
	case EPathFollowingStatus::Waiting:
		MovementState = ESmallShipMovementState::Stopping;
		break;
		
	default:
		break;
	}
}

void USimpleMovementComponent::CalculateSpeed(const float DistanceToTarget, const float DeltaTime)
{
	switch (MovementState)
	{
	case ESmallShipMovementState::Idle:
	{
		StartMoving();
		break;
	}

	case ESmallShipMovementState::Moving:
	{
		if (DistanceToFinish < DecelerationDistance)
		{
			MovementState = ESmallShipMovementState::Decelerate;
		}
		break;
	}
		
	case ESmallShipMovementState::Accelerate:
	{
		if (DistanceToFinish < DecelerationDistance)
		{
			MovementState = ESmallShipMovementState::Decelerate;
			// a = V^2 / (2S)
			DecelerationRate = CurrentVelocity * CurrentVelocity / (2 * DecelerationDistance);
			break;
		}

		CurrentVelocity = FMath::Min(CurrentVelocity + AccelerationRate * DeltaTime, MaxVelocity);
		if (FMath::IsNearlyEqual(CurrentVelocity, MaxVelocity))
		{
			MovementState = ESmallShipMovementState::Moving;
		}
		break;
	}
        
	case ESmallShipMovementState::Decelerate:
	{
		CurrentVelocity = FMath::Max(CurrentVelocity - DecelerationRate * DeltaTime, 0.f);
		if (FMath::IsNearlyZero(CurrentVelocity))
		{
			MovementState = ESmallShipMovementState::Stopping;
		}
		break;
	}

	case ESmallShipMovementState::Stopping:
	{
		CurrentVelocity = 0;
		MovementState = ESmallShipMovementState::Idle;
		break;
	}
        
	default:
		check(false);
		break;
	}
}

