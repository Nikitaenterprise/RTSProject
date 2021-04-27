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

	// Key press
	bool bShiftPressed = false;
	
	// Mouse clicks
	bool bLMBPressed = false;
	bool bRMBPressed = false;

	// Mouse wheel
	bool bDisableZooming = false;


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

	// Units selection and control
	void UpdateSelection();
	void HighlightActorsUnderCursor();
	void MoveSelectedActors();
	bool ShipsSelected();
	bool BuildingsSelected();

	// FactoryAssets
	UFactoryAssets* GetFactoryAssets();

protected:

	virtual void BeginPlay() override;
};
