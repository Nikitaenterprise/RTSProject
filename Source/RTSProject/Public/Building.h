#pragma once

#include "BaseBehavior.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"

class UWidgetComponent;
class UHealthShield;
class ARTSPlayerController;
class UHealthShieldBarHUD;

enum class EConstructionState
{
	NotConstructing,
	Constructing,
	Finishing
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
	UPROPERTY()
	ARTSPlayerController* PlayerController = nullptr;
	UPROPERTY()
	UHealthShieldBarHUD* HealthShieldBarHUD = nullptr;

	float DeltaTime = 0;
	float PastTime = 0;

	bool bIsSelected = false;
	bool bIsHighlighted = false;
	bool bJustCreated = false;
	
	
	EConstructionState ConstructionState = EConstructionState::NotConstructing;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	TArray<TSubclassOf<AActor>> BuildingQueue;
	
	
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

	UFUNCTION(BlueprintCallable, Category = "Building")
	void BuildUnit();
	UFUNCTION(BlueprintCallable, Category = "Building")
	void AddActorToBuildingQueue(TSubclassOf<AActor> Actor);

protected:

	virtual void BeginPlay() override;
};
