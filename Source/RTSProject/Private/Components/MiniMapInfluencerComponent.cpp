#include "Components/MiniMapInfluencerComponent.h"

#include "UI/GameHUD.h"
#include "UI/MiniMapWidget.h"
#include "Core/RTSPlayerController.h"

UMiniMapInfluencerComponent::UMiniMapInfluencerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UMiniMapInfluencerComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	ARTSPlayerController* TestController = Cast<ARTSPlayerController>(GetOwner()->GetOwner());
	if (!IsValid(TestController))
	{
		UE_LOG(LogTemp, Error, TEXT("TestController is nullptr in UMiniMapInfluencerComponent::InitializeComponent()"));
		return;
	}
	if (!TestController->GameHUD)
	{
		UE_LOG(LogTemp, Error, TEXT("TestController->GameHUD is nullptr in UMiniMapInfluencerComponent::InitializeComponent()"));
		return;
	}
	if (!TestController->GameHUD->MiniMapWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("TestController->GameHUD->MiniMapWidget is nullptr in UMiniMapInfluencerComponent::InitializeComponent()"));
		return;
	}
	MiniMapWidget = TestController->GameHUD->MiniMapWidget;
	MiniMapWidget->RegisterActor(GetOwner());
}

void UMiniMapInfluencerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMiniMapInfluencerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMiniMapInfluencerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(IsValid(MiniMapWidget))
	{
		MiniMapWidget->UnRegisterActor(GetOwner());
	}
	Super::EndPlay(EndPlayReason);
}


