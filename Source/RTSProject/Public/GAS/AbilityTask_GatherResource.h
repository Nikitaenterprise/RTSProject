// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Abilities/Tasks/AbilityTask.h"
#include "Actors/Resource.h"
#include "AbilityTask_GatherResource.generated.h"

class UResourceGathererAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitForResourceGathered);
/**
 * 
 */
UCLASS()
class RTSPROJECT_API UAbilityTask_GatherResource : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FWaitForResourceGathered OnResourceGathered;

	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_GatherResource* GatherResource(UGameplayAbility* OwningAbility, AResource* ResourceToGather);

private:
	virtual void OnDestroy(bool AbilityEnded) override;
	UFUNCTION()
	void Gather();
	UPROPERTY()
	UResourceGathererAttributeSet* ResourceGatherAttribute = nullptr;
	UPROPERTY()
	AResource* ResourceToGather = nullptr;
	FTimerHandle ResourceGatherTimerHandle;
	
};
