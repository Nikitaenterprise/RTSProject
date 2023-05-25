// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Orders/AbstractOrder.h"

#include "AI/RTSAIController.h"

void UAbstractOrder::Initialize(const TArray<AActor*>& InSelectedActors, FVector InOrderLocation)
{
	SelectedActors = InSelectedActors;
	OrderLocation = InOrderLocation;
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
		if (Actor->IsPendingKill())
		{
			continue;
		}

		ARTSAIController* ActorController = Cast<ARTSAIController>(Actor->GetInstigatorController());
		if (!ActorController)
		{
			continue;
		}

		UBlackboardComponent* SquadBlackboard = ActorController->GetBlackboardComponent();
		if (!SquadBlackboard)
		{
			continue;
		}

		ProcessOrder(ActorController, SquadBlackboard);
	}
	
	return true;
}

void UAbstractOrder::ProcessOrder(ARTSAIController* InRTSAIController, UBlackboardComponent* InBlackboard)
{
}


