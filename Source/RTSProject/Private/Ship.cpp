#include "Ship.h"

#include "RTSPlayerController.h"
#include "HealthShield.h"
#include "Turret.h"
#include "HealthShieldBarHUD.h"
#include "ShipMovementComponent.h"
#include "Camera.h"

#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/InputComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"


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
	
	SelectionCircle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionCircle"));
	SelectionCircle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SelectionCircle->SetupAttachment(GetRootComponent());
	
	HealthShieldBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthShieldBar"));
	HealthShieldBar->SetDrawSize(FVector2D(150, 150));
	HealthShieldBar->SetWidgetSpace(EWidgetSpace::Screen);
	HealthShieldBar->SetupAttachment(GetRootComponent());
	
	HealthShieldComponent = CreateDefaultSubobject<UHealthShield>(TEXT("HealthShield"));

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));

	MovementComponent = CreateDefaultSubobject<UShipMovementComponent>(TEXT("ShipMovementComponent"));
}

void AShip::BeginPlay()
{
	Super::BeginPlay();
}

void AShip::Tick(float _mainDeltaTime)
{
	Super::Tick(_mainDeltaTime);
	
	DeltaTime = _mainDeltaTime;
	PastTime += _mainDeltaTime;
	if (HealthShieldComponent->IsDead()) Destroy(false, true);

	if (bJustCreated && !PlayerController->bLMBPressed)
	{
		PlayerController->CameraRef->bDisableZooming = true;
		UpdatePositionWhenCreated();
	}
	else if (PlayerController->bLMBPressed)
	{
		PlayerController->CameraRef->bDisableZooming = false;
		bJustCreated = false;
	}

	bIsMoving = MovementComponent->Velocity.Size() > 0;
	if (bIsMoving && UKismetMathLibrary::NearlyEqual_FloatFloat(PastTime, DrawNavLineOncePerThisSeconds)) DrawNavLine();
	
}

void AShip::Initialize(ARTSPlayerController* RTSController)
{
	if (RTSController)
	{
		PlayerController = RTSController;

		if(MovementComponent)
		{
			MovementComponent->PlayerController = RTSController;
			MovementComponent->OwnerShip = this;
			MovementComponent->Initialize();	
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("MovementComponent in AShip->Init() is null"));
		}
		InputComponent = PlayerController->InputComponent;
		if (InputComponent)
		{
			InputComponent->BindAction(TEXT("MouseWheelYPositive"), IE_Pressed, this, &AShip::MouseYPositiveStart);
			InputComponent->BindAction(TEXT("MouseWheelYPositive"), IE_Released, this, &AShip::MouseYPositiveEnd);
			InputComponent->BindAction(TEXT("MouseWheelYNegative"), IE_Pressed, this, &AShip::MouseYNegativeStart);
			InputComponent->BindAction(TEXT("MouseWheelYNegative"), IE_Released, this, &AShip::MouseYNegativeEnd);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("InputComponent in AShip->Init() is null"));
		}
		
		HealthShieldBarHUD = Cast<UHealthShieldBarHUD>(HealthShieldBar->GetWidget());
		HealthShieldBarHUD->BindHealthShieldValues(HealthShieldComponent->GetHealthPercentPtr(), HealthShieldComponent->GetShieldPercentPtr());
		HealthShieldBar->SetVisibility(false);
		SelectionCircle->SetVisibility(false);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("PlayerController in AShip->Init() is null"));
	}
	
}

bool AShip::Destroy_Implementation(bool bNetForce, bool bShouldModifyLevel)
{
	PlayerController->PlayersActors.Remove(this);
	PlayerController->SelectedActors.Remove(this);
	if (bHasWorkingTurrets)
	{
		for (auto& Turret : Turrets) Turret->Destroy();
	}
	return Super::Destroy(bNetForce, bShouldModifyLevel);
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

bool AShip::Move(const FVector _TargetLocation)
{
	if (!MovementComponent)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("MovementComponent in AShip->Move() is null"));
		return false;
	}
	const bool bSuccessful = MovementComponent->RequestNavMoving(_TargetLocation);
	if (!bSuccessful) return false;
	NavPathCoords = MovementComponent->GetNavPathCoords();
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


