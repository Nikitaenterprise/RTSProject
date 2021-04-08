#include "BasicButtonsHUD.h"

#include "RTSPlayerController.h"
#include "ShipFactory.h"
#include "AsteroidFieldFactory.h"
#include "BuildingFactory.h"

#include "Kismet/KismetMathLibrary.h"

//UBasicButtonsHUD::UBasicButtonsHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
//
//}

void UBasicButtonsHUD::NativeConstruct() 
{
	Super::NativeConstruct();

	PlayerController = Cast<ARTSPlayerController>(GetOwningPlayer());
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan, TEXT("Created BasicButtonsHUD"));
}

void UBasicButtonsHUD::SpawnShip() 
{
	FVector v;
	v.X = UKismetMathLibrary::RandomIntegerInRange(-1000, 1000);
	v.Y = UKismetMathLibrary::RandomIntegerInRange(-1000, 1000);
	v.Z = 150;
	ShipFactory::NewShip(GetWorld(), FVector(-1000, 0, 150), PlayerController);
}

void UBasicButtonsHUD::SpawnAsteroidField()
{
	FVector v;
	v.X = UKismetMathLibrary::RandomIntegerInRange(-1000, 1000);
	v.Y = UKismetMathLibrary::RandomIntegerInRange(-1000, 1000);
	v.Z = 150;
	AsteroidFieldFactory::NewAsteroidField(GetWorld(), v, PlayerController);
}

void UBasicButtonsHUD::SpawnBuilding()
{
	FHitResult Hit;
	if (PlayerController->GetHitResultUnderCursorByChannel(
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
		false,
		Hit))
	{
		BuildingFactory::NewBuilding(GetWorld(), Hit.Location, PlayerController);
	}
}
