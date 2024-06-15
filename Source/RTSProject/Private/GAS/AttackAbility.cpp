#include "GAS/AttackAbility.h"
#include "Components/AttackComponent.h"

void UAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!IsValid(GetOwningActorFromActorInfo()))
	{
		K2_EndAbility();
	}
	auto FoundComponent = GetOwningActorFromActorInfo()->FindComponentByClass<UAttackComponent>();
	if (!IsValid(FoundComponent))
	{
		K2_EndAbility();
	}
	AttackerAttackComponent = FoundComponent;
	if (!AttackerAttackComponent->GetCanAttack())
	{
		K2_EndAbility();
	}
}

void UAttackAbility::PrepareAttack()
{
	if (!IsValid(Target))
	{
		K2_EndAbility();
	}
	auto FoundComponent = Target->FindComponentByClass<UAttackComponent>();
	if (!IsValid(FoundComponent))
	{
		K2_EndAbility();
	}
	TargetAttackComponent = FoundComponent;
	if (!TargetAttackComponent->GetCanBeAttacked())
	{
		K2_EndAbility();
	}
}

void UAttackAbility::Attack()
{
	if (!IsValid(Target))
	{
		K2_EndAbility();
	}
	if (!TargetAttackComponent->GetCanBeAttacked())
	{
		K2_EndAbility();
	}
}
