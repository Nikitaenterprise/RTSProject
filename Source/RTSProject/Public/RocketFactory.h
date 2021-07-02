#pragma once

#include "CoreMinimal.h"

class ATurret;
class ARTSPlayerController;
class UWorld;
struct FActorSpawnParameters;
class ARocket;

class RTSPROJECT_API RocketFactory
{
public:
	static ARocket* NewRocket(UWorld* World, ARTSPlayerController* Controller, ATurret* Turret);
	static ARocket* NewRocket(UWorld* World, ARTSPlayerController* Controller, ATurret* Turret, const FTransform& Transform);
private:
	static FActorSpawnParameters GetDefaultSpawnParams();
};
