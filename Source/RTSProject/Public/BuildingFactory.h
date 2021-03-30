#pragma once

#include "CoreMinimal.h"

class ABuilding;
class ARTSPlayerController;
class UWorld;
struct FActorSpawnParameters;

class RTSPROJECT_API BuildingFactory
{
public:
	
	static ABuilding* NewBuilding(UWorld* _World, ARTSPlayerController* _Controller);
	static ABuilding* NewBuilding(UWorld* _World, const FVector& _Location, ARTSPlayerController* _Controller);
	static ABuilding* NewBuilding(UWorld* _World, const FVector& _Location, const FRotator& _Rotation, ARTSPlayerController* _Controller);

private:
	
	static FActorSpawnParameters GetDefaultSpawnParams();
};
