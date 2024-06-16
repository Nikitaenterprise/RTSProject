#include "Systems/UnitMovement/BoidsMovementSystem.h"

#include "Actors/Units/Squad.h"

void UBoidsMovementSystem::InitializeMovementSystem(AActor* InOwnerActor, UPawnMovementComponent* InMovementComponent)
{

}

bool UBoidsMovementSystem::RequestMoveTo(const FVector& Location)
{
	return true;
}

void UBoidsMovementSystem::CalculatePosition(float DeltaTime)
{
}

void UBoidsMovementSystem::MakePath()
{
}