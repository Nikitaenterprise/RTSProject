#pragma once

#include "RTSAIController.h"

#include "ShipAIController.generated.h"

class AShip;
class UShipMovementComponent;

UCLASS()
class RTSPROJECT_API AShipAIController : public ARTSAIController
{
	GENERATED_BODY()

public:
	AShipAIController(const FObjectInitializer& ObjectInitializer);
	
	virtual FPathFollowingRequestResult MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath = nullptr) override;
	virtual void FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const override;
	
protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void ObtainPlayerState() override;
	
	UPROPERTY()
	AShip* Ship {nullptr};

	UPROPERTY()
	UShipMovementComponent* ShipMovementComponent {nullptr};

	FNavPathSharedPtr* CurrentNavPath = nullptr;
};
