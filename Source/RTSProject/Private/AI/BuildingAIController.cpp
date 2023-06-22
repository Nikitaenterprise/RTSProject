#include "AI/BuildingAIController.h"

#include "Actors/Buildings/Building.h"

ABuildingAIController::ABuildingAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABuildingAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ABuildingAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	Building = Cast<ABuilding>(GetPawn());
}

void ABuildingAIController::ObtainPlayerState()
{
	Super::ObtainPlayerState();

	RTSPlayerState = Building ? Building->GetRTSPlayerState() : nullptr;
}
