#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_TransferResource.generated.h"

class UResourceAttributeSet;
class UResourceComponent;


UCLASS()
class RTSPROJECT_API UAbilityTask_TransferResource : public UAbilityTask
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCargoFull);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResourceConsumed);
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnCargoFull OnCargoFull;

	UPROPERTY(BlueprintAssignable)
	FOnResourceConsumed OnResourceConsumed;
	
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_TransferResource* TransferResource(
		UGameplayAbility* OwningAbility,
		AActor* ActorToTransfer,
		AActor* ActorFromTransfer);

protected:
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
	UAbilitySystemComponent* ActorToTransferAbilitySystemComponent {nullptr};
	
	UPROPERTY()
	UAbilitySystemComponent* ActorFromTransferAbilitySystemComponent {nullptr};
	
	UPROPERTY()
	const UResourceAttributeSet* ToResourceAttributeSet {nullptr};
	
	UPROPERTY()
	const UResourceAttributeSet* FromResourceAttributeSet {nullptr};
	
	FTimerHandle TransferTimerHandle;
};