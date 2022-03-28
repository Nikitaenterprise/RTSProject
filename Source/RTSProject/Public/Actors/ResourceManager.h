#pragma once

#include "Actors/AsteroidField.h"
#include "GameFramework/Actor.h"
#include "ResourceManager.generated.h"

UCLASS()
class RTSPROJECT_API AResourceManager : public AActor
{
	GENERATED_BODY()
protected:

	UPROPERTY()
	TMap<FVector, AActor*> ResourcesOnMap;
	UPROPERTY()
	TMap<FVector, AAsteroidField*> AsteroidFieldsOnMap;
	
public:
	
	AResourceManager();
	UFUNCTION(BlueprintCallable)
	void AddAsteroidField(AAsteroidField* AsteroidFieldToAdd);
	UFUNCTION(BlueprintCallable)
	void RemoveAsteroidField(AAsteroidField* AsteroidFieldToRemove);
	UFUNCTION(BlueprintCallable)
	void AddResource(AActor* ResourceToAdd);
	UFUNCTION(BlueprintCallable)
	void RemoveResource(AActor* ResourceToRemove);
	UFUNCTION(BlueprintCallable)
	AAsteroidField* GetClosestAsteroidField(const FVector& Position);
	UFUNCTION(BlueprintCallable)
	AActor* GetClosestResource(const FVector& Position);
	
protected:
	
	virtual void BeginPlay() override;
	
};
