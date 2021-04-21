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

	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefaultMovementSpeed = 15;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeedModifier = 1;

	// Zoom
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraZoomSpeed = 150;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraMaxZoom = 4000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraMinZoom = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraDefaultZoom = 4000;

	// Pan rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDisablePanRotation = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PanRotationSpeed = 5;

	// Mouse clicks
	bool bLMBPressed = false;
	bool bRMBPressed = false;

	// Mouse wheel
	bool bDisableZooming = false;
	bool bMouseWheelYPositive = false;
	bool bMouseWheelYNegative = false;

	// Units selection and control
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
	void Move(FVector& v);
	void MoveForward(float value);
	void MoveRight(float value);

	void MovementIncrease();
	void MovementDecrease();
	void ResetMovementModifier();

	void MouseYPositiveStart();
	void MouseYPositiveEnd();
	void MouseYNegativeStart();
	void MouseYNegativeEnd();
	
	// Zoom
	void ZoomIn();
	void ZoomOut();
	void ZoomReset();

	// Edge scrolling
	void EdgeScrollingX(float value);
	void EdgeScrollingY(float value);
	void EdgeScrolling(float dx, float dy);

	// Pan rotation
	void RotatePanX(float value);
	void RotatePanY(float value);
	void RotatePan(float x, float y);
	void PanReset();
	void EnableCameraMovement();
	void DisableCameraMovement();

	// Mouse clicks
	void LMBPressed();
	void LMBReleased();
	void RMBPressed();
	void RMBReleased();

	// Units selection and control
	UFUNCTION(BlueprintCallable, Category = "Selection")
	bool SelectedShips();
	UFUNCTION(BlueprintCallable, Category = "Selection")
	bool SelectedBuildings();
	void SelectUnits();
	void DeselectUnits();
	void HighlightUnit();
	void DehighlightUnit();
	void UpdateSelection();
	void HighlightActorsUnderCursor();
	void MoveSelectedActors();

	// FactoryAssets
	UFactoryAssets* GetFactoryAssets();

protected:

	virtual void BeginPlay() override;
};
