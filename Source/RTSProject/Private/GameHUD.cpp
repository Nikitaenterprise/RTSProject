#include "GameHUD.h"

#include "RTSPlayerController.h"
#include "BasicButtonsHUD.h"
#include "ShipHUD.h"
#include "BuildingHUD.h"

//#include "Engine/Canvas.h"
//#include "Kismet/KismetMathLibrary.h"
//#include "Kismet/KismetArrayLibrary.h"
//#include "Kismet/KismetSystemLibrary.h"

AGameHUD::AGameHUD(const FObjectInitializer& OI) : Super(OI)
{
}

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();
	if (SetupWidget<UBasicButtonsHUD>(BasicButtonsHUD, BasicButtonsHUDClass))
		BasicButtonsHUD->AddToViewport();
	SetupWidget<UShipHUD>(ShipHUD, ShipHUDClass);
	SetupWidget<UBuildingHUD>(BuildingHUD, BuildingHUDClass);
}

void AGameHUD::DrawHUD()
{
	Super::DrawHUD();
	DrawSelectionRectAndSelectActors();
}

void AGameHUD::Tick(float mainDeltaTime)
{
	Super::Tick(mainDeltaTime);
}

void AGameHUD::BindController(ARTSPlayerController* Controller) 
{
	if (Controller) PlayerController = Controller;
}

TArray<AActor*>& AGameHUD::GetSelectedActors() 
{
	return SelectedActors;
}

void AGameHUD::OnInputStart() 
{

	PlayerController->GetMousePosition(StartClick.X, StartClick.Y);
	HoldingLocation = StartClick;
	SelectedActors.Empty();
	bIsDrawing = true;
}

void AGameHUD::OnInputHold() 
{
	PlayerController->GetMousePosition(HoldingLocation.X, HoldingLocation.Y);
}

void AGameHUD::OnInputRelease() 
{
	bIsDrawing = false;
	
}

void AGameHUD::DrawMarquee()
{
	DrawLine(StartClick.X, StartClick.Y, HoldingLocation.X, StartClick.Y, FColor::Green, 1);
	DrawLine(HoldingLocation.X, StartClick.Y, HoldingLocation.X, HoldingLocation.Y, FColor::Green, 1);
	DrawLine(HoldingLocation.X, HoldingLocation.Y, StartClick.X, HoldingLocation.Y, FColor::Green, 1);
	DrawLine(StartClick.X, HoldingLocation.Y, StartClick.X, StartClick.Y, FColor::Green, 1);
	
	const FVector2D RectSize = HoldingLocation - StartClick;
	const FLinearColor Color(0, 1, 0, 0.2);
	DrawRect(Color, StartClick.X, StartClick.Y, RectSize.X, RectSize.Y);

	/*GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Cyan, StartClick.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Cyan, HoldingLocation.ToString());*/

	GetActorsInSelectionRectangle<AActor>(StartClick, HoldingLocation, SelectedActors, false);
}

void AGameHUD::DrawSelectionRectAndSelectActors()
{
	if (bIsDrawing) {
		DrawMarquee();
		PlayerController->ShouldBeSelected = SelectedActors;
		SelectedActors.Empty();
	}
}
