#pragma once

#include "Interfaces/UnitMovementSystem.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "BoidsMovementSystem.generated.h"

class ASquad;

USTRUCT()
struct FBoidSquadron
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<AActor*> BoidSquadron;
};

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
	// Keeps boids from getting too close to one another
	void Separation(const ASquad* Squadron);
	// Calculates the average velocity of boids in the field of vision and
	// manipulates the velocity of the current boid in order to match it
	void Alignment(const ASquad* Squadron);
	// Finds the average location of nearby boids and manipulates the
	// steering force to move in that direction.
	void Cohesion(const ASquad* Squadron);
	void Apply();

	UPROPERTY()
	UWorld* World {nullptr};

	UPROPERTY()
	AActor* OwnerActor {nullptr};

	UPROPERTY()
	UPawnMovementComponent* MovementComponent {nullptr};
	
	UPROPERTY()
	TMap<ASquad*, FBoidSquadron> UnitsRegister;

	FVector LocationForTick = FVector::ZeroVector;
	FRotator RotationForTick = FRotator::ZeroRotator;
	FVector LocationToMove;
	bool bShouldMove = false;
	
	UPROPERTY()
	float MaxSpeed;

	UPROPERTY()
	float MaxForce;
};
