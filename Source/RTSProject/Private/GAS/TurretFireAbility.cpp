#include "GAS/TurretFireAbility.h"
#include "Actors/Turret.h"
#include "GAS/TurretAttributeSet.h"

void UTurretFireAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	Turret = Cast<ATurret>(GetOwningActorFromActorInfo());
	if (!IsValid(Turret))
	{
		return;
	}
	if (!Turret->GetTurretAttributeSet())
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		FireTimerHandle, this, &ThisClass::Fire, Turret->GetTurretAttributeSet()->GetFireRate(), true);
}

void UTurretFireAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void UTurretFireAbility::Fire()
{
}
