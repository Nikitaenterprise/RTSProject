#include "Turret.h"

#include "RTSPlayerController.h"
#include "Ship.h"
#include "AnglesFunctions.h"
#include "HealthShieldComponent.h"
#include "RocketFactory.h"

#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"


ATurret::ATurret()
{
 	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(GetRootComponent());
	
	HealthShieldComponent = CreateDefaultSubobject<UHealthShieldComponent>(TEXT("HealthShieldComponent"));
}

void ATurret::BeginPlay()
{
	Super::BeginPlay();
	FiredRockets.Reserve(20);
	// Binding Destroy() function that will destroy rocket
	// when timer hits MaxLifeTime
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("Fire"));
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, FireEveryThisSeconds, false);
}

void ATurret::Initialize(ARTSPlayerController* RTSController, AShip* Ship)
{
	if (RTSController)
	{
		PlayerController = RTSController;
	}
	if (Ship)
	{
		OwnerShip = Ship;
	}
	
	// Decide on which side the turret is
	const FVector ShipForward = OwnerShip->GetActorForwardVector();
	const FVector FromCenterOfShipToTurret = GetActorLocation() - OwnerShip->GetActorLocation();
	const bool bClockwise = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(ShipForward, FromCenterOfShipToTurret);
	if (bClockwise) OnWhichSide = ESide::Right;
	if (!bClockwise) OnWhichSide = ESide::Left;
	
	SetFacingLeftRight();

	OwnerShip->bHasWorkingTurrets = true;
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HealthShieldComponent->IsDead()) Destroy(false, true);

	if (OwnerShip->bIsSelected)
	{
		SetFacingOnMouse();
		Fire();
	}
	else
	{
		SetFacingLeftRight();
	}

	CheckAngle();
	RotateTurret(DeltaTime);
}


bool ATurret::Destroy_Implementation(bool bNetForce, bool bShouldModifyLevel)
{
	OwnerShip->Turrets.Remove(this);
	// If ship has no working turrets then set bool variable to false
	if (OwnerShip->Turrets.Num() == 0) OwnerShip->bHasWorkingTurrets = false;
	//SpawnEmitterAtLocation()
	return Super::Destroy(bNetForce, bShouldModifyLevel);
}

void ATurret::SetFacingOnMouse()
{
	FHitResult Hit;
	PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Camera), false, Hit);
	const FVector VectorFromTurretToCursor = Hit.Location - GetActorLocation();
	const FRotator Rotator = AnglesFunctions::FindYawRotatorOn2D(GetActorForwardVector(), VectorFromTurretToCursor);
	Rotation = FRotator(0, Rotator.Yaw, 0);
}

void ATurret::Fire()
{
	if (UKismetMathLibrary::RandomFloat() > ChanceToFire)
	{
		// Fire rocket from arrow
		ARocket* SpawnedRocket = RocketFactory::NewRocket(GetWorld(), PlayerController, this, Arrow->K2_GetComponentToWorld());
		//FiredRockets.Add(SpawnedRocket);
	}
}

void ATurret::SetFacingLeftRight()
{
	const FVector Forward = GetActorForwardVector();
	const FVector ShipRightVector = OwnerShip->GetActorRightVector();
	FRotator Rotator(0, 0, 0);

	if (OnWhichSide == ESide::Right)
	{
		Rotator = AnglesFunctions::FindYawRotatorOn2D(Forward, ShipRightVector);
	}
	if (OnWhichSide == ESide::Left)
	{
		Rotator = AnglesFunctions::FindYawRotatorOn2D(Forward, -1 * ShipRightVector);
	}
	Rotation = FRotator(0, Rotator.Yaw, 0);
}

void ATurret::CheckAngle()
{
	float Angle = 0;
	FRotator Rotator;
	if (OnWhichSide == ESide::Right)
	{
		Rotator = AnglesFunctions::FindYawRotatorOn2D(GetActorForwardVector(), OwnerShip->GetActorRightVector());
		Angle = -1 * Rotator.Yaw;
		
	}
	if (OnWhichSide == ESide::Left)
	{
		Rotator = AnglesFunctions::FindYawRotatorOn2D(GetActorForwardVector(), -1 * OwnerShip->GetActorRightVector());
		Angle = -1 * Rotator.Yaw;
	}

	if (Angle > 0 && Angle > MaxAngleDeviation) Rotation = FRotator(0, -10, 0);
	if (Angle < 0 && Angle < -1 * MaxAngleDeviation) Rotation = FRotator(0, 10, 0);
}

void ATurret::RotateTurret(float DeltaTime)
{
	const FRotator Rotator = GetActorRotation() + Rotation;
	const FRotator NewRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), Rotator, DeltaTime, RotationSpeed);
	SetActorRotation(NewRotation, ETeleportType::None);
}
