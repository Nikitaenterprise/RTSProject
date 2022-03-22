#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_GatherResource.generated.h"

class UResourceGathererAttributeSet;
class UResourceComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitForFullCargo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitForConsumedResource);

UCLASS()
class RTSPROJECT_API UAbilityTask_GatherResource : public UAbilityTask
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintAssignable)
	FWaitForFullCargo OnCargoFull;
	UPROPERTY(BlueprintAssignable)
	FWaitForConsumedResource OnResourceConsumed;
	
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_GatherResource* GatherResource(UGameplayAbility* OwningAbility, AActor* ResourceToGather);
	virtual void Activate() override;

private:
	
	virtual void OnDestroy(bool AbilityEnded) override;
	UFUNCTION()
	void Gather();
	UPROPERTY()
	UResourceGathererAttributeSet* ResourceGatherAttributeSet = nullptr;
	UPROPERTY()
	AActor* ResourceToGather = nullptr;
	UPROPERTY()
	UResourceComponent* ResourceComponent = nullptr;
	FTimerHandle ResourceGatherTimerHandle;
	
};
