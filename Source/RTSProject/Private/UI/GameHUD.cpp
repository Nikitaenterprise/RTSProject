#include "UI/GameHUD.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Core/RTSPlayerController.h"
#include "Engine/UserInterfaceSettings.h"
#include "UI/MiniMapWidget.h"
#include "UI/MouseCursorWidget.h"
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

	if (auto* UserInterfaceSettings = GetDefault<UUserInterfaceSettings>())
	{
		//check(Cast<UMouseCursorWidget>(UserInterfaceSettings->SoftwareCursors[EMouseCursor::Default].ResolveClass()));
		TArray<UUserWidget*> FoundWidgets;
		UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetGameInstance(), FoundWidgets, UMouseCursorWidget::StaticClass(), false);
		if (FoundWidgets.Num() != 0)
		{
			MouseCursorWidget = Cast<UMouseCursorWidget>(FoundWidgets[0]);
		}
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