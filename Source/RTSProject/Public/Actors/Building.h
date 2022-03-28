#pragma once

#include "AbilitySystemInterface.h"
#include "../../../../../Engine/Plugins/Media/BinkMedia/Source/SDK/include/binkplugin.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Selectable.h"
#include "Particles/ParticleSystemComponent.h"
#include "Building.generated.h"

class UMiniMapIconComponent;
class UMiniMapInfluencerComponent;
class UFogOfWarInfluencerComponent;
class UHealthShieldComponent;
class UWidgetComponent;
class UHealthShield;
class ARTSPlayerController;
class UHealthShieldBarHUD;
class UAbilitySystemComponent;
class UBuildingAttributeSet;

UCLASS()
class RTSPROJECT_API ABuilding : public AActor, public ISelectable, public IAbilitySystemInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UWidgetComponent* HealthShieldBar = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayAbility> BuildUnitAbility;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	UBuildingAttributeSet* BuildingAttributeSet = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UParticleSystemComponent* SpawnPoint = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthShieldComponent* HealthShieldComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FOW")
	UFogOfWarInfluencerComponent* FOWInfluencerComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMiniMapInfluencerComponent* MiniMapInfluencerComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMiniMapIconComponent* MiniMapIconComponent = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category = "Base")
	ARTSPlayerController* PlayerController = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	UInputComponent* DebugInputComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UHealthShieldBarHUD* HealthShieldBarHUD = nullptr;
	
	bool bIsSelected = false;
	bool bIsHighlighted = false;
	bool bJustCreated = false;
	bool bLMBPressed = false;

	FVector LocationToSpawnOutsideTheBorders = FVector(0, 0, -10000);
	
	TArray<TSubclassOf<AActor>> BuildingQueue;
	FGameplayAbilitySpecHandle BuildingUnitHandle;
	bool bIsBuildingUnit = false;
public:	
	ABuilding();
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
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Selected(bool _bIsSelected);
	virtual void Selected_Implementation(bool _bIsSelected) override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Highlighted(bool _bIsHighlighted);
	virtual void Highlighted_Implementation(bool _bIsHighlighted) override;

	void UpdatePositionWhenCreated();
	UFUNCTION(BlueprintCallable, Category = "Building")
	int GetBuildingQueueSizeByClass(TSubclassOf<AActor> ActorClass) const;
	UFUNCTION(BlueprintCallable, Category = "Building")
	void RequestBuildingUnit(TSubclassOf<AActor> ActorClass);
protected:
	void LMBPressed();
	void LMBReleased();
};
