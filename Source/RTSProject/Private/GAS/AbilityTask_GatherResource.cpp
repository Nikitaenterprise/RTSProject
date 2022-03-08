// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/AbilityTask_GatherResource.h"
#include "Actors/Ship.h"
#include "TimerManager.h"
#include "GAS/ResourceGathererAttributeSet.h"

void UAbilityTask_GatherResource::Activate()
{
	Super::Activate();

	bool HasGatherAttribute = false;
	auto Actor = Cast<AShip>(GetOwnerActor());
	if (Actor)
	{
		for (const auto Attribute : Actor->AdditionalAttributeSets)
		{
			ResourceGatherAttribute = Cast<UResourceGathererAttributeSet>(Attribute);
			if (ResourceGatherAttribute)
			{
				HasGatherAttribute = true;
				break;
			}
		}
	}
	if (!HasGatherAttribute)
	{
		EndTask();
	}
		
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &ThisClass::Gather);
	GetWorld()->GetTimerManager().SetTimer(ResourceGatherTimerHandle, TimerDelegate, 1.0, false, false);
}

UAbilityTask_GatherResource* UAbilityTask_GatherResource::GatherResource(UGameplayAbility* OwningAbility, AResource* ResourceToGather)
{
	auto AbilityTask = NewAbilityTask<UAbilityTask_GatherResource>(OwningAbility);
	AbilityTask->ResourceToGather = ResourceToGather;
	return AbilityTask;
}

void UAbilityTask_GatherResource::OnDestroy(bool AbilityEnded)
{
	GetWorld()->GetTimerManager().ClearTimer(ResourceGatherTimerHandle);
	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_GatherResource::Gather()
{
	if (!ResourceToGather)
	{
		OnResourceGathered.Broadcast();
		EndTask();
	}
	auto Actor = Cast<AShip>(GetOwnerActor());
	if (!Actor || !ResourceGatherAttribute)
	{
		EndTask();
		return;
	}

	auto Consumed = ResourceToGather->Consume(ResourceGatherAttribute->GetResourceGatheringSpeed());
	auto NewStorage = Consumed + ResourceGatherAttribute->GetResourceStorage();
	if (NewStorage > ResourceGatherAttribute->GetResourceStorageCapacity())
	{
		Consumed -= NewStorage - ResourceGatherAttribute->GetResourceStorageCapacity();
	}
	ResourceGatherAttribute->SetResourceStorage(Consumed);
}

