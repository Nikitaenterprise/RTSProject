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


UCLASS()
class RTSPROJECT_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	ACamera* CameraRef = nullptr;

	// Key press
	bool bShiftPressed = false;
	
	// Mouse clicks
	bool bLMBPressed = false;
	bool bRMBPressed = false;

	// Mouse wheel
	bool bDisableZooming = false;


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
	bool ShipsSelected();
	bool BuildingsSelected();

	void MoveSelectedShips();
	void SetSpawnPointForSelectedBuildings();
	
	// FactoryAssets	
	UFUNCTION(BlueprintCallable, Category = "Factory")
	UFactoryAssets* GetFactoryAssets();

protected:

	virtual void BeginPlay() override;
};
