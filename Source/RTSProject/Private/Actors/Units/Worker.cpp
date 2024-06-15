#include "Actors/Units/Worker.h"

#include "AbilitySystemComponent.h"

AWorker::AWorker(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWorker::BeginPlay()
{
	Super::BeginPlay();
}