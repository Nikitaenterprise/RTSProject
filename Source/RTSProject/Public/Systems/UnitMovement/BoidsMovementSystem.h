#pragma once

#include "Interfaces/UnitMovementSystem.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "BoidsMovementSystem.generated.h"

class ASquad;

UCLASS()
class RTSPROJECT_API UBoidsMovementSystem : public UObject, public IUnitMovementSystem
{
	GENERATED_BODY()
public:
	virtual void InitializeMovementSystem(AActor* InOwnerActor, UPawnMovementComponent* InMovementComponent) override;
	virtual const FRotator& GetRotatorForTick() override { return RotationForTick; }
	virtual const FVector& GetLocationForTick() override { return LocationForTick; }
	virtual bool RequestMoveTo(const FVector& Location) override;
	virtual bool IsUsingCapsuleCollision() override { return false; }
	virtual bool ShouldMove() override { return bShouldMove; }
	virtual bool IsMovingInPlane() override { return false; }
	virtual void CalculatePosition(float DeltaTime) override;

	// Distance of field of vision for separation between boids
	UPROPERTY()
	float DesiredSeparation = 20;

	UPROPERTY()
	float NeighborDist = 50;
	
protected:
	virtual void MakePath() override;

	UPROPERTY()
	UWorld* World {nullptr};

	UPROPERTY()
	AActor* OwnerActor {nullptr};

	UPROPERTY()
	UPawnMovementComponent* MovementComponent {nullptr};

	FVector LocationForTick = FVector::ZeroVector;
	FRotator RotationForTick = FRotator::ZeroRotator;
	FVector LocationToMove;
	bool bShouldMove = false;
	
	UPROPERTY()
	float MaxSpeed;

	UPROPERTY()
	float MaxForce;
};
