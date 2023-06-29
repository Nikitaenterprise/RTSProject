#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitForMouseClick.generated.h"


UCLASS()
class RTSPROJECT_API UAbilityTask_WaitForMouseClick : public UAbilityTask
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLMBClicked);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRMBClicked);
	
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_WaitForMouseClick* WaitForMouseClick(UGameplayAbility* OwningAbility);
	
	UPROPERTY(BlueprintAssignable)
	FOnLMBClicked OnLMBClicked;
	
	UPROPERTY(BlueprintAssignable)
	FOnRMBClicked OnRMBClicked;

protected:
	virtual void Activate() override;

	void LMBPressed();
	void RMBPressed();

private:
	FInputActionBinding LMBClickedBinding;
	FInputActionBinding RMBClickedBinding;
};
