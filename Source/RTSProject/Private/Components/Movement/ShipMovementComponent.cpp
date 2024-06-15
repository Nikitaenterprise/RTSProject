#include "Components/Movement/ShipMovementComponent.h"

#include "AIController.h"
#include "Components/Movement/ShipPathFollowingComponent.h"
#include "Interfaces/UnitMovementSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Miscellaneous/AnglesFunctions.h"
#include "Systems/UnitMovement/ComplexUnitMovementSystem.h"

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
	bWantsInitializeComponent = true;
}

void UShipMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	const auto* ShipPawn = Cast<APawn>(GetOwner());
	if (ShipPawn == nullptr)
	{
		return;
	}
	
	const auto* AIController = Cast<AAIController>(ShipPawn->GetController());
	if (AIController == nullptr)
	{
		return;
	}
	
	PathFollowingComponent = Cast<UShipPathFollowingComponent>(AIController->GetPathFollowingComponent());
}

void UShipMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (PawnOwner == nullptr ||	UpdatedComponent == nullptr ||	ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	if (FMath::IsNearlyZero(GetOwner()->GetActorRotation().Roll) == false)
	{
		RollState = EShipRollState::Roll_To_Zero;
		FRotator RotatorToSet = GetOwner()->GetActorRotation();
		CalculateRoll(RotatorToSet, GetOwner()->GetActorLocation(), DeltaTime);
		FHitResult Hit;
		SafeMoveUpdatedComponent(ConsumeInputVector(), RotatorToSet, true, Hit);
	}
	
	if (PathFollowingComponent == nullptr || PathFollowingComponent->HasValidPath() == false)
	{
		return;
	}

	const FVector& CurrentLocation = GetOwner()->GetActorLocation();
	const FVector& DestinationLocation = PathFollowingComponent->GetPathDestination();
	const float DistanceToTarget = FVector::Dist(CurrentLocation, DestinationLocation);
	
	CalculateSpeed(DistanceToTarget, DeltaTime);

	const float DeltaDistance = CurrentSpeed * DeltaTime;
	const FVector& NextLocation = PathFollowingComponent->GetNextDestinationPoint(DeltaDistance);
	FRotator NextRotation = PathFollowingComponent->GetNextRotationPoint(DeltaDistance);

	CalculateRoll(NextRotation, NextLocation, DeltaTime);
	
	const FVector& Direction = (NextLocation - CurrentLocation).GetSafeNormal();
	const FVector& DeltaMove = Direction * DeltaDistance;
	
	AddInputVector(DeltaMove);
	FHitResult Hit;
	SafeMoveUpdatedComponent(ConsumeInputVector(), NextRotation, true, Hit);
}

void UShipMovementComponent::AddInputVector(FVector WorldVector, bool bForce)
{
	Super::AddInputVector(WorldVector, bForce);
	InputVector += WorldVector;
}

FVector UShipMovementComponent::ConsumeInputVector()
{
	Super::ConsumeInputVector();
	const FVector ReturnVector = InputVector;
	InputVector = FVector::ZeroVector;
	return ReturnVector;
}

void UShipMovementComponent::StartMoving()
{
	switch(PathFollowingComponent->GetStatus())
	{
	case EPathFollowingStatus::Idle:
		MovementState = EShipMovementState::Idle;
		break;
		
	case EPathFollowingStatus::Moving:
	{
		const FVector& CurrentLocation = GetOwner()->GetActorLocation();
		const FVector& DestinationLocation = PathFollowingComponent->GetPathDestination();
		if (FVector::Dist(CurrentLocation, DestinationLocation) > DecelerationDistance)
		{
			MovementState = EShipMovementState::Accelerate;
		}
		else
		{
			MovementState = EShipMovementState::Decelerate;
		}
		RollState = EShipRollState::Rolling;
		break;
	}
		
	case EPathFollowingStatus::Paused:
		MovementState = EShipMovementState::Stopping;
		RollState = EShipRollState::Roll_To_Zero;
		break;
		
	case EPathFollowingStatus::Waiting:
		MovementState = EShipMovementState::Stopping;
		RollState = EShipRollState::Roll_To_Zero;
		break;
		
	default:
		break;
	}
}

