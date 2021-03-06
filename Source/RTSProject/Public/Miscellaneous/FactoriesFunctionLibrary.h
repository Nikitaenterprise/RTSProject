#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FactoriesFunctionLibrary.generated.h"

class ARTSPlayerController;
class AShip;
class ATurret;
class ARocket;
class ABuilding;
class AAsteroidField;

UCLASS()
class RTSPROJECT_API UFactoriesFunctionLibrary : public UBlueprintFunctionLibrary
{

	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext"))
	static AShip* NewShip(const UObject* WorldContext, UClass* ClassType, ARTSPlayerController* Controller, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));
	static AShip* NewShip(UWorld* World, UClass* ClassType, ARTSPlayerController* Controller, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));

	UFUNCTION(BlueprintCallable)
	static void AddTurretsToShip(AShip* Ship);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext"))
	static ABuilding* NewBuilding(const UObject* WorldContext, UClass* ClassType, ARTSPlayerController* Controller, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));
	static ABuilding* NewBuilding(UWorld* World, UClass* ClassType, ARTSPlayerController* Controller, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext"))
	static AAsteroidField* NewAsteroidField(const UObject* WorldContext, UClass* ClassType, ARTSPlayerController* Controller, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));
	static AAsteroidField* NewAsteroidField(UWorld* World, UClass* ClassType, ARTSPlayerController* Controller, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext"))
	static ARocket* NewRocket(const UObject* WorldContext, UClass* ClassType, ARTSPlayerController* Controller, ATurret* Turret, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));
	static ARocket* NewRocket(UWorld* World, UClass* ClassType, ARTSPlayerController* Controller, ATurret* Turret, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));
	
};
