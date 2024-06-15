#include "AI/Orders/MoveOrder.h"

#include "AI/RTSAIController.h"
#include "AI/Orders/OrdersProcessor.h"
#include "BehaviorTree/BlackboardComponent.h"

void UMoveOrder::ProcessOrder(ARTSAIController* InRTSAIController, UBlackboardComponent* InBlackboard)
{
	Super::ProcessOrder(InRTSAIController, InBlackboard);

	InRTSAIController->ChangeOrder(EOrderType::MoveOrder);
	
	InBlackboard->SetValueAsBool(FRTSBlackboardKeys::bMovementTargetLocation, true);
	InBlackboard->SetValueAsVector(FRTSBlackboardKeys::MovementTargetLocation, OrderLocation);
}