#include "Components/MiniMapIconComponent.h"

#include "Actors/FogOfWar.h"
#include "Core/RTSPlayerController.h"
#include "Volumes/FogOfWarBoundsVolume.h"

UMiniMapIconComponent::UMiniMapIconComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMiniMapIconComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMiniMapIconComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UMiniMapIconComponent::Initialize(const ARTSPlayerController* PlayerController)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController in UMiniMapIconComponent::Initialize() is nullptr"));
		return;
	}
	if (!PlayerController->FogOfWar)
	{
		UE_LOG(LogTemp, Error, TEXT("FogOfWar in UMiniMapIconComponent::Initialize() is nullptr"));
		return;
	}
	if(!PlayerController->FogOfWar->FOWBoundsVolume)
	{
		UE_LOG(LogTemp, Error, TEXT("FOWBoundsVolume in UMiniMapIconComponent::Initialize() is nullptr"));
		return;
	}
	

	FOWBoundsVolume = PlayerController->FogOfWar->FOWBoundsVolume;
}