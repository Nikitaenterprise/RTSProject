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
	UStaticMeshComponent* StaticMeshComponent = nullptr;
	UPROPERTY(EditAnywhere)
	float SizeModifier = 1.0;
	UPROPERTY(EditAnywhere)
	float RotationSpeed = 0.0;
	bool bIsInAsteroidField = false;
	
public:
	
	AAsteroidResource();
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }
	void SetStaticMeshComponent(UStaticMeshComponent* NewStaticMeshComponent) { StaticMeshComponent = NewStaticMeshComponent; }
	
	float GetRotationSpeed() const { return RotationSpeed; }
	void SetRotationSpeed(float NewRotationSpeed) { RotationSpeed = NewRotationSpeed; }
	bool GetIsInAsteroidField() const { return bIsInAsteroidField; }
	void SetIsInAsteroidField(bool bNewIsInAsteroidField) { bIsInAsteroidField = bNewIsInAsteroidField; }

protected:
	
	virtual void BeginPlay() override;
	virtual void CheckCapacity(const FOnAttributeChangeData& Data) override;
	virtual float SetupInitialCapacity() override;
	
};
