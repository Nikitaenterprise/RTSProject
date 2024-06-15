#pragma once

#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Selectable.h"
#include "Resource.generated.h"

class AResourceManager;
class UResourceAttributeSet;
class UWidgetComponent;

UENUM()
enum class EResourceType : uint8
{
	AsteroidField UMETA(DisplayName = "AsteroidField"),
	Star UMETA(DisplayName = "Star"),
	None UMETA(DisplayName = "None"),
	
	Num // Total
};

UCLASS()
class RTSPROJECT_API AResource
	: public AActor,
	  public ISelectable,
	  public IAbilitySystemInterface 
{
	GENERATED_BODY()

public:
	AResource();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Selectable")
	void Selected(bool bIsSelected);
	virtual void Selected_Implementation(bool bIsSelected) override {};
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Selectable")
	void Highlighted(bool bIsHighlighted);
	virtual void Highlighted_Implementation(bool bIsHighlighted) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnResourceEmpty();
	void SetResourceAmount() const;

	UFUNCTION(BlueprintNativeEvent)
	float GetResourceAmount() const;

	UPROPERTY(EditAnywhere)
	USceneComponent* SceneComponent {nullptr};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent {nullptr};

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UResourceAttributeSet> ResourceAttribute;
	
	UPROPERTY()
	const UResourceAttributeSet* ResourceAttributeSet {nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource")
	UWidgetComponent* WidgetComponent {nullptr};

	TDelegate<void()> OnEmptyResourceDelegate;

	UPROPERTY(EditAnywhere)
	EResourceType ResourceType;

	UPROPERTY()
	AResourceManager* ResourceManager {nullptr};
};
