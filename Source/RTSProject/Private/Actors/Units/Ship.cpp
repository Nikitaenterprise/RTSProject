#include "Actors/Units/Ship.h"

#include "AbilitySystemComponent.h"
#include "Actors/RTSPlayer.h"
#include "Actors/Units/Turret.h"
#include "AttributeSet.h"
#include "Components/AttackComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/FogOfWarInfluencerComponent.h"
#include "Components/HealthShieldWidgetComponent.h"
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
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());
	
	SelectionCircle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionCircle"));
	SelectionCircle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SelectionCircle->SetupAttachment(GetRootComponent());
	
	HealthShieldWidgetComponent = CreateDefaultSubobject<UHealthShieldWidgetComponent>(TEXT("HealthShieldWidgetComponent"));
	HealthShieldWidgetComponent->SetupAttachment(GetRootComponent());

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	AttackComponent = CreateDefaultSubobject<UAttackComponent>(TEXT("AttackComponent"));
	FOWInfluencerComponent = CreateDefaultSubobject<UFogOfWarInfluencerComponent>(TEXT("FOWInfluencerComponent"));
	MiniMapInfluencerComponent = CreateDefaultSubobject<UMiniMapInfluencerComponent>(TEXT("MiniMapInfluencerComponent"));
	MiniMapIconComponent = CreateDefaultSubobject<UMiniMapIconComponent>(TEXT("MiniMapIconComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ShipAttributeSet = CreateDefaultSubobject<UShipAttributeSet>(TEXT("ShipAttributeSet"));
	HealthShieldAttributeSet = CreateDefaultSubobject<UHealthShieldAttributeSet>(TEXT("HealthShieldAttributeSet"));
}

void AShip::PreInitializeComponents()
{
	PlayerController = Cast<ARTSPlayerController>(GetOwner());
	if (IsValid(PlayerController) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr in AShip::BeginPlay()"));
	}

	if (const auto* RTSPlayerController = Cast<AController>(GetOwner()))
	{
		if (const auto* PlayerPawn = RTSPlayerController->GetPawn())
		{
			RTSPlayerState = PlayerPawn->GetPlayerState<ARTSPlayerState>();
		}
	}

	MovementComponent = Cast<UPawnMovementComponent>(GetComponentByClass(UPawnMovementComponent::StaticClass()));
	CapsuleComponent = Cast<UCapsuleComponent>(GetComponentByClass(UCapsuleComponent::StaticClass()));
	Super::PreInitializeComponents();
}

void AShip::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->AddSet<UShipAttributeSet>();

	if (PlayerController)
	{
		DebugInputComponent = PlayerController->InputComponent;
		if (!IsValid(DebugInputComponent))
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("InputComponent in AShip->Initialize() is nullptr"));
			UE_LOG(LogTemp, Error, TEXT("InputComponent in AShip->Initialize() is nullptr"));
			return;
		}
		DebugInputComponent->BindAction(TEXT("MouseWheelYPositive"), IE_Pressed, this, &AShip::MouseYPositiveStart);
		DebugInputComponent->BindAction(TEXT("MouseWheelYPositive"), IE_Released, this, &AShip::MouseYPositiveEnd);
		DebugInputComponent->BindAction(TEXT("MouseWheelYNegative"), IE_Pressed, this, &AShip::MouseYNegativeStart);
		DebugInputComponent->BindAction(TEXT("MouseWheelYNegative"), IE_Released, this, &AShip::MouseYNegativeEnd);
		DebugInputComponent->BindAction(TEXT("LMB"), IE_Pressed, this, &AShip::LMBPressed);
		DebugInputComponent->BindAction(TEXT("LMB"), IE_Released, this, &AShip::LMBReleased);
	}
	

	if (HealthShieldWidgetComponent)
	{
		HealthShieldWidgetComponent->SetHealthShieldAttributeSet(HealthShieldAttributeSet);
	}
	SelectionCircle->SetVisibility(false);

	AbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(ShipAttributeSet);
	AbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(HealthShieldAttributeSet);
	if (IsValid(AttackAbility.Get()))
	{
		AttackAbilityHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AttackAbility, 1, INDEX_NONE, this));
	}
	HealthShieldAttributeSet->OnHealthZeroed.BindLambda([This = TWeakObjectPtr<ThisClass>(this)]()
	{
		if(This.IsValid())
		{
			This->Destroy();
		}
	});
}

