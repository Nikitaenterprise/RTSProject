#include "Actors/Units/Worker.h"

#include "AbilitySystemComponent.h"
#include "GAS/GatherResourceAbility.h"
#include "GAS/ResourceGathererAttributeSet.h"

AWorker::AWorker(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UShipMovementComponent>(FName("ShipMovementComponent")))
{
	PrimaryActorTick.bCanEverTick = true;
	ResourceGathererAttributeSet = CreateDefaultSubobject<UResourceGathererAttributeSet>(TEXT("ResourceGathererAttributeSet"));
}

void AWorker::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(GatherResourceAbility.Get()))
	{
		GatherResourceAbilityHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(GatherResourceAbility, 1, INDEX_NONE, this));
	}
	AbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(ResourceGathererAttributeSet);
}