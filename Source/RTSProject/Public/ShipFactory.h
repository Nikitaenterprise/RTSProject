#pragma once

#include "CoreMinimal.h"

class AShip;
class ARTSPlayerController;
class ATurret;
struct FActorSpawnParameters;


class RTSPROJECT_API ShipFactory
{
public:
	static AShip* NewShip(UWorld* World, UClass* ClassType, ARTSPlayerController* Controller, const FVector& Location = FVector(0, 0, 0), const FRotator& Rotation = FRotator(0, 0, 0));
	static void AddTurretsToShip(AShip* Ship);
	
private:
	static FActorSpawnParameters GetDefaultSpawnParams();
};
