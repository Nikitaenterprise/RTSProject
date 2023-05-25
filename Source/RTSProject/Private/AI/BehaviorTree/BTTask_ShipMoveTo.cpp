#include "AI/BehaviorTree/BTTask_ShipMoveTo.h"

#include "BehaviorTree/Tasks/BTTask_MoveTo.h"

UBTTask_ShipMoveTo::UBTTask_ShipMoveTo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bRecalculatePathOnDestinationChanged(false)
	, RecalculatePathTolerance(50.f)
{
	NodeName = "Ship Move To";
	bNotifyTick = true;
	
	FlightLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ShipMoveTo, FlightLocationKey));
	FlightResultKey.AddBoolFilter(this,	GET_MEMBER_NAME_CHECKED(UBTTask_ShipMoveTo, FlightResultKey));
	KeyToFlipFlopWhenTaskExits.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ShipMoveTo, KeyToFlipFlopWhenTaskExits));

	FlightLocationKey.AllowNoneAsValue(true);
	FlightResultKey.AllowNoneAsValue(true);
	KeyToFlipFlopWhenTaskExits.AllowNoneAsValue(true);
}

EBTNodeResult::Type UBTTask_ShipMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

EBTNodeResult::Type UBTTask_ShipMoveTo::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::AbortTask(OwnerComp, NodeMemory);
}

uint16 UBTTask_ShipMoveTo::GetInstanceMemorySize() const
{
	return Super::GetInstanceMemorySize();
}

void UBTTask_ShipMoveTo::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);
}

FString UBTTask_ShipMoveTo::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}

void UBTTask_ShipMoveTo::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
}

void UBTTask_ShipMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

void UBTTask_ShipMoveTo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
