#pragma once

#include "EntitlementBase.h"
#include "AbilityEntitlement.generated.h"

class UEntitlementUIData;
class UGameplayEffect;
class UGameplayAbility;


UCLASS(BlueprintType)
class RTSPROJECT_API UAbilityEntitlement : public UEntitlementBase
{
	GENERATED_BODY()

public:
	// A collection of gameplay effects that are granted to the unit immediately when this ability becomes available to the player.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffects;

	// These abilities can be triggered either via code or UI once these abilities becomes available to the player. 
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> GameplayAbilities;

	// These abilities is granted to unit but not available from user UI
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> AdditionalAbilities;
	
	// An image for changing cursor for this ability
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSoftObjectPtr<UEntitlementUIData> CursorData;
};
