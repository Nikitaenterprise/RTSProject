#include "Components/FogOfWarInfluencerComponent.h"

#include "Actors/FogOfWar.h"
#include "Core/RTSPlayerController.h"

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

void UFogOfWarInfluencerComponent::Initialize(const ARTSPlayerController* PlayerController)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController in UFogOfWarInfluencerComponent->Initialize() is nullptr"));
		return;
	}
	if (!PlayerController->FogOfWar)
	{
		UE_LOG(LogTemp, Error, TEXT("FogOfWar in UFogOfWarInfluencerComponent->Initialize() is nullptr"));
		return;
	}
	FOW = PlayerController->FogOfWar;
	FOW->RegisterActor(GetOwner());
}

