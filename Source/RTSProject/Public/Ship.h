#pragma once

#include "BaseBehavior.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Ship.generated.h"

class UStaticMeshComponent;
class UWidgetComponent;
class UHealthShield;
class UPawnSensingComponent;
class ARTSPlayerController;
class ATurret;
class UUserWidget;
class UHealthShieldBarHUD;

UCLASS()
class RTSPROJECT_API AShip : public ACharacter, public IBaseBehavior
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UHealthShield* HealthShieldComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship")
	UStaticMeshComponent* SelectionCircle = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	UWidgetComponent* HealthShieldBar = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship")
	UPawnSensingComponent* PawnSensing = nullptr;

	
	ARTSPlayerController* PlayerController = nullptr;
	
	// Widget
	UHealthShieldBarHUD* HealthShieldBarHUD = nullptr;

	bool bIsSelected = false;
	bool bIsHighlighted = false;
	
	float DeltaTime = 0;
	float PastTime = 0;

	// Moving
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
	float DrawNavLineOncePerThisSeconds = 0.2;
	TArray<FVector> NavPathCoords;

	// Turrets
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret")
	bool bHasWorkingTurrets = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret")
	TArray<ATurret*> Turrets;

public:

	AShip();

	virtual void Tick(float _mainDeltaTime) override;
	
	void BindHUD();
	void BindController(ARTSPlayerController* _Controller);

	void SetHealthShieldBar();

	// Interfaces
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	bool Destroy(bool bNetForce = false, bool bShouldModifyLevel = false);
	virtual bool Destroy_Implementation(bool bNetForce = false, bool bShouldModifyLevel = false) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Selected(bool _bIsSelected);
	virtual void Selected_Implementation(bool _bIsSelected) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Highlighted(bool _bIsHighlighted);
	virtual void Highlighted_Implementation(bool _bIsHighlighted) override;

	// Moving
	UFUNCTION(BlueprintCallable, Category = "Moving")
	bool SimpleMoving(const FVector& v);
	UFUNCTION(BlueprintCallable, Category = "Moving")
	bool Moving(const FVector& v);
	UFUNCTION(BlueprintCallable, Category = "Moving")
	bool StopMoving();
	UFUNCTION(BlueprintCallable, Category = "Moving")
	bool CustomMoving(const FVector& DestinationLocation);

	void DrawNavLine();
	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	
	virtual void BeginPlay() override;

};
