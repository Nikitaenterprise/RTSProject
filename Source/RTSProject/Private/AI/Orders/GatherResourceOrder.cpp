#include "AI/Orders/GatherResourceOrder.h"

#include "Actors/Resources/Resource.h"
#include "AI/RTSAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UGatherResourceOrder::ProcessOrder(ARTSAIController* InRTSAIController, UBlackboardComponent* InBlackboard)
{
	Super::ProcessOrder(InRTSAIController, InBlackboard);

	const TWeakObjectPtr<AActor> ActorUnderCursor = HitResultUnderCursor.GetActor();
	if (Cast<AResource>(ActorUnderCursor))
	{
		InRTSAIController->ChangeOrder(EOrderType::GatherResource);
		InBlackboard->SetValueAsObject(FRTSBlackboardKeys::SelfActor, InRTSAIController->GetPawn());
		InBlackboard->SetValueAsObject(FRTSBlackboardKeys::ResourceSource, ActorUnderCursor.Get());
		InBlackboard->SetValueAsVector(FRTSBlackboardKeys::MovementTargetLocation, OrderLocation);
	}
	else
	{
		InRTSAIController->ChangeOrder(EOrderType::MoveOrder);
	}
}

bool UGatherResourceOrder::Execute()
{
	return Super::Execute();
}
