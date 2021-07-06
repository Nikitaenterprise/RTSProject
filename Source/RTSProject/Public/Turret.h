#pragma once

#include "BaseBehavior.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Turret.generated.h"

class ARocket;
class UHealthShieldComponent;
class UStaticMeshComponent;
class ARTSPlayerController;
class UArrowComponent;
class UHealthShield;
class AShip;

enum class ESide
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
	UArrowComponent* Arrow = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthShieldComponent* HealthShieldComponent = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Base")
	ARTSPlayerController* PlayerController = nullptr;
	UPROPERTY(BlueprintReadOnly)
	AShip* OwnerShip = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rocket")
	TSubclassOf<ARocket> ShootableRocket;
	
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TArray<ARocket*> FiredRockets;
	bool bShouldFire = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int FireEveryThisSeconds = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float ChanceToFire = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float RotationSpeed = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float MaxAngleDeviation = 45;

	ESide OnWhichSide;

	FRotator Rotation = FRotator(0, 0, 0);

public:	

	ATurret();

	virtual void Tick(float DeltaTime) override;
	void Initialize(ARTSPlayerController* RTSController, AShip* Ship);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	bool Destroy(bool bNetForce = false, bool bShouldModifyLevel = false);
	UFUNCTION(BlueprintCallable)
	void SetFacingOnMouse();
	UFUNCTION(BlueprintCallable)
	void Fire();
	UFUNCTION(BlueprintCallable)
	void SetFacingLeftRight();
	UFUNCTION(BlueprintCallable)
	void CheckAngle();
	UFUNCTION(BlueprintCallable)
	void RotateTurret(float DeltaTime);

protected:
	
	virtual void BeginPlay() override;
};
