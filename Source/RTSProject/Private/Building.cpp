#include "Building.h"

#include "HealthShield.h"
#include "RTSPlayerController.h"
#include "HealthShieldBarHUD.h"

#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"

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
	
	HealthShieldComponent = CreateDefaultSubobject<UHealthShield>(TEXT("HealthShield"));

}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<ARTSPlayerController>(GetWorld()->GetFirstPlayerController());

	HealthShieldBarHUD = Cast<UHealthShieldBarHUD>(HealthShieldBar->GetWidget());
	//HealthShieldHUD = Cast<HealthShieldHUD>(HealthShieldBar->GetUserWidgetObject())
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
}

bool ABuilding::Destroy_Implementation(bool bNetForce, bool bShouldModifyLevel)
{
	PlayerController->PlayersActors.Remove(this);
	//SpawnEmitterAtLocation()
	return Super::Destroy(bNetForce, bShouldModifyLevel);
}

void ABuilding::SetHealthShieldBar()
{
	HealthShieldBarHUD->ShieldPercent = HealthShieldComponent->getShieldPercent();
	HealthShieldBarHUD->HealthPercent = HealthShieldComponent->getHealthPercent();
}

void ABuilding::BindHUD()
{
	if (PlayerController) {
		HealthShieldBarHUD = Cast<UHealthShieldBarHUD>(HealthShieldBar->GetWidget());
	}
}

void ABuilding::Selected_Implementation(bool _bIsSelected)
{
	bIsSelected = _bIsSelected;
	if (bIsSelected)
	{
		SetHealthShieldBar();
		HealthShieldBar->SetVisibility(true);
	}
	else
	{
		HealthShieldBar->SetVisibility(false);
	}
}

void ABuilding::Highlighted_Implementation(bool _bIsHighlighted)
{
	if (!bIsSelected)
	{
		bIsHighlighted = _bIsHighlighted;
		if (bIsHighlighted)
		{
			SetHealthShieldBar();
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
