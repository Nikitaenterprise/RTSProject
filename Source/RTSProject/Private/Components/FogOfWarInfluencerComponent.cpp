#include "Components/FogOfWarInfluencerComponent.h"

#include "Actors/FogOfWar.h"
#include "Core/RTSPlayerController.h"

UFogOfWarInfluencerComponent::UFogOfWarInfluencerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	// SightRadius can't be negative
	if (SightRadius < 0) SightRadius *= -1;
}

void UFogOfWarInfluencerComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	ARTSPlayerController* TestController = Cast<ARTSPlayerController>(GetOwner()->GetOwner());
	if (!IsValid(TestController))
	{
		UE_LOG(LogTemp, Error, TEXT("TestController is nullptr in UFogOfWarInfluencerComponent::InitializeComponent()"));
		return;
	}
	if (!IsValid(TestController->GetFogOfWar()))
	{
		UE_LOG(LogTemp, Error, TEXT("TestController->FogOfWar is nullptr in UFogOfWarInfluencerComponent::InitializeComponent()"));
		return;
	}
	FOW = TestController->GetFogOfWar();
}

void UFogOfWarInfluencerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!HasBeenInitialized())
	{
		UE_LOG(LogTemp, Warning, TEXT("UFogOfWarInfluencerComponent::BeginPlay() component hasn't been initialized"));
		return;
	}
	if (!IsValid(FOW))
	{
		UE_LOG(LogTemp, Error, TEXT("FOW is nullptr in UFogOfWarInfluencerComponent::BeginPlay()"));
		return;
	}
	FOW->RegisterActor(GetOwner());
}

void UFogOfWarInfluencerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UFogOfWarInfluencerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(FOW))
	{
		FOW->UnRegisterActor(GetOwner());
	}
	Super::EndPlay(EndPlayReason);
}



