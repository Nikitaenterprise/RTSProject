// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Orders/SetSpawnPointOrder.h"

#include "Actors/Buildings/Building.h"

void USetSpawnPointOrder::ProcessOrder(ARTSAIController* InRTSAIController, UBlackboardComponent* InBlackboard)
{
	Super::ProcessOrder(InRTSAIController, InBlackboard);

	
}

bool USetSpawnPointOrder::Execute()
{
	Super::Execute();

	if (OrderCarrier == nullptr)
	{
		return false;
	}

	ABuilding* Building = Cast<ABuilding>(OrderCarrier);
	if (IsValid(Building) == false)
	{
		return false;
	}

	FVector NewLocation = HitResultUnderCursor.Location;
	NewLocation.Z = Building->GetActorLocation().Z;
	Building->SetSpawnPointLocation(NewLocation);

	return true;
}
