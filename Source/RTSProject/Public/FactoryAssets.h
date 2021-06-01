#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FactoryAssets.generated.h"

class AFogOfWar;
class AFogOfWarBoundsVolume;

class AShip;
class ATurret;
class ABuilding;
class AAsteroidField;
class AAsteroidResource;

UCLASS(BlueprintType)
class RTSPROJECT_API UFactoryAssets : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FogOfWar")
	TSubclassOf<AFogOfWar> FogOfWarClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FogOfWar")
	TSubclassOf<AFogOfWarBoundsVolume> FogOfWarBoundsVolumeClass;
	
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
	
public:

	UFUNCTION(BlueprintCallable)
	TArray<TSubclassOf<AShip>> GetShipClass() { return ShipClass; };
};
