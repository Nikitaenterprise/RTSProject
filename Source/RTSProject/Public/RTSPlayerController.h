#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSPlayerController.generated.h"

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

	virtual void Tick(float mainDeltaTime) override;

	virtual void SetupInputComponent() override; //(class UInputComponent* PlayerInputComponent)

	void ShiftPressed();
	void ShiftReleased();

	// Edge scrolling
	void EdgeScrollingX(float value);
	void EdgeScrollingY(float value);

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

	void MoveSelectedShips();
	void SetSpawnPointForSelectedBuildings();
	
	// FactoryAssets	
	UFUNCTION(BlueprintCallable, Category = "Factory")
	UFactoryAssets* GetFactoryAssets();

	float GetScaleValueFromSettings();

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
