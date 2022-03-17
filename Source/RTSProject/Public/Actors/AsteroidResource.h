#pragma once

#include "Resource.h"
#include "AsteroidResource.generated.h"

class AAsteroidField;

UCLASS()
class RTSPROJECT_API AAsteroidResource : public AResource
{
	GENERATED_BODY()
public:
	
protected:

	UPROPERTY(EditAnywhere, Category = "Base")
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = "Base")
	UStaticMeshComponent* StaticMeshComponent = nullptr;
	float Omega = 0.0;
	bool bIsInAsteroidField = false;
	
public:
	
	AAsteroidResource();
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }
	void SetStaticMeshComponent(UStaticMeshComponent* NewStaticMeshComponent) { StaticMeshComponent = NewStaticMeshComponent; }
	
	void IsEmpty();
	float GetOmega() const { return Omega; }
	void SetOmega(float NewOmega) { Omega = NewOmega; }
	bool GetIsInAsteroidField() const { return bIsInAsteroidField; }
	void SetIsInAsteroidField(bool bNewIsInAsteroidField) { bIsInAsteroidField = bNewIsInAsteroidField; }

protected:
	
	virtual void BeginPlay() override;
	virtual float InitialCapacity() override;
	
};
