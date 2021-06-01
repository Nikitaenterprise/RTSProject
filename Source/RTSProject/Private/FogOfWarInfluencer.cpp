#include "FogOfWarInfluencer.h"

#include "FogOfWar.h"
#include "FogOfWarBoundsVolume.h"

UFogOfWarInfluencer::UFogOfWarInfluencer()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UFogOfWarInfluencer::BeginPlay()
{
	Super::BeginPlay();
}

void UFogOfWarInfluencer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

