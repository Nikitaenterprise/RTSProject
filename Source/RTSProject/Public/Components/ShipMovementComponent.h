#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "ShipMovementComponent.generated.h"

class IUnitMovementSystem;
class ARTSPlayerController;
class ARTSAIController;
class AShip;

UCLASS()
class RTSPROJECT_API UShipMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UShipMovementComponent(const FObjectInitializer& ObjectInitializer);
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void AddInputVector(FVector WorldVector, bool bForce = false) override;
	virtual FVector ConsumeInputVector() override;
	void MoveTo(const FVector& Location);

protected:
	UPROPERTY()
	TScriptInterface<IUnitMovementSystem> UnitMovingSystem {nullptr};

	// Normalized direction vector to shift this frame
	FVector InputVector = FVector(0, 0, 0);
};
