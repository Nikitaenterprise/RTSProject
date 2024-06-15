#pragma once

#include "GameplayAbilitySpec.h"
#include "Blueprint/UserWidget.h"
#include "UnitAbilityButton.generated.h"

class UAbilitySystemComponent;
class UButton;
class UGameplayAbility;
class UAbilityEntitlement;
class UImage;
class UTextBlock;


UCLASS()
class RTSPROJECT_API UUnitAbilityButton : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetupAbilityButton(
		UAbilitySystemComponent* InAbilitySystemComponent,
		const FGameplayAbilitySpec& InGameplayAbilitySpec,
		const UAbilityEntitlement* UIData);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnAbilityActivated();
	
	UPROPERTY(meta = (BindWidget))
	UButton* Button {nullptr};

	UPROPERTY(meta = (BindWidget))
	UImage* AbilityImage {nullptr};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Description {nullptr};

	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent {nullptr};

	FGameplayAbilitySpec Spec;
};
