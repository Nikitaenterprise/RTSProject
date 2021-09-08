#include "Components/MiniMapInfluencerComponent.h"

#include "UI/GameHUD.h"
#include "UI/MiniMapWidget.h"
#include "Core/RTSPlayerController.h"

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
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr in UMiniMapInfluencerComponent::Initialize()"));
		return;
	}
	if (!PlayerController->GameHUD->MiniMapWidget) 
	{
		UE_LOG(LogTemp, Error, TEXT("MiniMapWidget is nullptr in UMiniMapInfluencerComponent::Initialize()"));
		return;
	}
	MiniMapWidget = PlayerController->GameHUD->MiniMapWidget;
	MiniMapWidget->RegisterActor(GetOwner());
}
