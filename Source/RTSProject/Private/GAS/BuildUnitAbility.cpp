#include "GAS/BuildUnitAbility.h"

#include "Actors/Building.h"
#include "Actors/Ship.h"
#include "GAS/BuildingAttributeSet.h"
#include "Miscellaneous/FactoriesFunctionLibrary.h"

void UBuildUnitAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	auto TestBuilding = Cast<ABuilding>(GetOwningActorFromActorInfo());
	if (!IsValid(TestBuilding))
	{
		return;
	}
	Building = TestBuilding;
	Building->SetIsBuildingUnit(true);
	BuildingQueue = Building->GetBuildingQueue();
	PrepareBuildingUnit();
}

void UBuildUnitAbility::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Building->SetIsBuildingUnit(false);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBuildUnitAbility::PrepareBuildingUnit()
{
	if ((*BuildingQueue).Num() == 0)
	{
		return;
	}
	const float TimeToBuild = Building->GetBuildingAttributeSet()->GetBuildingSpeed();
	BuildUnitTimerDelegate.BindUObject(this, &ThisClass::BuildUnit);
	GetWorld()->GetTimerManager().SetTimer(BuildUnitTimerHandle, BuildUnitTimerDelegate, TimeToBuild, false);
}

void UBuildUnitAbility::BuildUnit()
{
	const TSubclassOf<AActor> Subclass = (*BuildingQueue).Pop();
	UClass* ClassType = Subclass.Get();
	// Add height to spawn location
	const FVector SpawnLocation = Building->GetSpawnPointLocation() + FVector(0, 0, 100);
	// First the ship is created in a place outside the borders
	AShip* SpawnedShip = UFactoriesFunctionLibrary::NewShip(
		GetWorld(), ClassType, Building->GetPlayerController(), SpawnLocation);
	if (IsValid(SpawnedShip))
	{
		UFactoriesFunctionLibrary::AddTurretsToShip(SpawnedShip);
	}
	FinishBuildingUnit();
}

void UBuildUnitAbility::FinishBuildingUnit()
{
	// Check if there are more units to build
	if ((*BuildingQueue).Num() == 0) 
	{
		K2_EndAbility();
	}
	else
	{
		PrepareBuildingUnit();
	}
}