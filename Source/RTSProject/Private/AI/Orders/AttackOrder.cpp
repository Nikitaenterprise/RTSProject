// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Orders/AttackOrder.h"

#include "Components/AttackComponent.h"

void UAttackOrder::ProcessOrder(ARTSAIController* InRTSAIController, UBlackboardComponent* InBlackboard)
{
	Super::ProcessOrder(InRTSAIController, InBlackboard);
}

bool UAttackOrder::Execute()
{
	Super::Execute();

	UAttackComponent* AttackComponent = OrderCarrier->FindComponentByClass<UAttackComponent>();
	if (IsValid(AttackComponent) == false)
	{
		return false;
	}

	const AActor* AttackedActor = HitResultUnderCursor.GetActor();
	const UAttackComponent* AttackedActorAttackComponent = AttackedActor->FindComponentByClass<UAttackComponent>();
	if (IsValid(AttackedActorAttackComponent) == false)
	{
		return false;
	}

	if (AttackComponent->GetCanAttack() == false ||
		AttackedActor == OrderCarrier ||
		AttackedActorAttackComponent->GetCanBeAttacked() == false)
	{
		return false;
	}

	AttackComponent->RequestAttack(AttackedActor);
	return true;
}
