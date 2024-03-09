#pragma once

#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "RTSPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UFloatingPawnMovement;
class ARTSPlayerController;
class UPlayerInputDataAsset;

UCLASS()
class RTSPROJECT_API ARTSPlayer : public APawn
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftMouseButtonClicked);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRightMouseButtonClicked);

public:
	ARTSPlayer();

	void SetIsZoomDisabled(bool bInDisableZoom) { bDisableZooming = bInDisableZoom; } 
	bool GetIsZoomDisabled() const { return bDisableZooming; }

	FOnLeftMouseButtonClicked OnLeftMouseButtonClicked;
	FOnRightMouseButtonClicked OnRightMouseButtonClicked;
	
protected:
	virtual void BeginPlay() override;
	virtual void EnableInput(class APlayerController* PlayerController) override;

	void KeyboardMove(const FInputActionValue& InputActionValue);
	void Zoom(const FInputActionValue& InputActionValue);
	void ZoomReset(const FInputActionValue& InputActionValue);

	void Move(const FVector& InputVector);

	// Edge scrolling
	void EdgeScrolling();
	void EdgeScrollingX(float MouseX);
	void EdgeScrollingY(float MouseY);
	
	void RotatePanX(float value);
	void RotatePanY(float value);
	void RotatePan(float x, float y);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UStaticMeshComponent* Sphere { nullptr };
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm { nullptr };
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera { nullptr };
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UFloatingPawnMovement* FloatingMovement { nullptr };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UPlayerInputDataAsset* PlayerInputDataAsset { nullptr };

	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefaultMovementSpeed = 15;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeedModifier = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AllowEdgeScrolling = true;
	
	// Pan rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PanRotationSpeed = 5;
	
	bool bEnablePanRotation = false;

	// Zoom
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraZoomSpeed = 150;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraMaxZoom = 4000;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraMinZoom = 200;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraDefaultZoom = 4000;

	bool bDisableZooming = false;
};

