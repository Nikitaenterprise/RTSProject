#pragma once

#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Movement/ShipMovementComponent.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "Interfaces/AttackInterface.h"
#include "Interfaces/Selectable.h"
#include "Ship.generated.h"

class ARTSPlayerState;
class UMiniMapIconComponent;
class UFogOfWarInfluencerComponent;
class UShipMovementComponent;
class UStaticMeshComponent;
class UHealthShieldWidgetComponent;
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
	: public APawn,
	  public ISelectable,
	  public IAbilitySystemInterface,
	  public IAttackInterface,
	  public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AShip(const FObjectInitializer& ObjectInitializer);
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual UAttackComponent* GetAttackComponent() const override { return AttackComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }
	UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMesh; }
	virtual UPawnMovementComponent* GetMovementComponent() const override { return MovementComponent; }

	// Begin ISelectable override
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Selected(bool _bIsSelected);
	virtual void Selected_Implementation(bool _bIsSelected) override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Highlighted(bool _bIsHighlighted);
	virtual void Highlighted_Implementation(bool _bIsHighlighted) override;
	// End ISelectable override

	// Begin IGenericTeamAgentInterface override
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override { TeamId = InTeamID; }
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	// End IGenericTeamAgentInterface override
	
	UHealthShieldAttributeSet* GetHealthShieldAttributeSet() const { return HealthShieldAttributeSet; }
	TArray<UAttributeSet*> GetAdditionalAttributeSets() const { return AdditionalAttributeSets; }
	virtual TArray<ATurret*>& GetTurrets() override { return Turrets; }
	ARTSPlayerController* GetPlayerController() const { return PlayerController; }
	bool GetHasWorkingTurrets() const { return bHasWorkingTurrets; }
	void SetHasWorkingTurrets( bool NewHasWorkingTurrets) { bHasWorkingTurrets = NewHasWorkingTurrets; }
	bool GetIsSelected() const { return bIsSelected; }
	void SetJustCreated(bool NewJustCreated) { bJustCreated = NewJustCreated; }
	void SetPlayerController(ARTSPlayerController* InPlayerController) { PlayerController = InPlayerController; }
	
	// Moving
	UFUNCTION(BlueprintCallable, Category = "Moving")
	bool RequestMove(const FVector TargetLocation);

	void DrawNavLine();
	void UpdatePositionWhenCreated();

	// Mouse wheel
	void RotateWhenCreatedPositive();
	void RotateWhenCreatedNegative();
	void MouseYPositiveStart();
	void MouseYPositiveEnd();
	void MouseYNegativeStart();
	void MouseYNegativeEnd();

	void LMBPressed();
	void LMBReleased();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UCapsuleComponent* CapsuleComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship")
	UStaticMeshComponent* SelectionCircle = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	UHealthShieldWidgetComponent* HealthShieldWidgetComponent = nullptr;
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UAttackAbility> AttackAbility;
	FGameplayAbilitySpecHandle AttackAbilityHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	UShipAttributeSet* ShipAttributeSet = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	UHealthShieldAttributeSet* HealthShieldAttributeSet = nullptr;
	UPROPERTY(EditAnywhere, Category = "GAS")
	TArray<UAttributeSet*> AdditionalAttributeSets;
	
	UPROPERTY(BlueprintReadOnly, Category = "Base")
	ARTSPlayerController* PlayerController = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Base")
	ARTSPlayerState* RTSPlayerState = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	UInputComponent* DebugInputComponent = nullptr;

	UPROPERTY(BlueprintReadOnly)
	bool bJustCreated = false;
	UPROPERTY(BlueprintReadOnly)
	bool bIsSelected = false;
	UPROPERTY(BlueprintReadOnly)
	bool bIsHighlighted = false;

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

	bool bLMBPressed = false;

	FGenericTeamId TeamId;

	// Turrets
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret")
	bool bHasWorkingTurrets = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret")
	TArray<ATurret*> Turrets;
};
