#include "GAS/AbilityTask_WaitForMouseClick.h"

#include "Kismet/GameplayStatics.h"

UAbilityTask_WaitForMouseClick* UAbilityTask_WaitForMouseClick::WaitForMouseClick(UGameplayAbility* OwningAbility)
{
	const auto AbilityTask = NewAbilityTask<UAbilityTask_WaitForMouseClick>(OwningAbility);
	return AbilityTask;
}

void UAbilityTask_WaitForMouseClick::Activate()
{
	Super::Activate();

	auto* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController == nullptr)
	{
		EndTask();
		return;
	}

	const auto InputComponent = PlayerController->InputComponent;
	if (InputComponent == nullptr)
	{
		EndTask();
		return;
	}
	
	LMBClickedBinding = InputComponent->BindAction(TEXT("LMB"), IE_Pressed, this, &ThisClass::LMBPressed);
	RMBClickedBinding = InputComponent->BindAction(TEXT("RMB"), IE_Pressed, this, &ThisClass::RMBPressed);
}

void UAbilityTask_WaitForMouseClick::LMBPressed()
{
	OnLMBClicked.Broadcast();
}

void UAbilityTask_WaitForMouseClick::RMBPressed()
{
	OnRMBClicked.Broadcast();
}