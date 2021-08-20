#include "Core/RTSGameMode.h"
#include "Core/RTSPlayerController.h"
#include "UI/GameHUD.h"

ARTSGameMode::ARTSGameMode() : Super() {
	PlayerControllerClass = ARTSPlayerController::StaticClass();
	HUDClass = AGameHUD::StaticClass();
}