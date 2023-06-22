#pragma once

#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "Actors/Units/BaseUnit.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Selectable.h"
#include "Particles/ParticleSystemComponent.h"
#include "Building.generated.h"

class UMiniMapIconComponent;
class UMiniMapInfluencerComponent;
class UFogOfWarInfluencerComponent;
class UHealthShieldWidgetComponent;
class UHealthShield;
class ARTSPlayerController;
class UHealthShieldWidget;
class UAbilitySystemComponent;
class UBuildingAttributeSet;
class UHealthShieldAttributeSet;

UCLASS()
class RTSPROJECT_API ABuilding
	: public ABaseUnit,
	  public IAbilitySystemInterface
{
	GENERATED_BODY()

public:	
	ABuilding(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	UBuildingAttributeSet* GetBuildingAttributeSet() const { return BuildingAttributeSet; }
	ARTSPlayerController* GetPlayerController() const { return PlayerController; }
	TArray<TSubclassOf<AActor>>* GetBuildingQueue() { return &BuildingQueue; }
	FVector GetSpawnPointLocation() const { return SpawnPoint->GetComponentLocation(); }
	void SetSpawnPointLocation(const FVector& Location) const { SpawnPoint->SetWorldLocation(Location); }

	UFUNCTION(BlueprintCallable)
	void SetIsBuildingUnit(bool NewIsBuildingUnit) { bIsBuildingUnit = NewIsBuildingUnit;}
	
	// Begin ISelectable override
	virtual void Selected_Implementation(bool bInIsSelected) override;
	virtual void Highlighted_Implementation(bool bInIsHighlighted) override;
	// End ISelectable override
	
	void UpdatePositionWhenCreated();
	UFUNCTION(BlueprintCallable, Category = "Building")
	int GetBuildingQueueSizeByClass(TSubclassOf<AActor> ActorClass) const;
	UFUNCTION(BlueprintCallable, Category = "Building")
	void RequestBuildingUnit(TSubclassOf<AActor> ActorClass);
	
protected:
	void LMBPressed();
	void LMBReleased();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UHealthShieldWidgetComponent* HealthShieldWidgetComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UParticleSystemComponent* SpawnPoint = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FOW")
	UFogOfWarInfluencerComponent* FOWInfluencerComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMiniMapInfluencerComponent* MiniMapInfluencerComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMiniMapIconComponent* MiniMapIconComponent = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	UInputComponent* DebugInputComponent = nullptr;
	
	bool bLMBPressed = false;

	FVector LocationToSpawnOutsideTheBorders = FVector(0, 0, -10000);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayAbility> BuildUnitAbility;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	UBuildingAttributeSet* BuildingAttributeSet = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	UHealthShieldAttributeSet* HealthShieldAttributeSet = nullptr;
	TArray<TSubclassOf<AActor>> BuildingQueue;
	FGameplayAbilitySpecHandle BuildingUnitHandle;
	bool bIsBuildingUnit = false;
};
