#pragma once

#include "Abilities/GameplayAbility.h"
#include "BuildUnitAbility.generated.h"

class ABuilding;

UCLASS()
class RTSPROJECT_API UBuildUnitAbility : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	ABuilding* Building = nullptr;
	TArray<TSubclassOf<AActor>>* BuildingQueue;
	FTimerHandle BuildUnitTimerHandle;
	FTimerDelegate BuildUnitTimerDelegate;
public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled)	override;
	
	UFUNCTION(BlueprintCallable)
	void PrepareBuildingUnit();
	UFUNCTION(BlueprintCallable)
	void BuildUnit();
	UFUNCTION(BlueprintCallable)
	void FinishBuildingUnit();
};
