#include "RocketFactory.h"

//#include "Engine/World.h"
#include "Rocket.h"

ARocket* RocketFactory::NewRocket(UWorld* World)
{
	return NewRocket(World, FTransform(FVector(0, 0, 0), FVector(0, 0, 0), FVector(0, 0, 0), FVector(0, 0, 0)));
}

ARocket* RocketFactory::NewRocket(UWorld* World, const FTransform& Transform)
{
	ARocket* SpawnedRocket = World->SpawnActor<ARocket>(ARocket::StaticClass(), Transform, GetDefaultSpawnParams());
	
	// If failed then spawn at 0,0,0
	if (!IsValid(SpawnedRocket))
	{
		// Could be problems with memory
		delete SpawnedRocket;
		return NewRocket(World);
	}
	return SpawnedRocket;
}

FActorSpawnParameters RocketFactory::GetDefaultSpawnParams()
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Instigator = nullptr;
	Params.Owner = nullptr;
	return Params;
}
