﻿#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "BaseUnit.h"
#include "GameplayAbilitySpec.h"
#include "BaseUnitWithAbility.generated.h"

class UAbilityEntitlement;
class UHealthShieldAttributeSet;

UCLASS()
class RTSPROJECT_API ABaseUnitWithAbility
	: public ABaseUnit,
	  public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABaseUnitWithAbility(const FObjectInitializer& ObjectInitializer);
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	UFUNCTION(BlueprintCallable)
	const TArray<UAttributeSet*>& GetUnitAttributeSets() { return UnitAttributeSets; }

	UFUNCTION(BlueprintCallable)
	const TArray<TSoftObjectPtr<UAbilityEntitlement>>& GetUnitAbilityEntitlements() const { return UnitAbilityEntitlements; }

	// Begin ISelectable override
	virtual void Selected_Implementation(bool bInIsSelected) override;
	// End ISelectable override

protected:
	template <typename AbilityClass>
	FGameplayAbilitySpecHandle GetAbilityByClass() const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent {nullptr};

	UPROPERTY(EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<UAttributeSet>> Attributes;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> DefaultGameplayEffects;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> UnitAbilities;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TArray<TSoftObjectPtr<UAbilityEntitlement>> UnitAbilityEntitlements;
	
	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TArray<UAbilityEntitlement*> AbilityEntitlements;

	UPROPERTY()
	TArray<UAttributeSet*> UnitAttributeSets;

	UPROPERTY()
	UHealthShieldAttributeSet* HealthShieldAttributeSet = nullptr;
};

template <typename AbilityClass>
FGameplayAbilitySpecHandle ABaseUnitWithAbility::GetAbilityByClass() const
{
	if (AbilitySystemComponent == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("ABaseUnitWithAbility::GetAbility AbilitySystemComponent is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("ABaseUnitWithAbility::GetAbility AbilitySystemComponent is nullptr"));
		return FGameplayAbilitySpecHandle();
	}
	
	for (const auto& Ability : AbilitySystemComponent->GetActivatableAbilities())
	{
		if (Cast<AbilityClass>(Ability.Ability))
		{
			return Ability.Handle;
		}
	}

	return FGameplayAbilitySpecHandle();
}