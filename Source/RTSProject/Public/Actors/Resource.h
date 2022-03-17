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
	None UMETA(DisplayName = "None"),
	
	Num // Total
};

UCLASS()
class RTSPROJECT_API AResource : public AActor
{
	GENERATED_BODY()
protected:
	
	UPROPERTY(EditAnywhere, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = "GAS")
	UResourceSourceAttributeSet* ResourceSourceAttributeSet = nullptr;
	UPROPERTY()
	AResourceManager* ResourceManager = nullptr;
	EResourceType ResourceType;

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
	void CheckCapacity(const FOnAttributeChangeData& Data);
	virtual float InitialCapacity();
	virtual void CalculateResource(TFunction<float()> Func);
};