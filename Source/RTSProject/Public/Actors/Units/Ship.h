#pragma once

#include "Actors/Units/BaseUnitWithAbility.h"
#include "Components/Movement/ShipMovementComponent.h"
#include "Interfaces/AttackInterface.h"
#include "Ship.generated.h"

class ARTSPlayerState;
class UMiniMapIconComponent;
class UFogOfWarInfluencerComponent;
class UShipMovementComponent;
class UStaticMeshComponent;
class UPawnSensingComponent;
class ARTSPlayerController;
class ATurret;
class UUserWidget;
class UHealthShieldWidget;
class UAttackComponent;
class UMiniMapInfluencerComponent;
class UAbilitySystemComponent;
class UShipAttributeSet;
class UAttributeSet;
class UHealthShieldAttributeSet;
class UAttackAbility;

UCLASS()
class RTSPROJECT_API AShip
	: public ABaseUnitWithAbility,
	  public IAttackInterface
{
	GENERATED_BODY()

public:
	AShip(const FObjectInitializer& ObjectInitializer);
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual UAttackComponent* GetAttackComponent() const override { return AttackComponent; }
	UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }
	UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMesh; }
	virtual UPawnMovementComponent* GetMovementComponent() const override { return MovementComponent; }

	// Begin ISelectable override
	virtual void Selected_Implementation(bool bInIsSelected) override;
	virtual void Highlighted_Implementation(bool bInIsHighlighted) override;
	// End ISelectable override
	
	UHealthShieldAttributeSet* GetHealthShieldAttributeSet() const { return HealthShieldAttributeSet; }
	virtual TArray<ATurret*>& GetTurrets() override { return Turrets; }
	ARTSPlayerController* GetPlayerController() const { return PlayerController; }
	bool GetHasWorkingTurrets() const { return bHasWorkingTurrets; }
	void SetHasWorkingTurrets( bool NewHasWorkingTurrets) { bHasWorkingTurrets = NewHasWorkingTurrets; }
	
	void UpdatePositionWhenCreated();

	// Mouse wheel
	void MouseYPositiveStart();
	void MouseYNegativeStart();

	UFUNCTION()
	void LMBPressed(ETriggerEvent Event);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UCapsuleComponent* CapsuleComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship")
	UStaticMeshComponent* SelectionCircle = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship")
	UPawnSensingComponent* PawnSensing = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship")
	UPawnMovementComponent* MovementComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAttackComponent* AttackComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FOW")
	UFogOfWarInfluencerComponent* FOWInfluencerComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	UMiniMapInfluencerComponent* MiniMapInfluencerComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	UMiniMapIconComponent* MiniMapIconComponent = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	UInputComponent* DebugInputComponent = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	float TimeToBuild = 5.0;

	// Turrets
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret")
	bool bHasWorkingTurrets = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret")
	TArray<ATurret*> Turrets;
};
