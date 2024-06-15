#include "Core/RTSGameMode.h"

#include "AIController.h"
#include "Actors/FogOfWar.h"
#include "Actors/RTSPlayer.h"
#include "Actors/Resources/ResourceManager.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Core/RTSPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Systems/RTSPlayerState.h"
#include "UI/GameHUD.h"
#include "Volumes/FogOfWarBoundsVolume.h"

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
	
	if (FindOnLevel<AResourceManager>(ResourceManager, AResourceManager::StaticClass()) == false)
	{
		ResourceManager = GetWorld()->SpawnActor<AResourceManager>(FActorSpawnParameters());
	}
	
	Super::StartPlay();
}

void ARTSGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Get real player start position
	FVector PlayerLocation;
	for (TActorIterator<ARTSPlayer> It(GetWorld()); It; ++It)
	{
		if (const ARTSPlayer* Player = *It)
		{
			PlayerLocation = Player->GetActorLocation();
			// Setup team for real player
			if (auto* RTSPlayerState = Cast<ARTSPlayerState>(Player->GetPlayerState()))
			{
				FGenericTeamId TeamId(0);
				RTSPlayerState->SetGenericTeamId(TeamId);
			}
		}
	}
	
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		if (const APlayerStart* PlayerStart = *It)
		{
			if (PlayerStart->GetActorLocation().Equals(PlayerLocation, 0.01) == false)
			{
				if (auto* Bot = Cast<ARTSPlayer>(UAIBlueprintHelperLibrary::SpawnAIFromClass(GetWorld(), DefaultPawnClass, nullptr, PlayerStart->GetActorLocation())))
				{
					// We need to use PlayerState for this bot
					// TODO: remove this when network is done
					auto* PlayerState = Cast<ARTSPlayerState>(Bot->GetPlayerState());
					// Setup team for bot
					FGenericTeamId TeamId(1);
					PlayerState->SetGenericTeamId(TeamId);
				}
			}
		}
	}
}

ARTSGameMode* ARTSGameMode::GetRTSGameMode(const UObject* WorldContextObject)
{
	const auto* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	return World ? Cast<ThisClass>(World->GetAuthGameMode()) : nullptr;
}
