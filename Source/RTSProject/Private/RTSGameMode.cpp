#include "RTSGameMode.h"
#include "RTSPlayerController.h"
#include "GameHUD.h"

ARTSGameMode::ARTSGameMode() : Super() {
	PlayerControllerClass = ARTSPlayerController::StaticClass();
	HUDClass = AGameHUD::StaticClass();
}