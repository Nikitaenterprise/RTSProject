#include "FogOfWarInfluencer.h"

#include "FogOfWar.h"
#include "FogOfWarBoundsVolume.h"
#include "RTSPlayerController.h"

UFogOfWarInfluencer::UFogOfWarInfluencer()
{
	PrimaryComponentTick.bCanEverTick = true;
	// SightRadius can't be negative
	if (SightRadius < 0) SightRadius *= -1;
}

void UFogOfWarInfluencer::BeginPlay()
{
	Super::BeginPlay();
}

void UFogOfWarInfluencer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UFogOfWarInfluencer::Initialize(ARTSPlayerController* PlayerController)
{
	if (!PlayerController->FogOfWar) return;
	FOW = PlayerController->FogOfWar;
	FOW->RegisterActor(GetOwner());
}

