#include "GAS/AbilityTask_GoToClosestResource.h"

#include "Actors/ResourceManager.h"
#include "Actors/Ship.h"
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
	auto Ship = Cast<AShip>(GetOwnerActor());
	if (!Ship)
	{
		EndTask();
	}
	auto Controller = Cast<ARTSPlayerController>(Ship->GetController());
	if (!Controller)
	{
		EndTask();
	}
	auto ResourceManager = Cast<AResourceManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AResourceManager::StaticClass()));
	if (!ResourceManager)
	{
		EndTask();
	}
	ClosestAsteroidField = ResourceManager->GetClosestAsteroidField(Ship->GetActorLocation());
	if (!ClosestAsteroidField)
	{
		NoAdjacentResourceFound.Broadcast();
		EndTask();
	}
	Ship->RequestMove(ClosestAsteroidField->GetActorLocation());
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
