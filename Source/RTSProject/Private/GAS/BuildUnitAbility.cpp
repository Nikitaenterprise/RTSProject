#include "GAS/BuildUnitAbility.h"

#include "ToolContextInterfaces.h"
#include "Actors/Buildings/Building.h"
#include "Actors/Units/Ship.h"
#include "Actors/Units/Worker.h"
#include "Core/RTSPlayerController.h"
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
	if ((*BuildingQueue).Num() == 0 && Building)
	{
		return;
	}

	float TimeToBuild = 1;
	if (auto* AttributeSet = Building->GetAbilitySystemComponent()->GetSet<UBuildingAttributeSet>())
	{
		TimeToBuild = AttributeSet->GetBuildingSpeed();
	}
	
	BuildUnitTimerDelegate.BindUObject(this, &ThisClass::BuildUnit);
	GetWorld()->GetTimerManager().SetTimer(BuildUnitTimerHandle, BuildUnitTimerDelegate, TimeToBuild, false);
}

void UBuildUnitAbility::BuildUnit()
{
	const TSubclassOf<AActor> Subclass = (*BuildingQueue).Pop();
	UClass* ClassType = Subclass.Get();
	// Add height to spawn location
	const FVector SpawnLocation = Building->GetSpawnPointLocation() + FVector(0, 0, 150);
	// First the ship is created in a place outside the borders
	AShip* SpawnedShip = UFactoriesFunctionLibrary::NewShip(
		GetWorld(),
		ClassType,
		Cast<ARTSPlayerController>(Building->GetPlayerController()),
		SpawnLocation);
	if (IsValid(SpawnedShip))
	{
		UFactoriesFunctionLibrary::AddTurretsToShip(GetWorld(), SpawnedShip);
	}
	
	if (auto* Worker = Cast<AWorker>(SpawnedShip))
	{
		Worker->SetResourceContainerBuilding(Building);
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