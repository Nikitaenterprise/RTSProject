#include "Building.h"

#include "HealthShield.h"
#include "RTSPlayerController.h"
#include "HealthShieldBarHUD.h"
#include "ShipFactory.h"
#include "Ship.h"

#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Particles/ParticleSystemComponent.h"

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

	SpawnPoint = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SpawnPoint"));
	
	
	HealthShieldComponent = CreateDefaultSubobject<UHealthShield>(TEXT("HealthShield"));

}

void ABuilding::BeginPlay()
{
	Super::BeginPlay();
}

void ABuilding::Initialize(ARTSPlayerController* RTSController)
{
	if (RTSController)
	{
		PlayerController = RTSController;
		HealthShieldBarHUD = Cast<UHealthShieldBarHUD>(HealthShieldBar->GetWidget());
		HealthShieldBarHUD->BindHealthShieldValues(HealthShieldComponent->GetHealthPercentPtr(), HealthShieldComponent->GetShieldPercentPtr());
		HealthShieldBar->SetVisibility(false);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("PlayerController in AShip->Init() is null"));
	}
	SpawnPoint->SetRelativeLocation(FVector(150, 0, 0));
	SpawnPoint->SetVisibility(false);
}

void ABuilding::Tick(float MainDeltaTime)
{
	Super::Tick(MainDeltaTime);
	DeltaTime = MainDeltaTime;
	PastTime += MainDeltaTime;
	if (HealthShieldComponent->IsDead()) Destroy(false, true);

	if (bJustCreated && !PlayerController->bLMBPressed)
	{
		UpdatePositionWhenCreated();
	}
	else if (PlayerController->bLMBPressed)
	{
		bJustCreated = false;
	}
	BuildUnit();
}

bool ABuilding::Destroy_Implementation(bool bNetForce, bool bShouldModifyLevel)
{
	PlayerController->PlayersActors.Remove(this);
	//SpawnEmitterAtLocation()
	return Super::Destroy(bNetForce, bShouldModifyLevel);
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

void ABuilding::BuildUnit()
{
	if (BuildingQueue.Num() == 0) return;
	const TSubclassOf<AActor> Subclass = BuildingQueue.Pop();
	UClass* ClassType = Subclass.Get();
	const FVector SpawnLocation = SpawnPoint->GetComponentLocation();
	
	AShip* SpawnedShip = ShipFactory::NewShip(GetWorld(), ClassType, PlayerController, SpawnLocation);
	ShipFactory::AddTurretsToShip(SpawnedShip);
}

void ABuilding::AddActorToBuildingQueue(TSubclassOf<AActor> Actor)
{
	BuildingQueue.Add(Actor);
}
