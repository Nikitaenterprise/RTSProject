#pragma once

#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "Resource.generated.h"

class UStaticMeshComponent;
class UResourceSourceAttributeSet;
class UAbilitySystemComponent;
class AResourceManager;

UENUM()
enum class EResourceType : uint8
{
	Asteroid UMETA(DisplayName = "Asteroid"),
	Star UMETA(DisplayName = "Star"),
	None UMETA(DisplayName = "None"),
	
	Num // Total
};

UCLASS()
class RTSPROJECT_API AResource : public AActor
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere, Category = "Base")
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = "GAS")
	UResourceSourceAttributeSet* ResourceSourceAttributeSet = nullptr;
	UPROPERTY(EditAnywhere)
	float InitialCapacity = 0;
	UPROPERTY()
	AResourceManager* ResourceManager = nullptr;
	EResourceType ResourceType;
	FDelegateHandle ResourceCapacityDelegateHandle;

public:

	AResource();
	
	UFUNCTION(BlueprintCallable)
	UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	void SetAbilitySystemComponent(UAbilitySystemComponent* NewAbilitySystemComponent) { AbilitySystemComponent = NewAbilitySystemComponent; }
	UFUNCTION(BlueprintCallable)
	UResourceSourceAttributeSet* GetResourceSourceAttributeSet() const { return ResourceSourceAttributeSet; }
	UFUNCTION(BlueprintCallable)
	EResourceType GetResourceType() const { return ResourceType; }
	
protected:
	
	virtual void BeginPlay() override;
	virtual void CheckCapacity(const FOnAttributeChangeData& Data);
	virtual float SetupInitialCapacity();
	virtual void CalculateResource(TFunction<float()> Func);
};