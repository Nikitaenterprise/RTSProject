#include "Actors/Units/Worker.h"

#include "AbilitySystemComponent.h"
#include "GAS/GatherResourceAbility.h"
#include "GAS/ResourceGathererAttributeSet.h"

AWorker::AWorker(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWorker::BeginPlay()
{
	Super::BeginPlay();
	
	GatherResourceAbilityHandle = GetAbilityByClass<UGatherResourceAbility>();
}