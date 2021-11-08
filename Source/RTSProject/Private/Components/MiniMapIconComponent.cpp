#include "Components/MiniMapIconComponent.h"

#include "Actors/FogOfWar.h"
#include "Core/RTSPlayerController.h"
#include "Volumes/FogOfWarBoundsVolume.h"

UMiniMapIconComponent::UMiniMapIconComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UMiniMapIconComponent::InitializeComponent()
{
	ARTSPlayerController* TestController = Cast<ARTSPlayerController>(GetOwner()->GetOwner());
	if (!IsValid(TestController))
	{
		UE_LOG(LogTemp, Error, TEXT("TestController is nullptr in UMiniMapIconComponent::InitializeComponent()"));
		return;
	}
	if (!TestController->FogOfWar)
	{
		UE_LOG(LogTemp, Error, TEXT("TestController->FogOfWar is nullptr in UMiniMapIconComponent::InitializeComponent()"));
		return;
	}
	if (!TestController->FogOfWar->FOWBoundsVolume)
	{
		UE_LOG(LogTemp, Error, TEXT("TestController->FogOfWar->FOWBoundsVolume in UMiniMapIconComponent::InitializeComponent() is nullptr"));
		return;
	}
	FOWBoundsVolume = TestController->FogOfWar->FOWBoundsVolume;
}

void UMiniMapIconComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMiniMapIconComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UMiniMapIconComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}