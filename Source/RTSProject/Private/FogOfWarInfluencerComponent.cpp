#include "FogOfWarInfluencerComponent.h"

#include "FogOfWar.h"
#include "RTSPlayerController.h"

UFogOfWarInfluencerComponent::UFogOfWarInfluencerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// SightRadius can't be negative
	if (SightRadius < 0) SightRadius *= -1;
}

void UFogOfWarInfluencerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFogOfWarInfluencerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UFogOfWarInfluencerComponent::Initialize(ARTSPlayerController* PlayerController)
{
	if (!PlayerController->FogOfWar) return;
	FOW = PlayerController->FogOfWar;
	FOW->RegisterActor(GetOwner());
}
