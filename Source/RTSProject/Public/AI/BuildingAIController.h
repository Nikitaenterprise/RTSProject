#pragma once

#include "AI/RTSAIController.h"
#include "BuildingAIController.generated.h"

class ABuilding;

UCLASS()
class RTSPROJECT_API ABuildingAIController : public ARTSAIController
{
	GENERATED_BODY()

public:
	ABuildingAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void ObtainPlayerState() override;

	UPROPERTY()
	ABuilding* Building { nullptr };
};
