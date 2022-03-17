#include "Core/RTSGameMode.h"

#include "Core/RTSPlayerController.h"
#include "Actors/FogOfWar.h"
#include "Actors/ResourceManager.h"
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

    ARTSPlayerController* TestPlayerController = nullptr;
    AFogOfWar* TestFogOfWar = nullptr;
    AFogOfWarBoundsVolume* TestFogOfWarBoundsVolume = nullptr;

    UWorld* World = GetWorld();
    if (!IsValid(World))
    {
        UE_LOG(LogTemp, Error, TEXT("World is nullptr in ARTSGameMode::StartPlay()"))
    }
    for (TActorIterator<ARTSPlayerController> It(World, ARTSPlayerController::StaticClass()); It; ++It)
    {
        TestPlayerController = *It;
        if (!IsValid(TestPlayerController))
        {
            UE_LOG(LogTemp, Error, TEXT("ARTSPlayerController is nullptr in ARTSGameMode::StartPlay()"))
        }
        else if (IsValid(TestPlayerController))
        {
            UE_LOG(LogTemp, Display, TEXT("ARTSPlayerController is found on level in ARTSGameMode::StartPlay()"))
            break;
        }
        
    }
    TestPlayerController->DispatchBeginPlay();

    for (TActorIterator<AFogOfWar> It(World, AFogOfWar::StaticClass()); It; ++It)
    {
        TestFogOfWar = *It;
        if (!IsValid(TestFogOfWar))
        {
            UE_LOG(LogTemp, Error, TEXT("AFogOfWar is nullptr in ARTSGameMode::StartPlay()"))
        }
        else if (IsValid(TestFogOfWar))
        {
            UE_LOG(LogTemp, Display, TEXT("AFogOfWar is found on level in ARTSGameMode::StartPlay()"))
            break;
        }
    }
    //TestFogOfWar->DispatchBeginPlay();
    for (TActorIterator<AFogOfWarBoundsVolume> It(World, AFogOfWarBoundsVolume::StaticClass()); It; ++It)
    {
        TestFogOfWarBoundsVolume = *It;
        if (!IsValid(TestFogOfWarBoundsVolume))
        {
            UE_LOG(LogTemp, Error, TEXT("AFogOfWarBoundsVolume is nullptr in ARTSGameMode::StartPlay()"))
        }
        else if (IsValid(TestFogOfWarBoundsVolume))
        {
            UE_LOG(LogTemp, Display, TEXT("AFogOfWarBoundsVolume is found on level in ARTSGameMode::StartPlay()"))
            break;
        }
    }
    //TestFogOfWarBoundsVolume->DispatchBeginPlay();

	GetWorld()->SpawnActor<AResourceManager>(FActorSpawnParameters());
	Super::StartPlay();
}
