#include "Actors/Building.h"

#include "Miscellaneous/FactoriesFunctionLibrary.h"
#include "Components/FogOfWarInfluencerComponent.h"
#include "Core/RTSPlayerController.h"
#include "UI/HealthShieldBarHUD.h"
#include "Components/HealthShieldComponent.h"
#include "Components/MiniMapIconComponent.h"
#include "Components/MiniMapInfluencerComponent.h"
#include "Actors/Ship.h"
#include "UI/GameHUD.h"
#include "UI/SelectionRectangleWidget.h"

#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "UI/SelectionRectangleWidget.h"

ABuilding::ABuilding()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());

	HealthShieldBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthShieldBar"));
	HealthShieldBar->SetupAttachment(GetRootComponent());
	HealthShieldBar->SetDrawSize(FVector2D(150, 150));
	HealthShieldBar->SetWidgetSpace(EWidgetSpace::Screen);

	FOWInfluencerComponent = CreateDefaultSubobject<UFogOfWarInfluencerComponent>(TEXT("FOWInfluencerComponent"));
	
	SpawnPoint = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SpawnPoint"));
	
	HealthShieldComponent = CreateDefaultSubobject<UHealthShieldComponent>(TEXT("HealthShieldComponent"));

	MiniMapInfluencerComponent = CreateDefaultSubobject<UMiniMapInfluencerComponent>(TEXT("MiniMapInfluencerComponent"));
	MiniMapIconComponent = CreateDefaultSubobject<UMiniMapIconComponent>(TEXT("MiniMapIconComponent"));
}

void ABuilding::BeginPlay()
{
	Super::BeginPlay();

	ARTSPlayerController* TestController = Cast<ARTSPlayerController>(GetOwner());
	if (!IsValid(TestController))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr in ABuilding::BeginPlay()"));
		return;
	}
	PlayerController = TestController;

	HealthShieldBarHUD = Cast<UHealthShieldBarHUD>(HealthShieldBar->GetWidget());
	HealthShieldBarHUD->BindHealthShieldValues(HealthShieldComponent->GetHealthPercentPtr(), HealthShieldComponent->GetShieldPercentPtr());
	HealthShieldBar->SetVisibility(false);

	DebugInputComponent = PlayerController->InputComponent;
	if (!IsValid(DebugInputComponent))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("InputComponent is nullptr in AShip::Initialize()"));
		UE_LOG(LogTemp, Error, TEXT("InputComponent is nullptr in AShip::Initialize()"));
	}
	DebugInputComponent->BindAction(TEXT("LMB"), IE_Pressed, this, &ABuilding::LMBPressed);
	DebugInputComponent->BindAction(TEXT("LMB"), IE_Released, this, &ABuilding::LMBReleased);

	SpawnPoint->SetRelativeLocation(FVector(150, 0, 0));
	SpawnPoint->SetVisibility(false);
}

void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HealthShieldComponent->IsDead())
	{
		Destroy();
	}

	if (bJustCreated && !bLMBPressed)
	{
		if (PlayerController->GameHUD) PlayerController->GameHUD->LockSelectionRectangle();
		UpdatePositionWhenCreated();
	}
	else if (bLMBPressed)
	{
		bJustCreated = false;
		if (PlayerController->GameHUD) PlayerController->GameHUD->UnlockSelectionRectangle();
	}
	if (ConstructionState == EConstructionState::RequestedConstruction) StartBuildingUnit();

}

void ABuilding::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	PlayerController->PlayersActors.Remove(this);
	//SpawnEmitterAtLocation()
	Super::EndPlay(EndPlayReason);
}

void ABuilding::LMBPressed()
{
	bLMBPressed = true;
}

void ABuilding::LMBReleased()
{
	bLMBPressed = false;
}

void ABuilding::SetSpawnPointLocation(const FVector& Location) const
{
	SpawnPoint->SetWorldLocation(Location);
}

void ABuilding::Selected_Implementation(bool _bIsSelected)
{
	bIsSelected = _bIsSelected;
	if (bIsSelected)
	{
		HealthShieldBar->SetVisibility(true);
		SpawnPoint->SetVisibility(true);
	}
	else
	{
		HealthShieldBar->SetVisibility(false);
		SpawnPoint->SetVisibility(false);
	}
}

void ABuilding::Highlighted_Implementation(bool _bIsHighlighted)
{
	if (!bIsSelected)
	{
		bIsHighlighted = _bIsHighlighted;
		if (bIsHighlighted)
		{
			HealthShieldBar->SetVisibility(true);
		}
		else
		{
			HealthShieldBar->SetVisibility(false);
		}
	}
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
	ConstructionState = EConstructionState::RequestedConstruction;
}

void ABuilding::StartBuildingUnit()
{
	if (BuildingQueue.Num() == 0) return;
	ConstructionState = EConstructionState::Constructing;
	float BaseTimeToBuild = 1;
	// Binding BuildUnit() function that will spawn ship in SpawnLocation
	// when timer hits BaseTimeToBuild
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("BuildUnit"));
	// Start timer
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, BaseTimeToBuild, false);
}

void ABuilding::BuildUnit()
{
	const TSubclassOf<AActor> Subclass = BuildingQueue.Pop();
	UClass* ClassType = Subclass.Get();
	// Add height to spawn location
	const FVector SpawnLocation = SpawnPoint->GetComponentLocation() + FVector(0, 0, 100);
	// First the ship is created in a place outside the borders
	AShip* SpawnedShip = UFactoriesFunctionLibrary::NewShip(GetWorld(), ClassType, PlayerController, SpawnLocation);
	if (IsValid(SpawnedShip))
	{
		UFactoriesFunctionLibrary::AddTurretsToShip(SpawnedShip);
	}
	FinishBuildingUnit();
}

void ABuilding::FinishBuildingUnit()
{
	// Check if there are more units to build
	if (BuildingQueue.Num() != 0) ConstructionState = EConstructionState::RequestedConstruction;
	else ConstructionState = EConstructionState::NotConstructing;
}

