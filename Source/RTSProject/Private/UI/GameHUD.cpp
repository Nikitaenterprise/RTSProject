#include "UI/GameHUD.h"

#include "Core/RTSPlayerController.h"
#include "UI/MiniMapWidget.h"
#include "UI/SelectionRectangleWidget.h"

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();

	ARTSPlayerController* TestPlayerController = Cast<ARTSPlayerController>(GetOwningPlayerController());
	if (TestPlayerController)
	{
		PlayerController = TestPlayerController;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("TestPlayerController is nullptr in AGameHUD::BeginPlay()"));
	}
	//if (SetupWidget<UMiniMapHUD>(MiniMapHUD, MiniMapHUDClass))
	//{
	//	MiniMapHUD->Initialize(PlayerController);
	//	//MiniMapHUD->AddToViewport();
	//}
	//else UE_LOG(LogTemp, Error, TEXT("Couldn't add UMiniMapHUD to viewport"));
}

void AGameHUD::DrawHUD()
{
	Super::DrawHUD();
	//DrawSelectionRectAndSelectActors();
}

void AGameHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGameHUD::LockSelectionRectangle() const
{
	if (SelectionRectangleWidget)
	{
		SelectionRectangleWidget->bIsSelectionShouldBeBlocked = true;
	}
}

void AGameHUD::UnlockSelectionRectangle() const
{
	if (SelectionRectangleWidget)
	{
		SelectionRectangleWidget->bIsSelectionShouldBeBlocked = false;
	}
}

//TArray<AActor*>& AGameHUD::GetSelectedActors() 
//{
//	return SelectedActors;
//}
//
//void AGameHUD::OnInputStart() 
//{
//	PlayerController->GetMousePosition(StartClick.X, StartClick.Y);
//	HoldingLocation = StartClick;
//	SelectedActors.Empty();
//	bIsDrawingSelectionRectangle = true;
//}
//
//void AGameHUD::OnInputHold() 
//{
//	PlayerController->GetMousePosition(HoldingLocation.X, HoldingLocation.Y);
//}
//
//void AGameHUD::OnInputRelease() 
//{
//	bIsDrawingSelectionRectangle = false;
//}
//
//void AGameHUD::DrawMarquee()
//{
//	DrawLine(StartClick.X, StartClick.Y, HoldingLocation.X, StartClick.Y, FColor::Green, 1);
//	DrawLine(HoldingLocation.X, StartClick.Y, HoldingLocation.X, HoldingLocation.Y, FColor::Green, 1);
//	DrawLine(HoldingLocation.X, HoldingLocation.Y, StartClick.X, HoldingLocation.Y, FColor::Green, 1);
//	DrawLine(StartClick.X, HoldingLocation.Y, StartClick.X, StartClick.Y, FColor::Green, 1);
//	
//	const FVector2D RectSize = HoldingLocation - StartClick;
//	const FLinearColor Color(0, 1, 0, 0.2);
//	DrawRect(Color, StartClick.X, StartClick.Y, RectSize.X, RectSize.Y);
//
//	GetActorsInSelectionRectangle<AActor>(StartClick, HoldingLocation, SelectedActors, false);
//}
//
//void AGameHUD::DrawSelectionRectAndSelectActors()
//{
//	if (bIsDrawingSelectionRectangle) {
//		DrawMarquee();
//		PlayerController->ShouldBeSelected = SelectedActors;
//		SelectedActors.Empty();
//	}
//}
