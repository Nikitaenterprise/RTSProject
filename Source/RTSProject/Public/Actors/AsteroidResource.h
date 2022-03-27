#pragma once

#include "Components/ResourceComponent.h"
#include "MeshProcessingPlugin/Public/DynamicSDMCActor.h"
#include "AsteroidResource.generated.h"

class AAsteroidField;

UCLASS()
class RTSPROJECT_API AAsteroidResource : public ADynamicSDMCActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;
	UPROPERTY(EditAnywhere)
	UResourceComponent* ResourceComponent = nullptr;
	UPROPERTY(EditAnywhere)
	float InitialCapacityModifier = 1.0;

	UPROPERTY(EditAnywhere)
	bool bManualSetUpRotation = false;
	UPROPERTY(EditAnywhere)
	float RotationSpeed = 0.0;
	UPROPERTY(EditAnywhere)
	FRotator RandomRotator;

	UPROPERTY(EditAnywhere)
	bool bManualSetUpNumberOfCavities;
	UPROPERTY(EditAnywhere)
	int32 NumberOfCavities;
	bool bShouldDistortCavityWithSinWave = false;
	
	bool bIsInAsteroidField = false;
public:
	AAsteroidResource();
	virtual void PostLoad() override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	void SetAbilitySystemComponent(UAbilitySystemComponent* NewAbilitySystemComponent) { AbilitySystemComponent = NewAbilitySystemComponent; }
	UFUNCTION(BlueprintCallable)
	UResourceComponent* GetResourceComponent() const { return ResourceComponent; }
	
	float GetRotationSpeed() const { return RotationSpeed; }
	void SetRotationSpeed(float NewRotationSpeed) { RotationSpeed = NewRotationSpeed; }
	bool GetIsInAsteroidField() const { return bIsInAsteroidField; }
	void SetIsInAsteroidField(bool bNewIsInAsteroidField) { bIsInAsteroidField = bNewIsInAsteroidField; }
protected:
	virtual void BeginPlay() override;
	virtual void CheckCapacity(const FOnAttributeChangeData& Data);
};