#include "BuildingHUD.h"


#include "Building.h"
#include "RTSPlayerController.h"

void UBuildingHUD::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerController = Cast<ARTSPlayerController>(GetOwningPlayer());

}

TArray<ABuilding*> UBuildingHUD::GetSelectedBuildings()
{
	TArray<ABuilding*> SelectedBuildings;
	for(auto& b : PlayerController->SelectedActors)
	{
		ABuilding* Building = Cast<ABuilding>(b);
		if (Building) SelectedBuildings.AddUnique(Building);
	}
	return SelectedBuildings;
}

void UBuildingHUD::AddToBuildingQueue(ABuilding* BuildingToAddTo, const TSubclassOf<AActor>& ActorTypeToSpawn)
{
	if (!ActorTypeToSpawn) return;
	if (!BuildingToAddTo) return;
	BuildingToAddTo->RequestBuildingUnit(ActorTypeToSpawn);
}

int UBuildingHUD::GetNumberOfQueueUnitsInBuilding(const ABuilding* BuildingToGetFrom, TSubclassOf<AActor> ActorClass)
{
	if (!BuildingToGetFrom) return 0;
	return BuildingToGetFrom->GetBuildingQueueSizeByClass(ActorClass);
}
