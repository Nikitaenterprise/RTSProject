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

	UPROPERTY(EditDefaultsOnly, Category = "Ship")
	TSubclassOf<AShip> ShipClass;
	UPROPERTY(EditDefaultsOnly, Category = "Turret")
	TSubclassOf<ATurret> TurretClass;
	UPROPERTY(EditDefaultsOnly, Category = "Building")
	TSubclassOf<ABuilding> BuildingClass;
	UPROPERTY(EditDefaultsOnly, Category = "Resource")
	TSubclassOf<AAsteroidField> AsteroidFieldClass;
	UPROPERTY(EditDefaultsOnly, Category = "Resource")
	TSubclassOf<AAsteroidResource> AsteroidResourceClass;
	
public:

};
