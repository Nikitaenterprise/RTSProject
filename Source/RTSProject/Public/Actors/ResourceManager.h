#pragma once

#include "Actors/Resource.h"
#include "Actors/AsteroidField.h"
#include "GameFramework/Actor.h"
#include "ResourceManager.generated.h"

UCLASS()
class RTSPROJECT_API AResourceManager : public AActor
{
	GENERATED_BODY()
protected:

	UPROPERTY()
	TMap<FVector, AResource*> ResourcesOnMap;
	UPROPERTY()
	TMap<FVector, AAsteroidField*> AsteroidFieldsOnMap;
	
public:
	
	AResourceManager();
	UFUNCTION(BlueprintCallable)
	void AddAsteroidField(AAsteroidField* AsteroidFieldToAdd);
	UFUNCTION(BlueprintCallable)
	void AddResource(AResource* ResourceToAdd);
	UFUNCTION(BlueprintCallable)
	AAsteroidField* GetClosestAsteroidField(const FVector& Position);
	UFUNCTION(BlueprintCallable)
	AResource* GetClosestResource(const FVector& Position);
	
protected:
	
	virtual void BeginPlay() override;
	
};
