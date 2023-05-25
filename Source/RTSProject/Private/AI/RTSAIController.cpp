#include "AI/RTSAIController.h"

#include "BehaviorTree/BlackboardComponent.h"

const FName FRTSBlackboardKeys::EnemyTarget = FName(TEXT("EnemyTarget"));
const FName FRTSBlackboardKeys::MovementTargetLocation = FName(TEXT("MovementTargetLocation"));
const FName FRTSBlackboardKeys::bMovementTargetLocation = FName(TEXT("bMovementTargetLocation"));
const FName FRTSBlackboardKeys::Order = FName(TEXT("Order"));

void ARTSAIController::ChangeOrder(EOrderType InOrder) const
{
	Blackboard->SetValueAsEnum(FRTSBlackboardKeys::Order, static_cast<uint8>(InOrder));
	Blackboard->SetValueAsObject(FRTSBlackboardKeys::EnemyTarget, nullptr);
	Blackboard->SetValueAsBool(FRTSBlackboardKeys::bMovementTargetLocation, false);
}
