#include "Actors/Resources/ResourceManager.h"

#include "Actors/Resources/Resource.h"

AResourceManager::AResourceManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AResourceManager::AddResource(AResource* ResourceToAdd)
{
	ResourcesOnMap.AddUnique(ResourceToAdd);
}

void AResourceManager::RemoveResource(AResource* ResourceToRemove)
{
	const int32 Num = ResourcesOnMap.Remove(ResourceToRemove);
	if (Num == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't remove resource from manager"));
	}
}

AResource* AResourceManager::GetClosestResource(const FVector& Position)
{
	float ClosestDistance = 0;
	AResource* ClosestResource {nullptr};
	for (const auto& Field : ResourcesOnMap)
	{
		const float NewClosestDistance = FVector::DistSquared(Position, Field->GetActorLocation());
		if (ClosestDistance == 0 || NewClosestDistance < ClosestDistance)
		{
			ClosestDistance = NewClosestDistance;
			ClosestResource = Field;
		}
	}
	return ClosestResource;
}
