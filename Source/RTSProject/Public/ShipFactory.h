#pragma once

#include "CoreMinimal.h"

class UWorld;
class AShip;
class ARTSPlayerController;
class ATurret;
struct FActorSpawnParameters;

class RTSPROJECT_API ShipFactory
{
public:
	static AShip* NewShip(UWorld* _World, ARTSPlayerController* _Controller);
	static AShip* NewShip(UWorld* _World, const FVector& _Location, ARTSPlayerController* _Controller);
	static AShip* NewShip(UWorld* _World, const FVector& _Location, const FRotator& _Rotation, ARTSPlayerController* _Controller);

private:
	static FActorSpawnParameters GetDefaultSpawnParams();
	static void AddTurretsToShip(UWorld* _World, ARTSPlayerController* _Controller, AShip* _Ship);

	static void SetTurretSide(ATurret* _Turret);
};
