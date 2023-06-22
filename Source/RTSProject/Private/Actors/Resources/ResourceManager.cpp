#include "Actors/Resources/ResourceManager.h"

#include "Field/FieldSystem.h"

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
	AsteroidFieldsOnMap.Add(AsteroidFieldToAdd->GetActorLocation(), AsteroidFieldToAdd);
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
	ResourcesOnMap.Add(ResourceToAdd->GetActorLocation(), ResourceToAdd);
}

void AResourceManager::RemoveResource(AActor* ResourceToRemove)
{
	auto Num = AsteroidFieldsOnMap.Remove(ResourceToRemove->GetActorLocation());
	if (Num == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't remove resource from manager"));
	}
}

AAsteroidField* AResourceManager::GetClosestAsteroidField(const FVector& Position)
{
	float ClosestDistance = 0;
	AAsteroidField* ClosestField = nullptr;
	for (auto& Field : AsteroidFieldsOnMap)
	{
		const float NewClosestDistance = FVector::DistSquared(Position, Field.Key);
		if (ClosestDistance == 0 || NewClosestDistance < ClosestDistance)
		{
			ClosestDistance = NewClosestDistance;
			ClosestField = Field.Value;
		}
	}
	return ClosestField;
}

AActor* AResourceManager::GetClosestResource(const FVector& Position)
{
	return *ResourcesOnMap.Find(GetActorLocation() - Position);
}
