#include "BasicButtonsHUD.h"

#include "RTSPlayerController.h"

#include "AsteroidFieldFactory.h"
#include "BuildingFactory.h"
#include "FactoryAssets.h"
#include "ShipFactory.h"
#include "Ship.h"
#include "Kismet/KismetMathLibrary.h"

//UBasicButtonsHUD::UBasicButtonsHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
//
//}

void UBasicButtonsHUD::NativeConstruct() 
{
	Super::NativeConstruct();

	PlayerController = Cast<ARTSPlayerController>(GetOwningPlayer());
}

void UBasicButtonsHUD::SpawnShip() 
{
	FVector v;
	v.X = UKismetMathLibrary::RandomIntegerInRange(-1000, 1000);
	v.Y = UKismetMathLibrary::RandomIntegerInRange(-1000, 1000);
	v.Z = 150;
	AShip* SpawnedShip = ShipFactory::NewShip(GetWorld(), PlayerController->GetFactoryAssets()->GetShipClass(0).Get(), PlayerController,FVector(-1000, 0, 0));
	if (!SpawnedShip) return;
	ShipFactory::AddTurretsToShip(SpawnedShip);
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
	BuildingFactory::NewBuilding(GetWorld(), FVector(0,0,-100), PlayerController);
}
