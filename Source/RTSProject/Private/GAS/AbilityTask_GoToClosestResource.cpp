#include "GAS/AbilityTask_GoToClosestResource.h"

#include "Actors/Resources/ResourceManager.h"
#include "Actors/Units/Ship.h"
#include "AI/ShipAIController.h"
#include "Core/RTSGameMode.h"
#include "Core/RTSPlayerController.h"
#include "Kismet/GameplayStatics.h"

UAbilityTask_GoToClosestResource* UAbilityTask_GoToClosestResource::GoToClosestResource(UGameplayAbility* OwningAbility, float MinimalDistanceToStop)
{
	auto AbilityTask = NewAbilityTask<UAbilityTask_GoToClosestResource>(OwningAbility);
	AbilityTask->MinDistanceToStop = MinimalDistanceToStop;
	return AbilityTask;
}

void UAbilityTask_GoToClosestResource::Activate()
{
	Super::Activate();
	const auto* Ship = Cast<AShip>(GetOwnerActor());
	if (!Ship)
	{
		EndTask();
		return;
	}
	auto* ResourceManager = ARTSGameMode::GetRTSGameMode(GetWorld())->GetResourceManager();
	if (!ResourceManager)
	{
		EndTask();
		return;
	}
	ClosestAsteroidField = ResourceManager->GetClosestAsteroidField(Ship->GetActorLocation());
	if (!ClosestAsteroidField)
	{
		NoAdjacentResourceFound.Broadcast();
		EndTask();
		return;
	}
	OnDestinationReached.Broadcast(ClosestAsteroidField);
}

void UAbilityTask_GoToClosestResource::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_GoToClosestResource::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	if (!ClosestAsteroidField)
	{
		EndTask();
	}
	const auto Distance = FVector::Distance(GetOwnerActor()->GetActorLocation(), ClosestAsteroidField->GetActorLocation());
	if (Distance <= MinDistanceToStop)
	{
		OnDestinationReached.Broadcast(ClosestAsteroidField);
	}
}
