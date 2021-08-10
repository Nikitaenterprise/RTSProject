#include "MiniMapInfluencerComponent.h"

#include "GameHUD.h"
#include "MiniMapHUD.h"
#include "RTSPlayerController.h"

UMiniMapInfluencerComponent::UMiniMapInfluencerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UMiniMapInfluencerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMiniMapInfluencerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UMiniMapInfluencerComponent::Initialize(const ARTSPlayerController* PlayerController)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController in UFogOfWarInfluencerComponent->Initialize() is nullptr"));
		return;
	}
	if (!PlayerController->GameHUD->MiniMapHUD) 
	{
		UE_LOG(LogTemp, Error, TEXT("MiniMapHUD in UFogOfWarInfluencerComponent->Initialize() is nullptr"));
		return;
	}
	MiniMapHUD = PlayerController->GameHUD->MiniMapHUD;
	MiniMapHUD->RegisterActor(GetOwner());
}
