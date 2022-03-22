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

void AResourceManager::RemoveAsteroidField(AAsteroidField* AsteroidFieldToRemove)
{
	auto Num = AsteroidFieldsOnMap.Remove(GetActorLocation() + AsteroidFieldToRemove->GetActorLocation());
	if (Num == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't remove asteroid field from manager"));
	}
}

void AResourceManager::AddResource(AActor* ResourceToAdd)
{
	ResourcesOnMap.Add(GetActorLocation() + ResourceToAdd->GetActorLocation(), ResourceToAdd);
}

void AResourceManager::RemoveResource(AActor* ResourceToRemove)
{
	auto Num = AsteroidFieldsOnMap.Remove(GetActorLocation() + ResourceToRemove->GetActorLocation());
	if (Num == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't remove resource from manager"));
	}
}

AAsteroidField* AResourceManager::GetClosestAsteroidField(const FVector& Position)
{
	return *AsteroidFieldsOnMap.Find(GetActorLocation() - Position);
}

AActor* AResourceManager::GetClosestResource(const FVector& Position)
{
	return *ResourcesOnMap.Find(GetActorLocation() - Position);
}