void UShipMovementComponent::CalculateSpeed(const float DistanceToTarget, const float DeltaTime)
{
	switch (MovementState)
	{
	case EShipMovementState::Idle:
	{
		StartMoving();
		break;
	}

	case EShipMovementState::Moving:
	{
		if (DistanceToTarget < DecelerationDistance)
		{
			MovementState = EShipMovementState::Decelerate;
		}
		break;
	}
		
	case EShipMovementState::Accelerate:
	{
		if (DistanceToTarget < DecelerationDistance)
		{
			MovementState = EShipMovementState::Decelerate;
			// a = V^2 / (2S)
			DecelerationRate = CurrentSpeed * CurrentSpeed / (2 * DecelerationDistance);
			break;
		}
	
		CurrentSpeed = FMath::Min(CurrentSpeed + AccelerationRate * DeltaTime, MaxSpeed);
		if (CurrentSpeed == MaxSpeed)
		{
			MovementState = EShipMovementState::Moving;
		}
		break;
	}
        
	case EShipMovementState::Decelerate:
	{
		CurrentSpeed = FMath::Max(CurrentSpeed - DecelerationRate * DeltaTime, 0.f);
		if (CurrentSpeed == 0.0f)
		{
			MovementState = EShipMovementState::Stopping;
		}
		break;
	}

	case EShipMovementState::Stopping:
	{
		CurrentSpeed = 0;
		MovementState = EShipMovementState::Idle;
		RollState = EShipRollState::Roll_To_Zero;
		break;
	}
        
	default:
		check(false);
		break;
	}
}

void UShipMovementComponent::CalculateRoll(FRotator& NextRotator, const FVector& NextLocation, const float DeltaTime)
{
	const float CurrentActorRoll = GetOwner()->GetActorRotation().Roll;
	
	switch (RollState)
	{
	case EShipRollState::No_Rolling:
	{
		CurrentRollSpeed = 0;
		//CurrentRollSpeed = FMath::Max(CurrentRollSpeed - AccelerationRollRate * DeltaTime, 0.f);
		break;
	}
		
	case EShipRollState::Rolling:
	{
		CurrentRollSpeed = FMath::Min(CurrentRollSpeed + AccelerationRollRate * DeltaTime, MaxRollSpeed);
		break;
	}
		
	case EShipRollState::Roll_To_Zero:
	{
		// If ship has some roll angle then it should be counter-rolled
		if (FMath::IsNearlyZero(FMath::Abs(CurrentActorRoll), 1E-2f) == true)
		{
			Rotator.Roll = 0;
			RollState = EShipRollState::No_Rolling;
			return;
		}

		CurrentRollSpeed = FMath::Max(CurrentRollSpeed - AccelerationRollRate * DeltaTime, 0.f);
		Rotator.Roll = CurrentRollSpeed * DeltaTime * CurrentActorRoll > 0 ? 1.f : -1.f;
		NextRotator.Roll = CurrentActorRoll - Rotator.Roll;
		break;
	}
	
	default:
		checkNoEntry();
		break;
	}
	
	const FVector& CurrentLocation = GetOwner()->GetActorLocation();
	const FVector& DirectionToNextLocation = NextLocation - CurrentLocation;
	const FVector& ForwardVector = GetOwner()->GetActorForwardVector();
	const float RotationAngle = AnglesFunctions::FindAngleBetweenVectorsOn2D(DirectionToNextLocation, ForwardVector);
	if (FMath::IsNearlyZero(RotationAngle) == false)
	{
		// Calculating banked turn
		// tan(DesiredRoll) = v^2 / r / g
		float DesiredRoll = UKismetMathLibrary::DegAtan(CurrentSpeed * CurrentSpeed / MaxTurnRadius / 9.81);
		DesiredRoll = FMath::Clamp(DesiredRoll, -MaxRollAngle, MaxRollAngle);
		
		const float DeltaRoll = CurrentRollSpeed * DeltaTime;
		// Set roll value in between +-DesiredRoll
		Rotator.Roll = FMath::Clamp(CurrentActorRoll + DeltaRoll, -DesiredRoll, DesiredRoll);
		NextRotator.Roll = Rotator.Roll;
		RollState = EShipRollState::Rolling;
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime*2, FColor::White, FString::SanitizeFloat(NextRotator.Roll));
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime*2, FColor::White, FString::SanitizeFloat(CurrentRollSpeed));
	}
}
