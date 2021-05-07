#pragma once

#include "BaseBehavior.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"

class UWidgetComponent;
class UHealthShield;
class ARTSPlayerController;
class UHealthShieldBarHUD;
class UFogOfWarComponent;

enum class EConstructionState
{
	NotConstructing,
	RequestedConstruction,
	Constructing,
	Finish
};

//
//template<class ActorType, class ... FactoryFunctionParameters>
//class TBuildingPair
//{
//	TBuildingPair(ActorType ActorTypeToSpawn, ActorType* (*FactoryFunctionToSpawn)(FactoryFunctionParameters))
//	{
//		ActorToSpawn = ActorTypeToSpawn;
//		Factory = FactoryFunctionToSpawn;
//	}
//	ActorType ActorToSpawn;
//	ActorType* (*Factory)(FactoryFunctionParameters);
//};

UCLASS()
class RTSPROJECT_API ABuilding : public AActor, public IBaseBehavior
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UWidgetComponent* HealthShieldBar = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UParticleSystemComponent* SpawnPoint = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	UHealthShield* HealthShieldComponent = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Base")
	ARTSPlayerController* PlayerController = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UHealthShieldBarHUD* HealthShieldBarHUD = nullptr;
	UPROPERTY(BlueprintReadOnly)
	UFogOfWarComponent* FogOfWarDispellerComponent = nullptr;

	float DeltaTime = 0;
	float PastTime = 0;

	bool bIsSelected = false;
	bool bIsHighlighted = false;
	bool bJustCreated = false;

	FVector LocationToSpawnOutsideTheBorders = FVector(0, 0, -10000);

private:
	
	EConstructionState ConstructionState = EConstructionState::NotConstructing;
	TArray<TSubclassOf<AActor>> BuildingQueue;
	FTimerHandle Timer;
	
	
public:	

	ABuilding();

	virtual void Tick(float MainDeltaTime) override;
	void Initialize(ARTSPlayerController* RTSController);
	
	void SetSpawnPointLocation(const FVector& Location = FVector(0, 0, 0)) const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	bool Destroy(bool bNetForce = false, bool bShouldModifyLevel = false);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Selected(bool _bIsSelected);
	virtual void Selected_Implementation(bool _bIsSelected) override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Highlighted(bool _bIsHighlighted);
	virtual void Highlighted_Implementation(bool _bIsHighlighted) override;

	void UpdatePositionWhenCreated();
	EConstructionState GetConstructionState() const { return ConstructionState; }
	UFUNCTION(BlueprintCallable, Category = "Building")
	int GetBuildingQueueSizeByClass(TSubclassOf<AActor> ActorClass) const;
	UFUNCTION(BlueprintCallable, Category = "Building")
	bool IsConstructing() const { return ConstructionState == EConstructionState::Constructing ? true : false; }
	UFUNCTION(BlueprintCallable, Category = "Building")
	void RequestBuildingUnit(TSubclassOf<AActor> ActorClass);

private:
	
	UFUNCTION(BlueprintCallable, Category = "Building")
	void StartBuildingUnit();
	UFUNCTION(BlueprintCallable, Category = "Building")
	void BuildUnit();
	UFUNCTION(BlueprintCallable, Category = "Building")
	void FinishBuildingUnit();

protected:

	virtual void BeginPlay() override;
};
