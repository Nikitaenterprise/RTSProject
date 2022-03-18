#pragma once

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
class RTSPROJECT_API ATurret : public AActor
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
	TSubclassOf<ARocket> RocketClass;
	UPROPERTY(BlueprintReadOnly, Category = "Rocket")
	TArray<ARocket*> FiredRockets;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	const AActor* ActorToAttack = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int FireEveryThisSeconds = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float ChanceToFire = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float RotationSpeed = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float MaxAngleDeviation = 45;

	

private:
	
	// Timer handle for firing rockets
	FTimerHandle THForFiring;
	bool bShouldFire = false;
	bool bIsOrderedToAttack = false;
	ESide OnWhichSide;
	FRotator DeltaRotation = FRotator(0, 0, 0);
	
public:	

	ATurret();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void RequestAttack(const AActor* _ActorToAttack);
	UFUNCTION(BlueprintCallable)
	void Fire();
	UFUNCTION(BlueprintCallable)
	void SetFacingOnMouse();
	UFUNCTION(BlueprintCallable)
	void SetFacingLeftRight();
	UFUNCTION(BlueprintCallable)
	void SetFacingOnActor(const AActor* ActorToSetFacingTo);
	UFUNCTION(BlueprintCallable)
	void CheckAngle();
	
};
