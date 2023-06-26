#pragma once

#include "Engine/DataAsset.h"
#include "FactoryAssets.generated.h"

class AResource;
class AFogOfWar;
class AFogOfWarBoundsVolume;

class AShip;
class ATurret;
class AProjectile;
class ABuilding;

UCLASS(BlueprintType)
class RTSPROJECT_API UFactoryAssets : public UDataAsset
{
	GENERATED_BODY()

private:
	// Classes that were created in editor by inheriting from base classes

	UPROPERTY(EditAnywhere, Category = "FogOfWar", meta = (AllowPrivateAcces = "true"))
	TSubclassOf<AFogOfWar> FogOfWarClass;
	UPROPERTY(EditAnywhere, Category = "FogOfWar", meta = (AllowPrivateAcces = "true"))
	TSubclassOf<AFogOfWarBoundsVolume> FogOfWarBoundsVolumeClass;
	
	UPROPERTY(EditAnywhere, Category = "Ship", meta = (AllowPrivateAcces = "true"))
	TArray<TSubclassOf<AShip>> ShipClasses;
	UPROPERTY(EditAnywhere, Category = "Turret", meta = (AllowPrivateAcces = "true"))
	TArray<TSubclassOf<ATurret>> TurretClasses;
	UPROPERTY(EditAnywhere, Category = "Rocket", meta = (AllowPrivateAcces = "true"))
	TArray<TSubclassOf<AProjectile>> RocketClasses;
	UPROPERTY(EditAnywhere, Category = "Building", meta = (AllowPrivateAcces = "true"))
	TArray<TSubclassOf<ABuilding>> BuildingClasses;
	UPROPERTY(EditAnywhere, Category = "Resource", meta = (AllowPrivateAcces = "true"))
	TArray<TSubclassOf<AResource>> ResourceClasses;
	
public:
	// Returns copy of TSubclassOf<AFogOfWar> FogOfWarClass
	UFUNCTION(BlueprintCallable)
	TSubclassOf<AFogOfWar> GetFogOfWarClass() { return FogOfWarClass; }
	// Returns coy of TSubclassOf<AFogOfWarBoundsVolume> FogOfWarBoundsVolumeClass
	UFUNCTION(BlueprintCallable)
	TSubclassOf<AFogOfWarBoundsVolume> GetFogOfWarBoundsVolumeClass() { return FogOfWarBoundsVolumeClass; }

	// Returns array copy of TArray<TSubclassOf<AShip>> ShipClasses
	UFUNCTION(BlueprintCallable)
	TArray<TSubclassOf<AShip>> GetShipClasses() const { return ShipClasses; }
	// Returns copy of TArray<TSubclassOf<AShip>> ShipClasses[i] if Index is in borders of array, else returns first element
	UFUNCTION(BlueprintCallable)
	TSubclassOf<AShip> GetShipClass(int Index) const { return Index > 0 && Index <= ShipClasses.Num() ? ShipClasses[Index] : ShipClasses[0]; }
	// Returns array copy of TArray<TSubclassOf<ATurret>> TurretClasses
	UFUNCTION(BlueprintCallable)
	TArray<TSubclassOf<ATurret>> GetTurretClasses() const { return TurretClasses; }
	// Returns copy of TArray<TSubclassOf<ATurret>> TurretClasses[i] if Index is in borders of array, else returns first element
	UFUNCTION(BlueprintCallable)
	TSubclassOf<ATurret> GetTurretClass(int Index) const { return Index > 0 && Index <= TurretClasses.Num() ? TurretClasses[Index] : TurretClasses[0]; }
	// Returns array copy of TArray<TSubclassOf<ARocket>> RocketClasses
	UFUNCTION(BlueprintCallable)
	TArray<TSubclassOf<AProjectile>> GetRocketClasses() const { return RocketClasses; }
	// Returns copy of TArray<TSubclassOf<ARocket>> RocketClasses[i] if Index is in borders of array, else returns first element
	UFUNCTION(BlueprintCallable)
	TSubclassOf<AProjectile> GetRocketClass(int Index) const { return Index > 0 && Index <= RocketClasses.Num() ? RocketClasses[Index] : RocketClasses[0]; }
	// Returns array copy of TArray<TSubclassOf<ABuilding>> BuildingClasses
	UFUNCTION(BlueprintCallable)
	TArray<TSubclassOf<ABuilding>> GetBuildingClasses() const { return BuildingClasses; }
	// Returns copy of TArray<TSubclassOf<ABuilding>> BuildingClasses[i] if Index is in borders of array, else returns first element
	UFUNCTION(BlueprintCallable)
	TSubclassOf<ABuilding> GetBuildingClass(int Index) const { return Index > 0 && Index <= BuildingClasses.Num() ? BuildingClasses[Index] : BuildingClasses[0]; }
	// Returns array copy of TArray<TSubclassOf<AAsteroidField>> AsteroidFieldClasses
	UFUNCTION(BlueprintCallable)
	TArray<TSubclassOf<AResource>> GetResourceClasses() const { return ResourceClasses; }
	// Returns copy of TArray<TSubclassOf<AAsteroidField>> AsteroidFieldClasses[i] if Index is in borders of array, else returns first element
	UFUNCTION(BlueprintCallable)
	TSubclassOf<AResource> GetResourceClass(int Index) const { return Index > 0 && Index <= ResourceClasses.Num() ? ResourceClasses[Index] : ResourceClasses[0]; }
};