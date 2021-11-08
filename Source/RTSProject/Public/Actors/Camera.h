#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UFloatingPawnMovement;
class ARTSPlayerController;

UCLASS()
class RTSPROJECT_API ACamera : public APawn
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UStaticMeshComponent* Sphere = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UFloatingPawnMovement* FloatingMovement = nullptr;
	UPROPERTY(BlueprintReadOnly)
	ARTSPlayerController* PlayerController = nullptr;
	
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
	bool bDisablePanRotation = false;

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
	
public:
	
	ACamera();
	void Initialize(ARTSPlayerController* RTSController);

	virtual void Tick(float DeltaTime) override;

	void MoveForward(float value);
	void MoveRight(float value);
	void Move(const FVector& InputVector);
	void MovementIncrease();
	void MovementDecrease();
	void ResetMovementModifier();

	// Edge scrolling
	void EdgeScrolling();
	void EdgeScrollingX(float MouseX);
	void EdgeScrollingY(float MouseY);
	
	void MouseWheelYPositiveStart();
	void MouseWheelYNegativeStart();
	void ZoomIn() const;
	void ZoomOut() const;
	void ZoomReset();
	void RotatePan(float x, float y);
	void RotatePanX(float value);
	void RotatePanY(float value);
	void PanReset();
	void EnableCameraMovement();
	void DisableCameraMovement();

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

