#include "AI/RTSAIController.h"

#include "BehaviorTree/BlackboardComponent.h"

const FName FRTSBlackboardKeys::SelfActor = FName(TEXT("SelfActor"));
const FName FRTSBlackboardKeys::EnemyTarget = FName(TEXT("EnemyTarget"));
const FName FRTSBlackboardKeys::MovementTargetLocation = FName(TEXT("MovementTargetLocation"));
const FName FRTSBlackboardKeys::bMovementTargetLocation = FName(TEXT("bMovementTargetLocation"));
const FName FRTSBlackboardKeys::Order = FName(TEXT("Order"));
const FName FRTSBlackboardKeys::ResourceSource = FName(TEXT("ResourceSource"));

void ARTSAIController::BeginPlay()
{
	Super::BeginPlay();

	ObtainPlayerState();
}

void ARTSAIController::ChangeOrder(EOrderType InOrder) const
{
	Blackboard->SetValueAsEnum(FRTSBlackboardKeys::Order, static_cast<uint8>(InOrder));
	switch(InOrder)
	{
	case EOrderType::MoveOrder:
		Blackboard->SetValueAsBool(FRTSBlackboardKeys::bMovementTargetLocation, true);
		break;
	}
	// Blackboard->SetValueAsObject(FRTSBlackboardKeys::EnemyTarget, nullptr);
	// Blackboard->SetValueAsBool(FRTSBlackboardKeys::bMovementTargetLocation, false);
}

void ARTSAIController::ClearAllBlackboardKeys() const
{
	Blackboard->SetValueAsObject(FRTSBlackboardKeys::SelfActor, nullptr);
	Blackboard->SetValueAsObject(FRTSBlackboardKeys::ResourceSource, nullptr);
	Blackboard->SetValueAsObject(FRTSBlackboardKeys::EnemyTarget, nullptr);
	Blackboard->SetValueAsEnum(FRTSBlackboardKeys::Order, static_cast<uint8>(EOrderType::NoOrder));
	Blackboard->SetValueAsVector(FRTSBlackboardKeys::MovementTargetLocation, FVector(0,0,0));
	Blackboard->SetValueAsBool(FRTSBlackboardKeys::bMovementTargetLocation, false);
}

void ARTSAIController::RequestMoveToLocation(const FVector& InLocation)
{
	Blackboard->SetValueAsEnum(FRTSBlackboardKeys::Order, static_cast<uint8>(EOrderType::MoveOrder));
	Blackboard->SetValueAsBool(FRTSBlackboardKeys::bMovementTargetLocation, true);
	Blackboard->SetValueAsVector(FRTSBlackboardKeys::MovementTargetLocation, InLocation);
}
