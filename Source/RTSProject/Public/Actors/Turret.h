#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "GameFramework/Actor.h"
#include "Turret.generated.h"

class AProjectile;
class UStaticMeshComponent;
class ARTSPlayerController;
class UArrowComponent;
class AShip;
class UHealthShieldAttributeSet;
class UTurretAttributeSet;
class UTurretFireAbility;

enum class ESide
{
	Left,
	Right
};

UCLASS()
class RTSPROJECT_API ATurret : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UArrowComponent* Arrow = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Base")
	ARTSPlayerController* PlayerController = nullptr;
	UPROPERTY(BlueprintReadOnly)
	AShip* OwnerShip = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<AProjectile> ProjectileClass;
	UPROPERTY(BlueprintReadOnly, Category = "Projectile")
	TArray<AProjectile*> FiredProjectiles;
	UPROPERTY(BlueprintReadWrite)
	const AActor* ActorToAttack = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	UHealthShieldAttributeSet* HealthShieldAttributeSet = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	UTurretAttributeSet* TurretAttributeSet = nullptr;
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UTurretFireAbility> TurretFireAbility;
	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	FGameplayAbilitySpecHandle TurretFireAbilityHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float RotationSpeed = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float MaxAngleDeviation = 45;

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

	AShip* GetOwnerShip() const { return OwnerShip; };
	UTurretAttributeSet* GetTurretAttributeSet() const { return TurretAttributeSet; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

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
