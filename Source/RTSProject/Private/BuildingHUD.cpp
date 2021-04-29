#include "BuildingHUD.h"


#include "Building.h"
#include "RTSPlayerController.h"

void UBuildingHUD::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerController = Cast<ARTSPlayerController>(GetOwningPlayer());

}

ABuilding* UBuildingHUD::GetSelectedBuilding()
{
	for(auto& b : PlayerController->SelectedActors)
	{
		ABuilding* Building = Cast<ABuilding>(b);
		if (Building) return Building;
	}
	return nullptr;
}

void UBuildingHUD::AddToBuildingQueue(const TSubclassOf<AActor>& ActorTypeToSpawn)
{
	if (!ActorTypeToSpawn) return;
	ABuilding* BuildingToAddTo = GetSelectedBuilding();
	if (!BuildingToAddTo) return;
	BuildingToAddTo->AddActorToBuildingQueue(ActorTypeToSpawn);
}
