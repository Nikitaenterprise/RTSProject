#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "Actors/Resources/AsteroidField.h"
#include "Components/ResourceComponent.h"
#include "AbilityTask_GoToClosestResource.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDestinationReached, AAsteroidField*, ClosestAsteroidField);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoAdjacentResourceFound);

UCLASS()
class RTSPROJECT_API UAbilityTask_GoToClosestResource : public UAbilityTask
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FOnDestinationReached OnDestinationReached;
	UPROPERTY(BlueprintAssignable)
	FNoAdjacentResourceFound NoAdjacentResourceFound;

protected:

	UPROPERTY()
	AAsteroidField* ClosestAsteroidField = nullptr;
	UPROPERTY()
	UResourceComponent* ClosestResource = nullptr;
	float MinDistanceToStop = 0.0;
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_GoToClosestResource* GoToClosestResource(UGameplayAbility* OwningAbility, float MinimalDistanceToStop);
	virtual void Activate() override;

protected:
	
	virtual void OnDestroy(bool AbilityEnded) override;
	virtual void TickTask(float DeltaTime) override;
	
};