#pragma once

#include "Actors/Units/BaseUnitWithAbility.h"
#include "Particles/ParticleSystemComponent.h"
#include "Building.generated.h"

class UMiniMapIconComponent;
class UMiniMapInfluencerComponent;
class UFogOfWarInfluencerComponent;
class UHealthShield;
class ARTSPlayerController;
class UHealthShieldWidget;
class UAbilitySystemComponent;
class UBuildingAttributeSet;
class UHealthShieldAttributeSet;

UENUM()
enum class EBuildingType : uint8
{
	Main,
	ResourceStorage,
	UnitFactory
};

UCLASS()
class RTSPROJECT_API ABuilding
	: public ABaseUnitWithAbility
{
	GENERATED_BODY()

public:	
	ABuilding(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	ARTSPlayerController* GetPlayerController() const { return PlayerController; }
	TArray<TSubclassOf<AActor>>* GetBuildingQueue() { return &BuildingQueue; }
	FVector GetSpawnPointLocation() const { return SpawnPoint->GetComponentLocation(); }
	void SetSpawnPointLocation(const FVector& Location) const { SpawnPoint->SetWorldLocation(Location); }

	UFUNCTION(BlueprintCallable)
	EBuildingType GetBuildingType() const { return BuildingType; }

	UFUNCTION(BlueprintCallable)
	void SetIsBuildingUnit(bool NewIsBuildingUnit) { bIsBuildingUnit = NewIsBuildingUnit;}
	
	void UpdatePositionWhenCreated();
	UFUNCTION(BlueprintCallable, Category = "Building")
	int GetBuildingQueueSizeByClass(TSubclassOf<AActor> ActorClass) const;
	UFUNCTION(BlueprintCallable, Category = "Building")
	void RequestBuildingUnit(TSubclassOf<AActor> ActorClass);
	
protected:
	UFUNCTION()
	void LMBPressed(ETriggerEvent Event);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* StaticMesh = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base")
	UParticleSystemComponent* SpawnPoint = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FOW")
	UFogOfWarInfluencerComponent* FOWInfluencerComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMiniMapInfluencerComponent* MiniMapInfluencerComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMiniMapIconComponent* MiniMapIconComponent = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	UInputComponent* DebugInputComponent = nullptr;
	
	bool bLMBPressed = false;

	FVector LocationToSpawnOutsideTheBorders = FVector(0, 0, -10000);
	
	TArray<TSubclassOf<AActor>> BuildingQueue;
	FGameplayAbilitySpecHandle BuildingUnitHandle;
	bool bIsBuildingUnit = false;

	UPROPERTY(EditDefaultsOnly)
	EBuildingType BuildingType;
};
