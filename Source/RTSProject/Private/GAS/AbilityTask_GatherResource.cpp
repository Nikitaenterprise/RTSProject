#include "GAS/AbilityTask_GatherResource.h"
#include "Actors/Units/Ship.h"
#include "TimerManager.h"
#include "GAS/ResourceGathererAttributeSet.h"
#include "GAS/ResourceSourceAttributeSet.h"
#include "Components/ResourceComponent.h"

UAbilityTask_GatherResource* UAbilityTask_GatherResource::GatherResource(UGameplayAbility* OwningAbility, AActor* ResourceToGather)
{
	const auto AbilityTask = NewAbilityTask<UAbilityTask_GatherResource>(OwningAbility);
	AbilityTask->ResourceToGather = ResourceToGather;
	return AbilityTask;
}

void UAbilityTask_GatherResource::Activate()
{
	Super::Activate();

	bool HasGatherAttribute = false;
	const auto* Actor = Cast<AShip>(GetOwnerActor());
	if (Actor)
	{
		for (const auto Attribute : Actor->GetAbilitySystemComponent()->GetSpawnedAttributes())
		{
			ResourceGatherAttributeSet = Cast<UResourceGathererAttributeSet>(Attribute);
			if (ResourceGatherAttributeSet)
			{
				HasGatherAttribute = true;
				break;
			}
		}
	}
	if (!HasGatherAttribute)
	{
		EndTask();
		return;
	}

	auto* TestResourceComponent = ResourceToGather->FindComponentByClass<UResourceComponent>();
	if (!TestResourceComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor %s has no UResourceComponent"), *ResourceToGather->GetName());
		EndTask();
		return;
	}
	ResourceComponent = TestResourceComponent;
		
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &ThisClass::Gather);
	GetWorld()->GetTimerManager().SetTimer(ResourceGatherTimerHandle, TimerDelegate, 1.0, true, false);
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
		OnResourceConsumed.Broadcast();
		EndTask();
		return;
	}
	const auto Ship = Cast<AShip>(GetOwnerActor());
	if (!Ship || !ResourceGatherAttributeSet)
	{
		EndTask();
		return;
	}
	
	UGameplayEffect* ToGathererResourceEffect = NewObject<UGameplayEffect>();
	UGameplayEffect* ToSourceResourceEffect = NewObject<UGameplayEffect>();
	ToGathererResourceEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
	ToSourceResourceEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo ToGathererGameplayModifierInfo;
	FGameplayModifierInfo ToSourceGameplayModifierInfo;
	ToGathererGameplayModifierInfo.Attribute = ResourceGatherAttributeSet->GetCargoStorageAttribute();
	ToGathererGameplayModifierInfo.ModifierOp = EGameplayModOp::Override;
	ToSourceGameplayModifierInfo.Attribute = ResourceComponent->GetResourceSourceAttributeSet()->GetResourceCapacityAttribute();
	ToSourceGameplayModifierInfo.ModifierOp = EGameplayModOp::Override;

	const float CurrentResourceCapacity = ResourceComponent->GetResourceSourceAttributeSet()->GetResourceCapacity();
	const float CurrentStorageCapacity = ResourceGatherAttributeSet->GetCargoStorage();
	auto TryGatherAmount = ResourceGatherAttributeSet->GetResourceGatheringSpeed();
	bool bIsCargoFull = false;
	bool bIsSourceEmpty = false;
	if (CurrentStorageCapacity + TryGatherAmount > ResourceGatherAttributeSet->GetCargoStorageLimit())
	{
		TryGatherAmount -= CurrentStorageCapacity + TryGatherAmount + ResourceGatherAttributeSet->GetCargoStorageLimit();
		bIsCargoFull = true;
	}
	ToGathererGameplayModifierInfo.ModifierMagnitude = FScalableFloat(CurrentStorageCapacity + TryGatherAmount);
	if (CurrentResourceCapacity - TryGatherAmount <=0 )
	{
		bIsSourceEmpty = true;
	}
	ToSourceGameplayModifierInfo.ModifierMagnitude = FScalableFloat(CurrentResourceCapacity - TryGatherAmount);
	
	ToGathererResourceEffect->Modifiers.Add(ToGathererGameplayModifierInfo);
	ToSourceResourceEffect->Modifiers.Add(ToSourceGameplayModifierInfo);
	AbilitySystemComponent->ApplyGameplayEffectToSelf(ToGathererResourceEffect, 0, AbilitySystemComponent->MakeEffectContext());
	ResourceComponent->GetOwnerAbilitySystemComponent()->ApplyGameplayEffectToSelf(
		ToSourceResourceEffect, 0, ResourceComponent->GetOwnerAbilitySystemComponent()->MakeEffectContext());
	if (bIsCargoFull)
	{
		OnCargoFull.Broadcast();
		EndTask();
		return;
	}
	if (bIsSourceEmpty)
	{
		OnResourceConsumed.Broadcast();
		EndTask();
		return;
	}
}

