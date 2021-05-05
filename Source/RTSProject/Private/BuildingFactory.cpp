#include "BuildingFactory.h"

#include "Building.h"
#include "FactoryAssets.h"
#include "RTSPlayerController.h"

ABuilding* BuildingFactory::NewBuilding(UWorld* _World, ARTSPlayerController* _Controller)
{
	return NewBuilding(_World, FVector(0, 0, 0), FRotator(0, 0, 0), _Controller);
}

ABuilding* BuildingFactory::NewBuilding(UWorld* _World, const FVector& _Location, ARTSPlayerController* _Controller)
{
	return NewBuilding(_World, _Location, FRotator(0, 0, 0), _Controller);
}

ABuilding* BuildingFactory::NewBuilding(UWorld* _World, const FVector& _Location, const FRotator& _Rotation, ARTSPlayerController* _Controller)
{
	if (!_Controller) return nullptr;

	const TSubclassOf<ABuilding> BuildingClass = _Controller->GetFactoryAssets()->BuildingClass;
	if (!BuildingClass) return nullptr;

	ABuilding* SpawnedBuilding = _World->SpawnActor<ABuilding>(
								BuildingClass.Get(), 
								_Location,
								_Rotation,
								GetDefaultSpawnParams());
	if (!SpawnedBuilding)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn building"));
		return nullptr;
	}
	SpawnedBuilding->Initialize(_Controller);
	SpawnedBuilding->bJustCreated = true;
	_Controller->PlayersActors.AddUnique(SpawnedBuilding);
	return SpawnedBuilding;
}

FActorSpawnParameters BuildingFactory::GetDefaultSpawnParams()
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Instigator = nullptr;
	Params.Owner = nullptr;
	return Params;
}

