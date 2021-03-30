#pragma once

#include "CoreMinimal.h"

class UWorld;
class ARTSPlayerController;
class AAsteroidResource;
class AAsteroidField;
struct FActorSpawnParameters;

class RTSPROJECT_API AsteroidFieldFactory
{
public:
	
	static AAsteroidField* NewAsteroidField(UWorld* _World, ARTSPlayerController* _Controller);
	static AAsteroidField* NewAsteroidField(UWorld* _World, const FVector& _Location, ARTSPlayerController* _Controller);
	
private:
	
	static FActorSpawnParameters GetDefaultSpawnParams();
};
