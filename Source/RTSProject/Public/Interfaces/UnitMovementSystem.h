#pragma once

#include "UObject/Interface.h"
#include "UnitMovementSystem.generated.h"

UINTERFACE()
class UUnitMovementSystem : public UInterface
{
	GENERATED_BODY()
};

class RTSPROJECT_API IUnitMovementSystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(AActor* InOwnerActor, UPawnMovementComponent* InMovementComponent) = 0;
	virtual const FRotator& GetRotatorForTick() = 0;
	virtual const FVector& GetLocationForTick() = 0;
	virtual bool RequestMoveTo(const FVector& Location) = 0;
	virtual bool IsUsingCapsuleCollision() = 0;
	virtual bool ShouldMove() = 0;
	virtual bool IsMovingInPlane() = 0;
	virtual void CalculatePosition(float DeltaTime) = 0;

protected:
	virtual void MakePath() = 0;
	
};
