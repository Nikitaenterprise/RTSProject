#pragma once

#include "Components/ResourceComponent.h"
#include "Interfaces/Selectable.h"
#include "AsteroidField.generated.h"

class AAsteroidResource;
class ARTSPlayerController;
class UBoxComponent;
class UWidgetComponent;
//class UAsteroidFieldMaker;

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
	//UPROPERTY()
	//UAsteroidFieldMaker* AsteroidFieldMaker = nullptr;
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

// DECLARE_DELEGATE(FAsteroidsReady)
//
// class ADynamicSDMCActor;
// class FDynamicMesh3;
// class FAsteroidMeshWorker;
//
// UCLASS()
// class RTSPROJECT_API UAsteroidFieldMaker : public UObject
// {
// 	GENERATED_BODY()
// protected:
// 	
// 	UPROPERTY()
// 	UWorld* World = nullptr;
// 	UPROPERTY()
// 	const AAsteroidField* Field = nullptr;
// 	TArray<AAsteroidResource*>* AsteroidsInFieldToEdit = nullptr;
// 	TArray<FAsteroidMeshWorker*> ThreadWorkers;
// 	uint32 NumberOfThreads = 0;
// 	uint32 NumberOfAsteroidsToSpawn = 1;
// 	TArray<TArray<ADynamicSDMCActor*>> SpawnedAsteroids2DArray;
// 	UPROPERTY()
// 	TArray<UMaterialInterface*> Materials;
// 	
// 	FTimerHandle CheckAllThreadsAreCompletedTimerHandle;
// 	FTimerDelegate CheckAllThreadsAreCompletedTimerDelegate;
// 	bool AreAllThreadsCompleted = false;
// 	TArray<bool> ThreadsCompleted;
// 	TArray<FTimerHandle> CheckThreadCompletionTimerHandles;
//
// 	UAsteroidFieldMaker();
// 	bool IsReady() const { return World && Field && AsteroidsInFieldToEdit; }
// 	void Start();
// 	UFUNCTION()
// 	void CheckAreAllThreadsCompleted();
// 	UFUNCTION()
// 	void CheckIsThreadCompleted(uint32 Index);
// 	void FinishUp();
// 	void CopyMesh(const AAsteroidResource* AsteroidResourceFrom, AAsteroidResource* AsteroidResourceTo);
// 	
// public:
// 	FAsteroidsReady OnAsteroidsModificationCompleted;
// 	
// 	static UAsteroidFieldMaker* AsteroidFieldMaker(UWorld* ThisWorld,
// 		const AAsteroidField* AsteroidField, TArray<AAsteroidResource*>& AsteroidsInField, uint32 MeshSpawningAmount);
// 	
// };