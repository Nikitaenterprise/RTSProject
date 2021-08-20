#include "MiniMapInfluencerComponent.h"

#include "GameHUD.h"
#include "MiniMapWidget.h"
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
	if (!PlayerController->GameHUD->MiniMapWidget) 
	{
		UE_LOG(LogTemp, Error, TEXT("MiniMapWidget in UFogOfWarInfluencerComponent->Initialize() is nullptr"));
		return;
	}
	MiniMapHUD = PlayerController->GameHUD->MiniMapWidget;
	MiniMapHUD->RegisterActor(GetOwner());
}
