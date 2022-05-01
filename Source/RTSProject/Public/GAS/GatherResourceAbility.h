#pragma once

#include "Abilities/GameplayAbility.h"
#include "GatherResourceAbility.generated.h"


UCLASS()
class RTSPROJECT_API UGatherResourceAbility : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	AActor* Target = nullptr;
public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION(BlueprintCallable)
	void Gather();
};
