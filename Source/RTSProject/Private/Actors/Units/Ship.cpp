#include "Actors/Units/Ship.h"

#include "AbilitySystemComponent.h"
#include "Actors/RTSPlayer.h"
#include "Actors/Units/Turret.h"
#include "AttributeSet.h"
#include "Components/AttackComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/FogOfWarInfluencerComponent.h"
#include "Components/InputComponent.h"
#include "Components/MiniMapIconComponent.h"
#include "Components/MiniMapInfluencerComponent.h"
#include "Components/Movement/ShipMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/RTSPlayerController.h"
#include "DrawDebugHelpers.h"
#include "GAS/AttackAbility.h"
#include "GAS/HealthShieldAttributeSet.h"
#include "GAS/ShipAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/PawnSensingComponent.h"
#include "Systems/RTSPlayerState.h"
#include "UI/GameHUD.h"

AShip::AShip(const FObjectInitializer& OI) : Super(OI)
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());
	
	SelectionCircle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionCircle"));
	SelectionCircle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SelectionCircle->SetupAttachment(GetRootComponent());

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	AttackComponent = CreateDefaultSubobject<UAttackComponent>(TEXT("AttackComponent"));
	FOWInfluencerComponent = CreateDefaultSubobject<UFogOfWarInfluencerComponent>(TEXT("FOWInfluencerComponent"));
	MiniMapInfluencerComponent = CreateDefaultSubobject<UMiniMapInfluencerComponent>(TEXT("MiniMapInfluencerComponent"));
	MiniMapIconComponent = CreateDefaultSubobject<UMiniMapIconComponent>(TEXT("MiniMapIconComponent"));
}

void AShip::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	MovementComponent = Cast<UPawnMovementComponent>(GetComponentByClass(UPawnMovementComponent::StaticClass()));
	CapsuleComponent = Cast<UCapsuleComponent>(GetComponentByClass(UCapsuleComponent::StaticClass()));
}

void AShip::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->AddSet<UShipAttributeSet>();

	if (PlayerController)
	{
		//DebugInputComponent->BindAction(TEXT("MouseWheelYPositive"), IE_Pressed, this, &AShip::MouseYPositiveStart);
		//DebugInputComponent->BindAction(TEXT("MouseWheelYNegative"), IE_Pressed, this, &AShip::MouseYNegativeStart);
		PlayerController->OnLeftMouseButtonClicked.AddUniqueDynamic(this, &ThisClass::LMBPressed);
	}
	
	
	SelectionCircle->SetVisibility(false);

	if (RTSPlayer)
	{
		RTSPlayer->SetIsZoomDisabled(true);
	}
	
	if (PlayerController->GetGameHUD())
	{
		PlayerController->GetGameHUD()->LockSelectionRectangle();
	}
}

void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShip::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Turrets.Empty();
	
	Super::EndPlay(EndPlayReason);
}

void AShip::Selected_Implementation(bool bInIsSelected)
{
	Super::Selected_Implementation(bInIsSelected);
	if (bIsSelected)
	{
		SelectionCircle->SetVisibility(true);
	}
	else
	{
		SelectionCircle->SetVisibility(false);
	}
}

void AShip::Highlighted_Implementation(bool bInIsHighlighted)
{
	Super::Highlighted_Implementation(bInIsHighlighted);
	if (!bIsSelected)
	{
		if (bIsHighlighted)
		{
			SelectionCircle->SetVisibility(true);
		}
		else
		{
			SelectionCircle->SetVisibility(false);
		}
	}
}

void AShip::UpdatePositionWhenCreated()
{
	/*FHitResult Hit;
	const bool bHit = PlayerController->GetHitResultUnderCursorByChannel(
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
		false,
		Hit);

	FVector Location = FVector(0,0, 150);
	
	
	if (bHit) Location = Hit.Location + Location;

	SetActorLocation(Location, false, nullptr, ETeleportType::None);*/
}

void AShip::MouseYPositiveStart()
{
	if (bJustCreated == false)
	{
		return;
	}
	SetActorRotation(FRotator(0, GetActorRotation().Yaw - 10, 0));
	GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::White, GetActorForwardVector().Rotation().ToString());
}

void AShip::MouseYNegativeStart()
{
	if (bJustCreated == false)
	{
		return;
	}
	SetActorRotation(FRotator(0, GetActorRotation().Yaw + 10, 0));
	GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::White, GetActorForwardVector().Rotation().ToString());
}

void AShip::LMBPressed(ETriggerEvent Event)
{
	if (RTSPlayer)
	{
		RTSPlayer->SetIsZoomDisabled(false);
	}
	
	if (PlayerController->GetGameHUD())
	{
		PlayerController->GetGameHUD()->UnlockSelectionRectangle();
		PlayerController->OnLeftMouseButtonClicked.RemoveDynamic(this, &ThisClass::LMBPressed);
	}
	
	bJustCreated = false;
}