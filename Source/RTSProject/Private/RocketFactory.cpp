#include "RocketFactory.h"

#include "Rocket.h"
#include "Turret.h"

ARocket* RocketFactory::NewRocket(UWorld* World, UClass* ClassType, ARTSPlayerController* Controller, ATurret* Turret)
{
	return NewRocket(World, ClassType, Controller, Turret, FTransform());
}

ARocket* RocketFactory::NewRocket(UWorld* World, UClass* ClassType, ARTSPlayerController* Controller, ATurret* Turret, const FTransform& Transform)
{
	if (!Controller)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn rocket, Controller is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn rocket, Controller is nullptr"));
		return nullptr;
	}
	if (!Turret)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn rocket, Turret is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn rocket, Turret is nullptr"));
		return nullptr;
	}
	ARocket* SpawnedRocket = World->SpawnActor<ARocket>(ClassType, Transform, GetDefaultSpawnParams());
	if (!SpawnedRocket)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn rocket"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn rocket"));
		return nullptr;
	}
	SpawnedRocket->Initialize(Controller, Turret);
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
