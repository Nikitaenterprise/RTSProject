#include "Actors/Ship.h"

#include "Components/AttackComponent.h"
#include "Core/RTSPlayerController.h"
#include "Actors/Turret.h"
#include "UI/HealthShieldBarHUD.h"
#include "Components/ShipMovementComponent.h"
#include "Actors/Camera.h"
#include "Components/FogOfWarInfluencerComponent.h"
#include "Components/HealthShieldComponent.h"
#include "AbilitySystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/InputComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "GAS/ShipAttributeSet.h"
#include "Components/MiniMapIconComponent.h"
#include "Components/MiniMapInfluencerComponent.h"
#include "UI/GameHUD.h"


AShip::AShip(const FObjectInitializer& OI)
	: Super(OI.SetDefaultSubobjectClass<UShipMovementComponent>(FName("ShipMovementComponent")))
{
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetRelativeRotation(FRotator(0, 90, 0));
	CapsuleComponent->SetupAttachment(GetRootComponent());
	CapsuleComponent->bDynamicObstacle = true;
	
	SelectionCircle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionCircle"));
	SelectionCircle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SelectionCircle->SetupAttachment(GetRootComponent());
	
	HealthShieldBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthShieldBar"));
	HealthShieldBar->SetDrawSize(FVector2D(150, 150));
	HealthShieldBar->SetWidgetSpace(EWidgetSpace::Screen);
	HealthShieldBar->SetupAttachment(GetRootComponent());
	

	HealthShieldComponent = CreateDefaultSubobject<UHealthShieldComponent>(TEXT("HealthShieldComponent"));
	
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));

	MovementComponent = CreateDefaultSubobject<UShipMovementComponent>(TEXT("ShipMovementComponent"));

	AttackComponent = CreateDefaultSubobject<UAttackComponent>(TEXT("AttackComponent"));

	FOWInfluencerComponent = CreateDefaultSubobject<UFogOfWarInfluencerComponent>(TEXT("FOWInfluencerComponent"));

	MiniMapInfluencerComponent = CreateDefaultSubobject<UMiniMapInfluencerComponent>(TEXT("MiniMapInfluencerComponent"));

	MiniMapIconComponent = CreateDefaultSubobject<UMiniMapIconComponent>(TEXT("MiniMapIconComponent"));

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	ShipAttributeSet = CreateDefaultSubobject<UShipAttributeSet>(TEXT("ShipAttributeSet"));
	
}

void AShip::PreInitializeComponents()
{
	ARTSPlayerController* TestController = Cast<ARTSPlayerController>(GetOwner());
	if (!IsValid(TestController))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr in AShip::BeginPlay()"));
		return;
	}
	PlayerController = TestController;
	Super::PreInitializeComponents();
}

void AShip::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->AddSet<UShipAttributeSet>();
	
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

	HealthShieldBar->InitWidget();
	HealthShieldBarHUD = Cast<UHealthShieldBarHUD>(HealthShieldBar->GetWidget());
	if (!IsValid(HealthShieldBarHUD))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("HealthShieldBarHUD in AShip->Initialize() is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("HealthShieldBarHUD in AShip->Initialize() is nullptr"));
		return;
	}
	HealthShieldBarHUD->BindHealthShieldValues(HealthShieldComponent->GetHealthPercentPtr(), HealthShieldComponent->GetShieldPercentPtr());
	HealthShieldBar->SetVisibility(false);
	SelectionCircle->SetVisibility(false);
}

void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HealthShieldComponent->IsDead()) Destroy(false, true);

	if (bJustCreated && !bLMBPressed)
	{
		PlayerController->CameraRef->bDisableZooming = true;
		if (PlayerController->GameHUD) PlayerController->GameHUD->LockSelectionRectangle();
		UpdatePositionWhenCreated();
	}
	else if (bLMBPressed)
	{
		PlayerController->CameraRef->bDisableZooming = false;
		if (PlayerController->GameHUD) PlayerController->GameHUD->UnlockSelectionRectangle();
		bJustCreated = false;
	}

	bIsMoving = MovementComponent->Velocity.Size() > 0;
	//if (bIsMoving && UKismetMathLibrary::NearlyEqual_FloatFloat(PastTime, DrawNavLineOncePerThisSeconds)) DrawNavLine();
}

void AShip::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	PlayerController->PlayersActors.Remove(this);
	PlayerController->SelectedActors.Remove(this);
	Turrets.Empty();
	Super::EndPlay(EndPlayReason);
}

void AShip::Selected_Implementation(bool _bIsSelected)
{
	bIsSelected = _bIsSelected;
	if (bIsSelected)
	{
		HealthShieldBar->SetVisibility(true);
		SelectionCircle->SetVisibility(true);
	}
	else
	{
		HealthShieldBar->SetVisibility(false);
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
			HealthShieldBar->SetVisibility(true);
			SelectionCircle->SetVisibility(true);
		}
		else
		{
			HealthShieldBar->SetVisibility(false);
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
	if (!MovementComponent->RequestNavMoving(TargetLocation)) return false;
	NavPathCoords = MovementComponent->GetNavPathCoords();
	//DrawNavLine();
	return true;
}

void AShip::RequestAttack(const AActor* ActorToAttack)
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, FString::Printf(TEXT("Attacking %s"), *ActorToAttack->GetName()));
	for(const auto Turret : Turrets)
	{
		Turret->RequestAttack(ActorToAttack);
	}
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