#include "UI/BasicUIWidget.h"

#include "Core/RTSPlayerController.h"
#include "Actors/Units/Ship.h"
#include "Actors/Buildings/Building.h"

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
		return PlayerController->IsArrayContainThisTypeActors<AShip>(PlayerController->GetSelectedActors());
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
		return PlayerController->IsArrayContainThisTypeActors<ABuilding>(PlayerController->GetSelectedActors());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr in UBasicUIWidget::IsShipsAreSelected"));
		return false;
	}
}
