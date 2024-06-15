#include "Components/Movement/BoidMovementComponent.h"

#include "Systems/UnitMovement/BoidsMovementSystem.h"

UBoidMovementComponent::UBoidMovementComponent(const FObjectInitializer& ObjectInitializer)
{
	UnitMovingSystem = ObjectInitializer.CreateDefaultSubobject<UBoidsMovementSystem>(this, FName("BoidsMovementSystem"));
	if (UnitMovingSystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("IUnitMovingSystem is nullptr in UShipMovementComponent::InitializeComponent()"));
	}
}

void UBoidMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	//UnitMovingSystem = GetWorld()->GetSubsystem<UBoidsMovementSystem>();
	if (UnitMovingSystem)
	{
		UnitMovingSystem->InitializeMovementSystem(GetOwner(), this);
	}
}

void UBoidMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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

void UBoidMovementComponent::AddInputVector(FVector WorldVector, bool bForce)
{
	InputVector += WorldVector;
}

FVector UBoidMovementComponent::ConsumeInputVector()
{
	const FVector ReturnVector = InputVector;
	InputVector = FVector::ZeroVector;
	return ReturnVector;
}
