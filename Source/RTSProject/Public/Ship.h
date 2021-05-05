#pragma once

#include "BaseBehavior.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Ship.generated.h"

class UShipMovementComponent;
class UStaticMeshComponent;
class UWidgetComponent;
class UHealthShield;
class UPawnSensingComponent;
class ARTSPlayerController;
class ATurret;
class UUserWidget;
class UHealthShieldBarHUD;

UCLASS()
class RTSPROJECT_API AShip : public APawn, public IBaseBehavior
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UCapsuleComponent* CapsuleComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UHealthShield* HealthShieldComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship")
	UStaticMeshComponent* SelectionCircle = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	UWidgetComponent* HealthShieldBar = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship")
	UPawnSensingComponent* PawnSensing = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship")
	UShipMovementComponent* MovementComponent = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Base")
	ARTSPlayerController* PlayerController = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	UInputComponent* DebugInputComponent = nullptr;
	
	// Widget
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UHealthShieldBarHUD* HealthShieldBarHUD = nullptr;

	UPROPERTY(BlueprintReadOnly)
	bool bJustCreated = false;
	UPROPERTY(BlueprintReadOnly)
	bool bIsSelected = false;
	UPROPERTY(BlueprintReadOnly)
	bool bIsHighlighted = false;

	UPROPERTY(BlueprintReadOnly)
	float DeltaTime = 0;
	UPROPERTY(BlueprintReadOnly)
	float PastTime = 0;
	UPROPERTY(BlueprintReadWrite)
	float TimeToBuild = 5.0;

	// Moving
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Moving")
	bool bIsMoving = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Moving")
	bool bShouldMove = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Moving")
	float ForwardSpeed = 500;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Moving")
	float TurnAngleSpeed = 5;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Moving")
	float TurnForwardSpeed = ForwardSpeed * 0.3;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Moving")
	float DrawNavLineOncePerThisSeconds = 1;
	UPROPERTY(BlueprintReadOnly, Category = "Moving")
	TArray<FVector> NavPathCoords;

	// Mouse wheel
	bool bMouseWheelYPositive = false;
	bool bMouseWheelYNegative = false;

	// Turrets
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret")
	bool bHasWorkingTurrets = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret")
	TArray<ATurret*> Turrets;

public:

	AShip(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float _mainDeltaTime) override;
	
	void Initialize(ARTSPlayerController* RTSController);

	// Interfaces
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	bool Destroy(bool bNetForce = false, bool bShouldModifyLevel = false);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Selected(bool _bIsSelected);
	virtual void Selected_Implementation(bool _bIsSelected) override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Highlighted(bool _bIsHighlighted);
	virtual void Highlighted_Implementation(bool _bIsHighlighted) override;

	// Moving
	UFUNCTION(BlueprintCallable, Category = "Moving")
	bool Move(const FVector _TargetLocation);

	void DrawNavLine();
	void UpdatePositionWhenCreated();

	// Mouse wheel
	void RotateWhenCreatedPositive();
	void RotateWhenCreatedNegative();
	void MouseYPositiveStart();
	void MouseYPositiveEnd();
	void MouseYNegativeStart();
	void MouseYNegativeEnd();
	
protected:
	
	virtual void BeginPlay() override;

};
