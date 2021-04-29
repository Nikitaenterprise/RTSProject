#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FactoryAssets.generated.h"


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
	
	UPROPERTY(BlueprintReadOnly, Category = "Ship")
	TSubclassOf<AShip> ShipClass;
	UPROPERTY(BlueprintReadOnly, Category = "Turret")
	TSubclassOf<ATurret> TurretClass;
	UPROPERTY(BlueprintReadOnly, Category = "Building")
	TSubclassOf<ABuilding> BuildingClass;
	UPROPERTY(BlueprintReadOnly, Category = "Resource")
	TSubclassOf<AAsteroidField> AsteroidFieldClass;
	UPROPERTY(BlueprintReadOnly, Category = "Resource")
	TSubclassOf<AAsteroidResource> AsteroidResourceClass;
	
public:

	UFUNCTION(BlueprintCallable)
	TSubclassOf<AShip> GetShipClass() { return ShipClass; };
};
