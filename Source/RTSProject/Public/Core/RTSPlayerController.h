#pragma once
#include "GameFramework/PlayerController.h"
#include "RTSPlayerController.generated.h"

class ABuilding;
class AShip;
class UInputController;
class AGameHUD;
class UShipHUD;
class UBuildingHUD;
class UBasicButtonsHUD;
class UFactoryAssets;
class UOrdersProcessor;
class AFogOfWar;


UCLASS()
class RTSPROJECT_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override; //(class UInputComponent* PlayerInputComponent)

	void AddToSelectedActors(AActor* ActorToAdd) { SelectedActors.AddUnique(ActorToAdd); }
	void RemoveFromSelectedActors(AActor* ActorToRemove) { SelectedActors.Remove(ActorToRemove); }
	const TArray<AActor*>& GetSelectedActors() const { return SelectedActors; }
	TArray<AActor*>& GetSelectedActorsRef() { return SelectedActors; }
	
	AGameHUD* GetGameHUD() const { return GameHUD; }
	AFogOfWar* GetFogOfWar() const { return FogOfWar; }
	
	// Mouse clicks
	void RMBReleased();

	void DamagePressed();
	
	template <class ActorType>
	bool IsArrayContainThisTypeActors(const TArray<AActor*>& Array);

	template <class ActorType>
	TArray<ActorType*> GetSelectedActorsByType();
	UFUNCTION(BlueprintCallable)
	TArray<ABuilding*> GetSelectedBuildings(); 
	UFUNCTION(BlueprintCallable)
	TArray<AShip*> GetSelectedShips(); 

	bool MoveSelectedActors(AShip* Ship, FHitResult HitResult);
	bool AttackBySelectedActors(AShip* Ship, FHitResult HitResult);
	bool SetSpawnPointForSelectedBuildings(ABuilding* Building,FHitResult HitResult);
	template <typename ActorType>
	bool ExecuteCommandToSelectedActors(TFunction<bool(ActorType* Actor, FHitResult HitResult)> Function);

	// FactoryAssets	
	UFUNCTION(BlueprintCallable, Category = "Getters")
	UFactoryAssets* GetFactoryAssets() const { return FactoryAssets; }
	UFUNCTION(BlueprintCallable, Category = "Getters")
	AFogOfWar* GetFOWManager() const { return FogOfWar; }
	
protected:
	// True if FogOfWar was placed on level in editor
	// Checked in GameMode class
	UPROPERTY(BlueprintReadOnly, Category = "FogOfWar")
	bool bIsFOWPlacedOnLevel {false};
	// True if FogOfWarBoundsVolume was placed on level in editor
	// Checked in GameMode class
	UPROPERTY(BlueprintReadOnly, Category = "FogOfWar")
	bool bIsFOWBoundsVolumePlacedOnLevel {false};
	UPROPERTY(BlueprintReadOnly, Category = "FogOfWar")
	AFogOfWar* FogOfWar {nullptr};

	// Units selection
	// Array of currently selected actors for this controller
	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	TArray<AActor*> SelectedActors;
	// Array of all created and owned by this controller actors
	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	TArray<AActor*> PlayersActors;

	UPROPERTY(Transient)
	UOrdersProcessor* OrdersProcessor {nullptr};

	UPROPERTY(BlueprintReadOnly, Category = "Factory")
	UFactoryAssets* FactoryAssets {nullptr};

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	AGameHUD* GameHUD {nullptr};
};

template <class ActorType>
bool ARTSPlayerController::IsArrayContainThisTypeActors(const TArray<AActor*>& Array)
{
	if (Array.Num() == 0) return false;
	for (const auto& e : Array)
	{
		ActorType* Actor = Cast<ActorType>(e);
		if (IsValid(Actor)) return true;
	}
	return false;
}

template <class ActorType>
TArray<ActorType*> ARTSPlayerController::GetSelectedActorsByType()
{
	TArray<ActorType*> Actors;
	for (auto& Actor : SelectedActors)
	{
		ActorType* TestActor = Cast<ActorType>(Actor);
		if (TestActor) Actors.AddUnique(TestActor);
	}
	return Actors;
}

template <typename ActorType>
bool ARTSPlayerController::ExecuteCommandToSelectedActors(TFunction<bool(ActorType* Actor, FHitResult HitResult)> Function)
{
	if (SelectedActors.Num() == 0)
	{
		return false;
	}
	for (auto& Actor : SelectedActors)
	{
		ActorType* ThisActor = Cast<ActorType>(Actor);
		if (ThisActor)
		{
			FHitResult Hit;
			const bool bHit = GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Camera), false, Hit);
			if (bHit)
			{
				bool bSuccess = Function(ThisActor, Hit);
			}
		}
	}
	return true;
}
