#include "Components/ShipMovementComponent.h"

#include "AIController.h"
#include "Interfaces/UnitMovementSystem.h"
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

	UnitMovingSystem = ObjectInitializer.CreateDefaultSubobject<UComplexUnitMovementSystem>(this, FName("UnitMovingSystem"));
	if (UnitMovingSystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("IUnitMovingSystem is nullptr in UShipMovementComponent::InitializeComponent()"));
	}
}

void UShipMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UnitMovingSystem->Initialize(GetOwner(), this);
}

void UShipMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (PawnOwner == nullptr ||
		UpdatedComponent == nullptr ||
		UnitMovingSystem == nullptr ||
		UnitMovingSystem->ShouldMove() == false ||
		ShouldSkipUpdate(DeltaTime))
	{
		return;
	}
	UnitMovingSystem->CalculatePosition(DeltaTime);
	AddInputVector(UnitMovingSystem->GetLocationForTick());
	FHitResult Hit;
	SafeMoveUpdatedComponent(ConsumeInputVector(), UnitMovingSystem->GetRotatorForTick(), true, Hit);
}

void UShipMovementComponent::AddInputVector(FVector WorldVector, bool bForce)
{
	InputVector += WorldVector;
}

FVector UShipMovementComponent::ConsumeInputVector()
{
	const FVector ReturnVector = InputVector;
	InputVector = FVector::ZeroVector;
	return ReturnVector;
}

void UShipMovementComponent::MoveTo(const FVector& Location)
{
	if (UnitMovingSystem == nullptr)
	{
		return;
	}
	UnitMovingSystem->RequestMoveTo(Location);
}
