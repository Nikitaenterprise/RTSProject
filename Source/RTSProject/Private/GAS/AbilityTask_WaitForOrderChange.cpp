#include "GAS/AbilityTask_WaitForOrderChange.h"

#include "AI/RTSAIController.h"

UAbilityTask_WaitForOrderChange* UAbilityTask_WaitForOrderChange::WaitForOrderChange(UGameplayAbility* OwningAbility, TArray<UAbilityTask*> TasksToEnd)
{
	const auto AbilityTask = NewAbilityTask<UAbilityTask_WaitForOrderChange>(OwningAbility);
	AbilityTask->TasksToEnd = TasksToEnd;
	return AbilityTask;
}

void UAbilityTask_WaitForOrderChange::Activate()
{
	Super::Activate();
	
	auto* Pawn = Cast<APawn>(GetOwnerActor());
	if (Pawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAbilityTask_WaitForOrderChange Actor is not a pawn"));
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "UAbilityTask_WaitForOrderChange::Activate() Actor is not a pawn");
		EndTask();
		return;
	}

	RTSAIController = Pawn->GetController<ARTSAIController>();
	if (RTSAIController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAbilityTask_WaitForOrderChange Actor has no controller"));
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "UAbilityTask_WaitForOrderChange::Activate() Actor has no controller");
		EndTask();
		return;
	}

	OrderChangedHandle = RTSAIController->OnOrderChanged.AddUObject(this, &ThisClass::OrderChanged);
}

void UAbilityTask_WaitForOrderChange::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
	
	RTSAIController->OnOrderChanged.Remove(OrderChangedHandle);
	EndTask();
}

void UAbilityTask_WaitForOrderChange::OrderChanged(EOrderType OrderType)
{
	for (const auto& TaskToEnd : TasksToEnd)
	{
		TaskToEnd->EndTask();
	}
	OnOrderChanged.Broadcast(OrderType);
	EndTask();
}
