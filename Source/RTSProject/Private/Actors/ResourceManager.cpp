#include "Actors/ResourceManager.h"

AResourceManager::AResourceManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AResourceManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AResourceManager::AddAsteroidField(AAsteroidField* AsteroidFieldToAdd)
{
	AsteroidFieldsOnMap.Add(GetActorLocation() + AsteroidFieldToAdd->GetActorLocation(), AsteroidFieldToAdd);
}

void AResourceManager::AddResource(AResource* ResourceToAdd)
{
	ResourcesOnMap.Add(GetActorLocation() + ResourceToAdd->GetActorLocation(), ResourceToAdd);
}

AAsteroidField* AResourceManager::GetClosestAsteroidField(const FVector& Position)
{
	return *AsteroidFieldsOnMap.Find(GetActorLocation() - Position);
}

AResource* AResourceManager::GetClosestResource(const FVector& Position)
{
	return *ResourcesOnMap.Find(GetActorLocation() - Position);
}
