#pragma once

#include "BaseBehavior.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Turret.generated.h"

class UStaticMeshComponent;
class ARTSPlayerController;
class UArrowComponent;
class UHealthShield;
class AShip;

enum ESide
{
	Left,
	Right
};

UCLASS()
class RTSPROJECT_API ATurret : public AActor, public IBaseBehavior
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UHealthShield* HealthShieldComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UArrowComponent* Arrow = nullptr;

	ARTSPlayerController* PlayerController = nullptr;
	AShip* OwnerShip = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float DeltaTime = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float PastTime = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int FireEveryThisSeconds = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float ChanceToFire = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float RotationSpeed = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float MaxAngleDeviation = 45;

	ESide OnWhichSide;

	FRotator Rotation = FRotator(0, 0, 0);

public:	

	ATurret();

	virtual void Tick(float mainDeltaTime) override;

	// Interfaces
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	bool Destroy(bool bNetForce = false, bool bShouldModifyLevel = false);

	UFUNCTION(BlueprintCallable)
	void SetFacingOnMouse();

	UFUNCTION(BlueprintCallable)
	void ShootRocket();

	UFUNCTION(BlueprintCallable)
	void SetFacingLeftRight();

	UFUNCTION(BlueprintCallable)
	void CheckAngle();

	UFUNCTION(BlueprintCallable)
	void RotateTurret();

	virtual void SetOwner(AActor* NewOwner) override;

protected:
	
	virtual void BeginPlay() override;
};
