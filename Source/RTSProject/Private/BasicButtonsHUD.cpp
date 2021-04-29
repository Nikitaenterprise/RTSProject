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
	//ShipFactory::NewShip(GetWorld(), FVector(-1000, 0, -100), PlayerController);
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
