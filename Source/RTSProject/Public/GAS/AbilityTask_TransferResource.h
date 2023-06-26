#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_TransferResource.generated.h"

class UResourceAttributeSet;
class UResourceComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitForFullCargo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitForConsumedResource);

UCLASS()
class RTSPROJECT_API UAbilityTask_TransferResource : public UAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FWaitForFullCargo OnCargoFull;

	UPROPERTY(BlueprintAssignable)
	FWaitForConsumedResource OnResourceConsumed;
	
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_TransferResource* TransferResource(
		UGameplayAbility* OwningAbility,
		AActor* ActorToTransfer,
		AActor* ActorFromTransfer);

	virtual void Activate() override;

private:
	virtual void OnDestroy(bool AbilityEnded) override;
	
	UFUNCTION()
	void Transfer();

	UPROPERTY()
	AActor* ActorToTransfer {nullptr};

	UPROPERTY()
	AActor* ActorFromTransfer {nullptr};

	UPROPERTY()
	UAbilitySystemComponent* ActorFromTransferAbilitySystemComponent {nullptr};
	
	UPROPERTY()
	const UResourceAttributeSet* ToResourceAttributeSet {nullptr};
	
	UPROPERTY()
	const UResourceAttributeSet* FromResourceAttributeSet {nullptr};
	
	FTimerHandle TransferTimerHandle;
};