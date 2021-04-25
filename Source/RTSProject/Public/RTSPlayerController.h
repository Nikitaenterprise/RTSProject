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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ACamera* CameraRef = nullptr;


	bool bDisablePanRotation = false;

	// Key press
	bool bShiftPressed = false;
	bool bAltPressed = false;
	
	// Mouse clicks
	bool bLMBPressed = false;
	bool bRMBPressed = false;

	// Mouse wheel
	bool bDisableZooming = false;
	bool bMouseWheelYPositive = false;
	bool bMouseWheelYNegative = false;

	// Units selection and control
	TArray<AActor*> ShouldBeSelected;
	TArray<AActor*> SelectedActors;
	TArray<AActor*> PlayersActors;
	AActor* HighlightedActor = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Factory")
	UFactoryAssets* FactoryAssets = nullptr;

	AGameHUD* GameHUD = nullptr;

public:

	ARTSPlayerController();

	void TestThis();

	virtual void Tick(float mainDeltaTime) override;

	virtual void SetupInputComponent() override; //(class UInputComponent* PlayerInputComponent)
	void MoveForward(float value);
	void MoveRight(float value);

	void MovementIncrease();
	void MovementDecrease();
	void ResetMovementModifier();

	void MouseYPositiveStart();
	void MouseYPositiveEnd();
	void MouseYNegativeStart();
	void MouseYNegativeEnd();

	// Edge scrolling
	void EdgeScrollingX(float value);
	void EdgeScrollingY(float value);

	// Pan rotation
	void RotatePanX(float value);
	void RotatePanY(float value);
	void PanReset();
	void EnableCameraMovement();
	void DisableCameraMovement();

	// Zoom
	void ZoomReset();
	
	// Mouse clicks
	void LMBPressed();
	void LMBReleased();
	void RMBPressed();
	void RMBReleased();

	// Units selection and control
	void UpdateSelection();
	void HighlightActorsUnderCursor();
	void MoveSelectedActors();

	// FactoryAssets
	UFactoryAssets* GetFactoryAssets();

protected:

	virtual void BeginPlay() override;
};
