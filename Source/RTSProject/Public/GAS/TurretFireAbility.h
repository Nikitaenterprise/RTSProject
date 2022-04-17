#pragma once

#include "Abilities/GameplayAbility.h"
#include "TurretFireAbility.generated.h"

class ATurret;

UCLASS()
class RTSPROJECT_API UTurretFireAbility : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly)
	ATurret* Turret{nullptr};
	UPROPERTY(BlueprintReadOnly)
	AActor* Target{nullptr};
	FTimerHandle FireTimerHandle;
public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;
protected:
	void Fire();
};
