#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AsteroidField.generated.h"

class AAsteroidResource;
class ARTSPlayerController;
class UBoxComponent;

UCLASS()
class RTSPROJECT_API AAsteroidField : public AActor
{
	GENERATED_BODY()

	//friend class AsteroidFieldFactory;
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UBoxComponent* BoxCollision = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	ARTSPlayerController* PlayerController = nullptr;

	int NumberOfAsteroids = 0;
	int TotalResource = 0;
	TArray<AAsteroidResource*> Asteroids;
	
public:	
	
	AAsteroidField();
	AAsteroidField(FTransform Transform);
	virtual void BeginPlay() override;
	virtual void Tick(float MainDeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	int RecalculateNumberOfAsteroidsInField();
	void AddRandomNumberOfAsteroidsToField(int MinValue = 5, int MaxValue = 20);
	void AddAsteroidToField();

	void RemoveRandomAsteroidFromField();
	void RemoveAsteroidFromField(AAsteroidResource* Asteroid);
	
};
