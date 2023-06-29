#include "GAS/AbilityTask_TransferResource.h"

#include "Actors/Units/Ship.h"
#include "TimerManager.h"
#include "GAS/ResourceAttributeSet.h"

UAbilityTask_TransferResource* UAbilityTask_TransferResource::TransferResource(
	UGameplayAbility* OwningAbility,
	AActor* ActorToTransfer,
	AActor* ActorFromTransfer)
{
	const auto AbilityTask = NewAbilityTask<UAbilityTask_TransferResource>(OwningAbility);
	AbilityTask->ActorToTransfer = ActorToTransfer;
	AbilityTask->ActorFromTransfer = ActorFromTransfer;
	return AbilityTask;
}

void UAbilityTask_TransferResource::Activate()
{
	Super::Activate();

	ActorToTransferAbilitySystemComponent = ActorToTransfer->FindComponentByClass<UAbilitySystemComponent>();
	if (ActorToTransferAbilitySystemComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor %s has no UAbilitySystemComponent"), *ActorToTransfer->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "UAbilityTask_TransferResource::Activate() UAbilitySystemComponent is nullptr");
		EndTask();
		return;
	}
	
	ToResourceAttributeSet = ActorToTransferAbilitySystemComponent->GetSet<UResourceAttributeSet>();
	if (ToResourceAttributeSet == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor %s has no ToResourceAttributeSet"), *ActorToTransfer->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "UAbilityTask_TransferResource::Activate() ToResourceAttributeSet is nullptr");
		EndTask();
		return;
	}

	ActorFromTransferAbilitySystemComponent = ActorFromTransfer->FindComponentByClass<UAbilitySystemComponent>();
	if (ActorFromTransferAbilitySystemComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor %s has no UAbilitySystemComponent"), *ActorFromTransfer->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "UAbilityTask_TransferResource::Activate() UAbilitySystemComponent is nullptr");
		EndTask();
		return;
	}

	FromResourceAttributeSet = ActorFromTransferAbilitySystemComponent->GetSet<UResourceAttributeSet>();
	if (FromResourceAttributeSet == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor %s has no UResourceAttributeSet"), *ActorFromTransfer->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "UAbilityTask_TransferResource::Activate() FromResourceAttributeSet is nullptr");
		EndTask();
		return;
	}
		
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &ThisClass::Transfer);
	GetWorld()->GetTimerManager().SetTimer(TransferTimerHandle, TimerDelegate, 1.0, true, false);
}

void UAbilityTask_TransferResource::OnDestroy(bool AbilityEnded)
{
	GetWorld()->GetTimerManager().ClearTimer(TransferTimerHandle);
	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_TransferResource::Transfer()
{
	if (ActorFromTransfer == nullptr)
	{
		OnResourceConsumed.Broadcast();
		EndTask();
		return;
	}
	
	UGameplayEffect* ToTransferResourceEffect = NewObject<UGameplayEffect>();
	UGameplayEffect* FromTransferResourceEffect = NewObject<UGameplayEffect>();
	ToTransferResourceEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
	FromTransferResourceEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo ToTransferGameplayModifierInfo;
	FGameplayModifierInfo FromTransferGameplayModifierInfo;
	ToTransferGameplayModifierInfo.Attribute = ToResourceAttributeSet->GetResourceAmountAttribute();
	ToTransferGameplayModifierInfo.ModifierOp = EGameplayModOp::Override;
	FromTransferGameplayModifierInfo.Attribute = FromResourceAttributeSet->GetResourceAmountAttribute();
	FromTransferGameplayModifierInfo.ModifierOp = EGameplayModOp::Override;

	const float ToCapacity = ToResourceAttributeSet->GetResourceCapacity();
	const float ToAmount = ToResourceAttributeSet->GetResourceAmount();
	const float FromAmount = FromResourceAttributeSet->GetResourceAmount();
	float TryGatherAmount = ToResourceAttributeSet->GetResourceTransferringSpeed();
	bool bIsCargoFull = false;
	bool bIsSourceEmpty = false;

	if (FromAmount <= 0)
	{
		OnResourceConsumed.Broadcast();
		EndTask();
		return;
	}
	
	if (ToAmount + TryGatherAmount > ToCapacity)
	{
		TryGatherAmount = ToCapacity - ToAmount;
		bIsCargoFull = true;
	}

	bIsSourceEmpty = FromAmount - TryGatherAmount <= 0;
	
	
	ToTransferGameplayModifierInfo.ModifierMagnitude = FScalableFloat(ToAmount + TryGatherAmount);
	FromTransferGameplayModifierInfo.ModifierMagnitude = FScalableFloat(FromAmount - TryGatherAmount);
	
	ToTransferResourceEffect->Modifiers.Add(ToTransferGameplayModifierInfo);
	FromTransferResourceEffect->Modifiers.Add(FromTransferGameplayModifierInfo);
	
	ActorToTransferAbilitySystemComponent->ApplyGameplayEffectToSelf(
		ToTransferResourceEffect, 0, ActorToTransferAbilitySystemComponent->MakeEffectContext());
	ActorFromTransferAbilitySystemComponent->ApplyGameplayEffectToSelf(
		FromTransferResourceEffect, 0, ActorFromTransferAbilitySystemComponent->MakeEffectContext());

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