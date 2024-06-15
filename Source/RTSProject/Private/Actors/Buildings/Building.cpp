#include "Actors/Buildings/Building.h"

#include "AbilitySystemComponent.h"
#include "Miscellaneous/FactoriesFunctionLibrary.h"
#include "Components/FogOfWarInfluencerComponent.h"
#include "Core/RTSPlayerController.h"
#include "UI/HealthShieldWidget.h"
#include "Components/MiniMapIconComponent.h"
#include "Components/MiniMapInfluencerComponent.h"
#include "Actors/Units/Ship.h"
#include "UI/GameHUD.h"
#include "UI/SelectionRectangleWidget.h"
#include "AttributeSet.h"
#include "Components/StaticMeshComponent.h"
#include "Components/UnitIndicatorComponent.h"
#include "Components/WidgetComponent.h"
#include "GAS/BuildingAttributeSet.h"
#include "GAS/BuildUnitAbility.h"
#include "GAS/HealthShieldAttributeSet.h"
#include "Systems/RTSPlayerState.h"
#include "UI/SelectionRectangleWidget.h"

ABuilding::ABuilding(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());

	FOWInfluencerComponent = CreateDefaultSubobject<UFogOfWarInfluencerComponent>(TEXT("FOWInfluencerComponent"));
	SpawnPoint = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SpawnPoint"));
	MiniMapInfluencerComponent = CreateDefaultSubobject<UMiniMapInfluencerComponent>(TEXT("MiniMapInfluencerComponent"));
	MiniMapIconComponent = CreateDefaultSubobject<UMiniMapIconComponent>(TEXT("MiniMapIconComponent"));
}

void ABuilding::BeginPlay()
{
	Super::BeginPlay();

	bJustCreated = HasAnyFlags(RF_WasLoaded) == false;
	PlayerController->OnLeftMouseButtonClicked.AddUniqueDynamic(this, &ThisClass::LMBPressed);

	SpawnPoint->SetRelativeLocation(FVector(150, 0, 0));
	SpawnPoint->SetVisibility(false);
	
	BuildingUnitHandle = GetAbilityByClass<UBuildUnitAbility>();
}

void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAnyFlags(RF_WasLoaded) == false)
	{
		if (bJustCreated && !bLMBPressed)
		{
			if (PlayerController->GetGameHUD()) PlayerController->GetGameHUD()->LockSelectionRectangle();
			UpdatePositionWhenCreated();
		}
		else if (bLMBPressed)
		{
			bJustCreated = false;
			if (PlayerController->GetGameHUD()) PlayerController->GetGameHUD()->UnlockSelectionRectangle();
			PlayerController->OnLeftMouseButtonClicked.RemoveDynamic(this, &ThisClass::LMBPressed);
		}
	}
}

void ABuilding::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//SpawnEmitterAtLocation();
	
	Super::EndPlay(EndPlayReason);
}

void ABuilding::LMBPressed(ETriggerEvent Event)
{
	bLMBPressed = true;
}

void ABuilding::UpdatePositionWhenCreated()
{
	FHitResult Hit;
	const bool bHit = PlayerController->GetHitResultUnderCursorByChannel(
				UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), 
				false,
				Hit);

	FVector Location;
	if (bHit) Location = Hit.Location;
	else Location = FVector(0, 0, 0);
	// Clamp to ground
	Location.Z = 0;
	
	SetActorLocation(Location, false, nullptr, ETeleportType::None);
}

int ABuilding::GetBuildingQueueSizeByClass(TSubclassOf<AActor> ActorClass) const
{
	if (BuildingQueue.Num() == 0) return 0;
	int NumberOfActorsToBuild = 0;
	for(auto& a : BuildingQueue)
	{
		if (a == ActorClass) NumberOfActorsToBuild++;
	}
	return NumberOfActorsToBuild;
}

void ABuilding::RequestBuildingUnit(TSubclassOf<AActor> ActorClass)
{
	BuildingQueue.Add(ActorClass);
	if (BuildingUnitHandle.IsValid())
	{
		if (!bIsBuildingUnit)
		{
			AbilitySystemComponent->TryActivateAbility(BuildingUnitHandle, false);
		}
	}
}