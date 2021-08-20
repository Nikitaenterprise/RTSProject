#include "BasicUIWidget.h"

#include "RTSPlayerController.h"
#include "Ship.h"
#include "Building.h"

void UBasicUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ARTSPlayerController* TestPlayerController = Cast<ARTSPlayerController>(GetOwningPlayer());
	if (TestPlayerController) PlayerController = TestPlayerController;
	else UE_LOG(LogTemp, Error, TEXT("TestPlayerController is nullptr in UBasicUIWidget::NativeConstruct()"));
}

bool UBasicUIWidget::IsShipsAreSelected() const
{
	if (PlayerController)
	{
		return PlayerController->IsArrayContainThisTypeActors<AShip>(PlayerController->SelectedActors);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr in UBasicUIWidget::IsShipsAreSelected"));
		return false;
	}
}

bool UBasicUIWidget::IsBuildingsAreSelected() const
{
	if (PlayerController)
	{
		return PlayerController->IsArrayContainThisTypeActors<ABuilding>(PlayerController->SelectedActors);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr in UBasicUIWidget::IsShipsAreSelected"));
		return false;
	}
}
