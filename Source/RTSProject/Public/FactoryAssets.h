#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FactoryAssets.generated.h"


class AShip;
class ATurret;
class ABuilding;
class AAsteroidField;
class AAsteroidResource;
class AFogOfWar;

UCLASS(BlueprintType)
class RTSPROJECT_API UFactoryAssets : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ship")
	TArray<TSubclassOf<AShip>> ShipClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret")
	TSubclassOf<ATurret> TurretClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building")
	TSubclassOf<ABuilding> BuildingClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource")
	TSubclassOf<AAsteroidField> AsteroidFieldClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource")
	TSubclassOf<AAsteroidResource> AsteroidResourceClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game")
	TSubclassOf<AFogOfWar> FogOfWarClass;
	
public:

	UFUNCTION(BlueprintCallable)
	TArray<TSubclassOf<AShip>> GetShipClass() { return ShipClass; };
};
