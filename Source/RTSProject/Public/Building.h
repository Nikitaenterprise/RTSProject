#pragma once

#include "BaseBehavior.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"

class UWidgetComponent;
class UHealthShield;
class ARTSPlayerController;
class UHealthShieldBarHUD;

UCLASS()
class RTSPROJECT_API ABuilding : public AActor, public IBaseBehavior
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UWidgetComponent* HealthShieldBar = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	UHealthShield* HealthShieldComponent = nullptr;
	
	ARTSPlayerController* PlayerController = nullptr;

	// Widget
	UHealthShieldBarHUD* HealthShieldBarHUD = nullptr;

	float DeltaTime = 0;
	float PastTime = 0;

	bool bIsSelected = false;
	bool bIsHighlighted = false;
	bool bShouldUpdatePosition = false;

public:	

	ABuilding();

	virtual void Tick(float MainDeltaTime) override;
	
	void SetHealthShieldBar();
	void BindHUD();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	bool Destroy(bool bNetForce = false, bool bShouldModifyLevel = false);
	virtual bool Destroy_Implementation(bool bNetForce = false, bool bShouldModifyLevel = false) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Selected(bool _bIsSelected);
	virtual void Selected_Implementation(bool _bIsSelected) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Highlighted(bool _bIsHighlighted);
	virtual void Highlighted_Implementation(bool _bIsHighlighted) override;

	void UpdateBuildingPosition();

protected:

	virtual void BeginPlay() override;
};
