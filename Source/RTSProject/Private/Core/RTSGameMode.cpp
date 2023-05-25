#include "Core/RTSGameMode.h"
#include "Core/RTSPlayerController.h"
#include "Actors/FogOfWar.h"
#include "Actors/Resources/ResourceManager.h"
#include "Volumes/FogOfWarBoundsVolume.h"
#include "UI/GameHUD.h"

ARTSGameMode::ARTSGameMode() : Super()
{
	PlayerControllerClass = ARTSPlayerController::StaticClass();
	HUDClass = AGameHUD::StaticClass();
}

void ARTSGameMode::StartPlay()
{
    // Make sure that ARTSPlayerController::BeginPlay() will fire first
    // because of inner logic (FogOfWar, FogOfWarBoundsVolume, etc.)
    // After that other actor's BeginPlay() will fire

    ARTSPlayerController* PlayerController {nullptr};
    AFogOfWar* FogOfWar {nullptr};
    AFogOfWarBoundsVolume* FogOfWarBoundsVolume {nullptr};
    
	FindOnLevel<ARTSPlayerController>(PlayerController, ARTSPlayerController::StaticClass());
	PlayerController->DispatchBeginPlay();
	FindOnLevel<AFogOfWar>(FogOfWar, AFogOfWar::StaticClass());
	//TestFogOfWar->DispatchBeginPlay();
	FindOnLevel<AFogOfWarBoundsVolume>(FogOfWarBoundsVolume, AFogOfWarBoundsVolume::StaticClass());
    //TestFogOfWarBoundsVolume->DispatchBeginPlay();

	GetWorld()->SpawnActor<AResourceManager>(FActorSpawnParameters());
	Super::StartPlay();
}
