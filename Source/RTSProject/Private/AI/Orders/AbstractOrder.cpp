// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Orders/AbstractOrder.h"

#include "AI/RTSAIController.h"

void UAbstractOrder::Initialize(
	const TArray<AActor*>& InSelectedActors,
	AActor* InOrderCarrier,
	const FVector& InOrderLocation,
	const FHitResult& InHitResult)
{
	SelectedActors = InSelectedActors;
	OrderCarrier = InOrderCarrier;
	OrderLocation = InOrderLocation;
	HitResultUnderCursor = InHitResult;
}

bool UAbstractOrder::Execute()
{
	if (SelectedActors.Num() == 0)
	{
		return false;
	}

	// Set one of the orders to selected squads: Move or Attack
	for (AActor* Actor : SelectedActors)
	{
		if (Actor->IsPendingKillPending())
		{
			continue;
		}

		ARTSAIController* ActorController = Cast<ARTSAIController>(Actor->GetInstigatorController());
		if (!ActorController)
		{
			continue;
		}

		UBlackboardComponent* Blackboard = ActorController->GetBlackboardComponent();
		if (!Blackboard)
		{
			continue;
		}

		ProcessOrder(ActorController, Blackboard);
	}
	
	return true;
}

bool UAbstractOrder::Undo()
{
	return true;
}

void UAbstractOrder::ProcessOrder(ARTSAIController* InRTSAIController, UBlackboardComponent* InBlackboard)
{
}
