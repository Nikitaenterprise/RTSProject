#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UFloatingPawnMovement;

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

	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefaultMovementSpeed = 15;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeedModifier = 1;
	// Pan rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PanRotationSpeed = 5;
	// Zoom
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraZoomSpeed = 150;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraMaxZoom = 4000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraMinZoom = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraDefaultZoom = 4000;
	
public:
	
	ACamera();

	virtual void Tick(float DeltaTime) override;

	void Move(const FVector& InputVector);
	void EdgeScrolling(float dx, float dy);
	void ZoomIn() const;
	void ZoomOut() const;
	void ZoomReset() const;
	void RotatePan(float x, float y);
	void PanReset();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

