#pragma once

#include "Components/ResourceComponent.h"
#include "Interfaces/Selectable.h"
#include "AsteroidField.generated.h"

class AAsteroidResource;
class ARTSPlayerController;
class UBoxComponent;
class UWidgetComponent;

UCLASS()
class RTSPROJECT_API AAsteroidField : public AActor, public ISelectable
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere)
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UBoxComponent* BoxCollision = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	ARTSPlayerController* PlayerController = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UWidgetComponent* WidgetComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;
	UPROPERTY(EditAnywhere)
	UResourceComponent* ResourceComponent = nullptr;
	TArray<AAsteroidResource*> Asteroids;
	
	
public:	
	
	AAsteroidField();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	void SetAbilitySystemComponent(UAbilitySystemComponent* NewAbilitySystemComponent) { AbilitySystemComponent = NewAbilitySystemComponent; }
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Selected(bool bIsSelected);
	virtual void Selected_Implementation(bool bIsSelected) override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Highlighted(bool bIsHighlighted);
	virtual void Highlighted_Implementation(bool bIsHighlighted) override;
	
	UFUNCTION(BlueprintCallable)
	int32 GetNumberOfAsteroidsInField();
	UFUNCTION(BlueprintCallable)
	void AddRandomNumberOfAsteroidsToField(int MinValue = 5, int MaxValue = 20);
	UFUNCTION(BlueprintCallable)
	void CreateAsteroidAndAddItToField();
	UFUNCTION(BlueprintCallable)
	void AddAsteroidToField(AAsteroidResource* AsteroidToAdd);
	UFUNCTION(BlueprintCallable)
	void RemoveRandomAsteroidFromField();
	UFUNCTION(BlueprintCallable)
	void RemoveAsteroidFromField(AAsteroidResource* Asteroid);
	UFUNCTION(BlueprintCallable)
	TArray<AAsteroidResource*>& GetAsteroids() { return Asteroids; }
	
};