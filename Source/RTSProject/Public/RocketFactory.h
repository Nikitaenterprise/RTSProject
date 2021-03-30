#pragma once

#include "CoreMinimal.h"

class UWorld;
struct FActorSpawnParameters;
class ARocket;

class RTSPROJECT_API RocketFactory
{
public:
	static ARocket* NewRocket(UWorld* World);
	static ARocket* NewRocket(UWorld* World, const FTransform& Transform);
private:
	static FActorSpawnParameters GetDefaultSpawnParams();
};
