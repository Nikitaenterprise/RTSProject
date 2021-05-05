#include "ShipHUD.h"

#include "RTSPlayerController.h"

void UShipHUD::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerController = Cast<ARTSPlayerController>(GetOwningPlayer());
}
