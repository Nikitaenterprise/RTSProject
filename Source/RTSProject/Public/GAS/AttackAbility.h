#pragma once

#include "Abilities/GameplayAbility.h"
#include "AttackAbility.generated.h"

class UAttackComponent;

UCLASS()
class RTSPROJECT_API UAttackAbility : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	UAttackComponent* AttackerAttackComponent = nullptr;
	UPROPERTY()
	AActor* Target = nullptr;
	UPROPERTY()
	UAttackComponent* TargetAttackComponent = nullptr;
public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* NewTarget) { Target = NewTarget; }
protected:
	UFUNCTION(BlueprintCallable)
	void PrepareAttack();
	UFUNCTION(BlueprintCallable)
	void Attack();
};
