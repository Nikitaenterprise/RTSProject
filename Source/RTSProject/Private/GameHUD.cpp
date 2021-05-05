#include "GameHUD.h"

#include "RTSPlayerController.h"
#include "BasicButtonsHUD.h"
#include "ShipHUD.h"
#include "BuildingHUD.h"


AGameHUD::AGameHUD(const FObjectInitializer& OI) : Super(OI)
{
}

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();
	if (SetupWidget<UBasicButtonsHUD>(BasicButtonsHUD, BasicButtonsHUDClass))
		BasicButtonsHUD->AddToViewport();
	if(SetupWidget<UShipHUD>(ShipHUD, ShipHUDClass))
		ShipHUD->AddToViewport();
	if(SetupWidget<UBuildingHUD>(BuildingHUD, BuildingHUDClass))
		BuildingHUD->AddToViewport();
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

void AGameHUD::ShowBasicButtonsHUD() const
{
	if (bIsShowingBasicButtonsHUD) return;
	BasicButtonsHUD->SetVisibility(ESlateVisibility::Visible);
	BuildingHUD->SetVisibility(ESlateVisibility::Hidden);
	ShipHUD->SetVisibility(ESlateVisibility::Hidden);
}

void AGameHUD::ShowBuildingHUD() const
{
	if (bIsShowingBuildingHUD) return;
	BuildingHUD->SetVisibility(ESlateVisibility::Visible);
	BasicButtonsHUD->SetVisibility(ESlateVisibility::Hidden);
	ShipHUD->SetVisibility(ESlateVisibility::Hidden);
}

void AGameHUD::ShowShipHUD() const
{
	if (bIsShowingShipHUD) return;
	ShipHUD->SetVisibility(ESlateVisibility::Visible);
	BasicButtonsHUD->SetVisibility(ESlateVisibility::Hidden);
	BuildingHUD->SetVisibility(ESlateVisibility::Hidden);
}

void AGameHUD::OnInputStart() 
{
	PlayerController->GetMousePosition(StartClick.X, StartClick.Y);
	HoldingLocation = StartClick;
	SelectedActors.Empty();
	bIsDrawingSelectionRectangle = true;
}

void AGameHUD::OnInputHold() 
{
	PlayerController->GetMousePosition(HoldingLocation.X, HoldingLocation.Y);
}

void AGameHUD::OnInputRelease() 
{
	bIsDrawingSelectionRectangle = false;
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

	GetActorsInSelectionRectangle<AActor>(StartClick, HoldingLocation, SelectedActors, false);
}

void AGameHUD::DrawSelectionRectAndSelectActors()
{
	if (bIsDrawingSelectionRectangle) {
		DrawMarquee();
		PlayerController->ShouldBeSelected = SelectedActors;
		SelectedActors.Empty();
	}
}
