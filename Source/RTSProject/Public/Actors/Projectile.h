#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "GameFramework/Actor.h"

#include "Projectile.generated.h"

class ATurret;
class UProjectileAttributeSet;
class UProjectileMovementComponent;
class UGameplayEffect;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnProjectileHit, AActor*, ActorToHit);

UCLASS()
class RTSPROJECT_API AProjectile : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	USceneComponent* SceneComponent {nullptr};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship")
	UProjectileMovementComponent* ProjectileMovementComponent {nullptr};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* StaticMesh {nullptr};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	ATurret* OwnerTurret {nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent {nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	UProjectileAttributeSet* ProjectileAttributeSet {nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DamageEffectClass {nullptr};
	FTimerHandle MaxLifeTimeTimerHandle;
public:
	FOnProjectileHit OnProjectileHit;
	
	AProjectile(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void OnOverlapBegin_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};