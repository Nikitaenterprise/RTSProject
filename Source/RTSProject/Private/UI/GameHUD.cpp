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