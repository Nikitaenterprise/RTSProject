#include "GameHUD.h"

#include "RTSPlayerController.h"
#include "BasicButtonsHUD.h"
#include "ShipHUD.h"
#include "BuildingHUD.h"

#include "Engine/Canvas.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

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
	float mouseX = 0, mouseY = 0;
	PlayerController->GetMousePosition(mouseX, mouseY);
	StartClick.X = mouseX;
	StartClick.Y = mouseY;
	HoldingLocation = StartClick;
	SelectedActors.Empty();
	bIsDrawing = true;
}

void AGameHUD::OnInputHold() 
{
	float mouseX = 0, mouseY = 0;
	PlayerController->GetMousePosition(mouseX, mouseY);
	HoldingLocation.X = mouseX;
	HoldingLocation.Y = mouseY;
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
	
	FVector2D res = HoldingLocation - StartClick;
	FLinearColor color(0, 1, 0, 0.2);
	DrawRect(color, StartClick.X, StartClick.Y, res.X, res.Y);

	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Cyan, StartClick.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Cyan, HoldingLocation.ToString());

	// Original selection
	GetActorsInSelectionRectangle<AActor>(StartClick, HoldingLocation, SelectedActors, true);
}

void AGameHUD::DrawSelectionRectAndSelectActors()
{
	if (bIsDrawing) {
		DrawMarquee();
		PlayerController->SelectedActors = SelectedActors;
		/*for (const auto& a : SelectedActors) {
			FString str = UKismetSystemLibrary::GetDisplayName(a);
			GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Cyan, str);
		}*/
		SelectedActors.Empty();
	}
}