void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerController)
	{
		if (bJustCreated && !bLMBPressed)
		{
		
			auto* PlayerPawn = Cast<ARTSPlayer>(PlayerController->GetPawn());
			if (PlayerPawn == nullptr)
			{
				return;
			}
		
			PlayerPawn->SetIsZoomDisabled(true);
			if (PlayerController->GetGameHUD()) PlayerController->GetGameHUD()->LockSelectionRectangle();
			UpdatePositionWhenCreated();
		}
		else if (bLMBPressed)
		{
			auto* PlayerPawn = Cast<ARTSPlayer>(PlayerController->GetPawn());
			if (PlayerPawn == nullptr)
			{
				return;
			}
		
			PlayerPawn->SetIsZoomDisabled(false);
			if (PlayerController->GetGameHUD()) PlayerController->GetGameHUD()->UnlockSelectionRectangle();
			bJustCreated = false;
		}	
	}

	if (MovementComponent)
	{
		bIsMoving = MovementComponent->Velocity.Size() > 0;
	}
	//if (bIsMoving && UKismetMathLibrary::NearlyEqual_FloatFloat(PastTime, DrawNavLineOncePerThisSeconds)) DrawNavLine();
}

void AShip::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (RTSPlayerState)
	{
		RTSPlayerState->RemoveFromPlayersUnits(this);
	}
	
	if (PlayerController)
	{
		PlayerController->RemoveFromSelectedActors(this);
	}
	Turrets.Empty();
	Super::EndPlay(EndPlayReason);
}

void AShip::Selected_Implementation(bool _bIsSelected)
{
	bIsSelected = _bIsSelected;
	if (bIsSelected)
	{
		HealthShieldWidgetComponent->SetVisibility(true);
		SelectionCircle->SetVisibility(true);
	}
	else
	{
		HealthShieldWidgetComponent->SetVisibility(false);
		SelectionCircle->SetVisibility(false);
	}
}

void AShip::Highlighted_Implementation(bool _bIsHighlighted)
{
	if (!bIsSelected)
	{
		bIsHighlighted = _bIsHighlighted;
		if (bIsHighlighted)
		{
			HealthShieldWidgetComponent->SetVisibility(true);
			SelectionCircle->SetVisibility(true);
		}
		else
		{
			HealthShieldWidgetComponent->SetVisibility(false);
			SelectionCircle->SetVisibility(false);
		}
	}
}

bool AShip::RequestMove(const FVector TargetLocation)
{
	if (!MovementComponent)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("MovementComponent in AShip->Move() is null"));
		UE_LOG(LogTemp, Error, TEXT("MovementComponent in AShip->Move() is null"));
		return false;
	}
	//MovementComponent->MoveTo(TargetLocation);
	//DrawNavLine();
	return true;
}

void AShip::DrawNavLine()
{
	if (NavPathCoords.Num() < 2) return;
	
	for (size_t i = 0; i < NavPathCoords.Num() - 1; i++)
	{
		DrawDebugLine(GetWorld(),
			NavPathCoords[i],
			NavPathCoords[i + 1],
			FColor::Red,
			false,
			DrawNavLineOncePerThisSeconds,
			0,
			5);
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

void AShip::RotateWhenCreatedPositive()
{
	if (!bJustCreated) return;
	SetActorRotation(FRotator(0, GetActorRotation().Yaw - 10, 0));
	GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::White, GetActorForwardVector().Rotation().ToString());
}

void AShip::RotateWhenCreatedNegative()
{
	if (!bJustCreated) return;
	SetActorRotation(FRotator(0, GetActorRotation().Yaw + 10, 0));
	GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::White, GetActorForwardVector().Rotation().ToString());
}

void AShip::MouseYPositiveStart()
{
	bMouseWheelYPositive = true;
	RotateWhenCreatedPositive();
}

void AShip::MouseYPositiveEnd()
{
	bMouseWheelYPositive = false;
}

void AShip::MouseYNegativeStart()
{
	bMouseWheelYNegative = true;
	RotateWhenCreatedNegative();
}

void AShip::MouseYNegativeEnd()
{
	bMouseWheelYNegative = false;
}

void AShip::LMBPressed()
{
	bLMBPressed = true;
}

void AShip::LMBReleased()
{
	bLMBPressed = false;
}