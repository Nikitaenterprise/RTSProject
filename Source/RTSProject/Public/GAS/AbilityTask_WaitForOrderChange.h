#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "AI/Orders/OrdersProcessor.h"
#include "AbilityTask_WaitForOrderChange.generated.h"


class ARTSAIController;

UCLASS()
class RTSPROJECT_API UAbilityTask_WaitForOrderChange : public UAbilityTask
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOrderChanged, EOrderType, OrderType);

public:
	UPROPERTY(BlueprintAssignable)
	FOnOrderChanged OnOrderChanged;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_WaitForOrderChange* WaitForOrderChange(UGameplayAbility* OwningAbility, TArray<UAbilityTask*> TasksToEnd);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	void OrderChanged(EOrderType OrderType);

	UPROPERTY()
	AActor* ActorToObserve {nullptr};

	UPROPERTY()
	ARTSAIController* RTSAIController {nullptr};

	FDelegateHandle OrderChangedHandle;

	UPROPERTY()
	TArray<UAbilityTask*> TasksToEnd;
};
