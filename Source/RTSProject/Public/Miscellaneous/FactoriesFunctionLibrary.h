#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "FactoriesFunctionLibrary.generated.h"

class ARTSPlayerController;
class AShip;
class AFighter;
class ASquad;
class ATurret;
class AProjectile;
class ABuilding;

UCLASS()
class RTSPROJECT_API UFactoriesFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext"))
	static AShip* NewShip(const UObject* WorldContext, UClass* ClassType, AController* PlayerController, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));
	static AShip* NewShip(UWorld* World, UClass* ClassType, AController* PlayerController, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext"))
	static AFighter* NewFighter(const UObject* WorldContext, UClass* ClassType, AController* PlayerController, ASquad* Squadron, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));
	static AFighter* NewFighter(UWorld* World, UClass* ClassType, AController* PlayerController, ASquad* Squadron, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext"))
	static ASquad* NewFighterSquadron(const UObject* WorldContext, UClass* ClassType, AController* PlayerController, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));
	static ASquad* NewFighterSquadron(UWorld* World, UClass* ClassType, AController* PlayerController, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));

	UFUNCTION(BlueprintCallable)
	static void AddTurretsToShip(UWorld* World, AShip* Ship);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext"))
	static ABuilding* NewBuilding(const UObject* WorldContext, UClass* ClassType, AController* PlayerController, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));
	static ABuilding* NewBuilding(UWorld* World, UClass* ClassType, AController* PlayerController, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext"))
	static AProjectile* NewRocket(const UObject* WorldContext, UClass* ClassType, AController* Controller, ATurret* Turret, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));
	static AProjectile* NewRocket(UWorld* World, UClass* ClassType, AController* Controller, ATurret* Turret, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));
	
};
