#pragma once

#include "GameFramework/Actor.h"
#include "ResourceManager.generated.h"

class AResource;

UCLASS()
class RTSPROJECT_API AResourceManager : public AActor
{
	GENERATED_BODY()

public:
	AResourceManager();
	
	UFUNCTION(BlueprintCallable)
	void AddResource(AResource* ResourceToAdd);

	UFUNCTION(BlueprintCallable)
	void RemoveResource(AResource* ResourceToRemove);

	UFUNCTION(BlueprintCallable)
	AResource* GetClosestResource(const FVector& Position);
	
protected:
	UPROPERTY()
	TArray<AResource*> ResourcesOnMap;	
};
