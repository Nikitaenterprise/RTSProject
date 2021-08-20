#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSPlayerController.generated.h"

class ABuilding;
class AShip;
class UInputController;
class ACamera;
class AGameHUD;
class UShipHUD;
class UBuildingHUD;
class UBasicButtonsHUD;
class UFactoryAssets;
class AFogOfWar;


UCLASS()
class RTSPROJECT_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	// Key press
	bool bShiftPressed = false;
	
	// Mouse clicks
	bool bLMBPressed = false;
	bool bRMBPressed = false;

	// Mouse wheel
	bool bDisableZooming = false;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	ACamera* CameraRef = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "FogOfWar")
	AFogOfWar* FogOfWar = nullptr;

	// Units selection
	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	TArray<AActor*> ShouldBeSelected;
	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	TArray<AActor*> SelectedActors;
	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	TArray<AActor*> PlayersActors;
	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	AActor* HighlightedActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Factory")
	UFactoryAssets* FactoryAssets = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	AGameHUD* GameHUD = nullptr;

public:

	ARTSPlayerController();

	void TestThis();
	bool FF() { return true; }
	virtual void Tick(float mainDeltaTime) override;

	virtual void SetupInputComponent() override; //(class UInputComponent* PlayerInputComponent)

	void ShiftPressed();
	void ShiftReleased();

	// Mouse clicks
	void LMBPressed();
	void LMBReleased();
	void RMBPressed();
	void RMBReleased();

	void DamagePressed();

	// Units selection
	void UpdateSelection();
	void HighlightActorsUnderCursor();
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
	bool ExecuteCommandToSelectedActors(bool(ARTSPlayerController::* FnCommand)(ActorType* Actor, FHitResult HitResult));

	
	// FactoryAssets	
	UFUNCTION(BlueprintCallable, Category = "Factory")
	UFactoryAssets* GetFactoryAssets() const { return FactoryAssets; }
	UFUNCTION(BlueprintCallable, Category = "FogOfWar")
	AFogOfWar* GetFOWManager() const { return FogOfWar; }
	
protected:

	virtual void BeginPlay() override;
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
bool ARTSPlayerController::ExecuteCommandToSelectedActors(bool(ARTSPlayerController::* FnCommand)(ActorType* Actor, FHitResult HitResult))
{
	if (SelectedActors.Num() == 0) return false;
	for (auto& Actor : SelectedActors)
	{
		ActorType* ThisActor = Cast<ActorType>(Actor);
		if (ThisActor)
		{
			FHitResult Hit;
			const bool bHit = GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Camera), false, Hit);
			if (bHit)
			{
				// Calling std::invoke to avoid this monstrosity: (this->*FnCommand)(Actor1, HitResult1);
				bool bSuccess = std::invoke(FnCommand, this, ThisActor, Hit);
			}
		}
	}
	return true;
}
