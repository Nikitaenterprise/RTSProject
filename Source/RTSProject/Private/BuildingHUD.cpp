#include "BuildingHUD.h"

#include "RTSPlayerController.h"

void UBuildingHUD::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerController = Cast<ARTSPlayerController>(GetOwningPlayer());

}

void UBuildingHUD::CheckButton()
{
	GEngine->AddOnScreenDebugMessage(-1, 0.5, FColor::White, TEXT("CheckButton()"));
}
