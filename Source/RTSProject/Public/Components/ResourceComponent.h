#pragma once

#include "GameplayEffectTypes.h"
#include "Components/ActorComponent.h"
#include "ResourceComponent.generated.h"

class UStaticMeshComponent;
class UResourceSourceAttributeSet;
class AResourceManager;

UENUM()
enum class EResourceType : uint8
{
	Asteroid UMETA(DisplayName = "Asteroid"),
	AsteroidField UMETA(DisplayName = "AsteroidField"),
	Star UMETA(DisplayName = "Star"),
	None UMETA(DisplayName = "None"),
	
	Num // Total
};

UCLASS()
class RTSPROJECT_API UResourceComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UResourceComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void InitializeCapacity(const TFunction<float()>& CalculationFunction);
	virtual void ModifyResource(const TFunction<float()>& CalculationFunction);
	
	UFUNCTION(BlueprintCallable)
	UResourceSourceAttributeSet* GetResourceSourceAttributeSet() const { return ResourceSourceAttributeSet; }
	UFUNCTION(BlueprintCallable)
	EResourceType GetResourceType() const { return ResourceType; }
	void SetResourceType(EResourceType NewResourceType) { ResourceType = NewResourceType; }
	UFUNCTION(BlueprintCallable)
	UAbilitySystemComponent* GetOwnerAbilitySystemComponent() const;

	FOnGameplayAttributeValueChange& GetResourceCapacityDelegate() { return ResourceCapacityDelegate; }

protected:
	UPROPERTY()
	UResourceSourceAttributeSet* ResourceSourceAttributeSet { nullptr };
	UPROPERTY()
	AResourceManager* ResourceManager = nullptr;
	
	UPROPERTY(EditAnywhere)
	float InitialCapacity = 0;

	UPROPERTY(EditAnywhere)
	EResourceType ResourceType;
	
	FOnGameplayAttributeValueChange ResourceCapacityDelegate;
	bool bCapacityWasInitialized = false;
};