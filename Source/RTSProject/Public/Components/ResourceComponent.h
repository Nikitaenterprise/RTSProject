#pragma once

#include "GameplayEffectTypes.h"
#include "Components/ActorComponent.h"
#include "ResourceComponent.generated.h"

class UStaticMeshComponent;
class UResourceSourceAttributeSet;
class UAbilitySystemComponent;
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
protected:
	UPROPERTY()
	UResourceSourceAttributeSet* ResourceSourceAttributeSet = nullptr;
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent = nullptr; 
	UPROPERTY(EditAnywhere)
	float InitialCapacity = 0;
	UPROPERTY()
	AResourceManager* ResourceManager = nullptr;
	EResourceType ResourceType;
	FDelegateHandle ResourceCapacityDelegateHandle;
	bool bCapacityWasInitialized = false;
public:
	UResourceComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void InitializeCapacity(TFunction<float()> CalculationFunction);
	virtual void ModifyResource(TFunction<float()> CalculationFunction);
	
	UFUNCTION(BlueprintCallable)
	UResourceSourceAttributeSet* GetResourceSourceAttributeSet() const { return ResourceSourceAttributeSet; }
	UFUNCTION(BlueprintCallable)
	EResourceType GetResourceType() const { return ResourceType; }
	void SetResourceType(EResourceType NewResourceType) { ResourceType = NewResourceType; }
	void SetResourceCapacityDelegateHandle(FDelegateHandle NewHandle) { ResourceCapacityDelegateHandle = NewHandle; }
	UFUNCTION(BlueprintCallable)
	UAbilitySystemComponent* GetOwnerAbilitySystemComponent() const { return AbilitySystemComponent; }
};