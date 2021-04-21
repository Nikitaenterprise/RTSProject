#include "Turret.h"

#include "RTSPlayerController.h"
#include "Ship.h"
#include "AnglesFunctions.h"
#include "HealthShield.h"
#include "RocketFactory.h"
#include "Rocket.h"

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

	HealthShieldComponent = CreateDefaultSubobject<UHealthShield>(TEXT("HealthShield"));

}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	//PlayerController = Cast<ARTSPlayerController>(GetWorld()->GetFirstPlayerController());
}

// Called every frame
void ATurret::Tick(float mainDeltaTime)
{
	Super::Tick(mainDeltaTime);
	DeltaTime = mainDeltaTime;
	PastTime += mainDeltaTime;
	if (HealthShieldComponent->IsDead()) Destroy(false, true);

	if (OwnerShip->bIsSelected)
	{
		SetFacingOnMouse();
		ShootRocket();
	}
	else
	{
		SetFacingLeftRight();
	}

	CheckAngle();
	RotateTurret();
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

void ATurret::ShootRocket()
{
	static bool flag = false;
	if (UKismetMathLibrary::Round(PastTime) % FireEveryThisSeconds == 0)
	{
		if (flag && UKismetMathLibrary::RandomFloat() > ChanceToFire)
		{
			// Fire rocket from arrow
			ARocket* SpawnedRocket = RocketFactory::NewRocket(this->GetWorld(), Arrow->K2_GetComponentToWorld());
			SpawnedRocket->OwnerTurret = this;
			flag = false;
		}
	}
	else flag = true;
}

void ATurret::SetFacingLeftRight()
{
	FVector Forward = GetActorForwardVector();
	FVector ShipRightVector = OwnerShip->GetActorRightVector();
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
	FRotator Rotator(0, 0, 0);
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

void ATurret::RotateTurret()
{
	FRotator Rotator = GetActorRotation() + Rotation;
	FRotator NewRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), Rotator, DeltaTime, RotationSpeed);
	SetActorRotation(NewRotation, ETeleportType::None);
}

void ATurret::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	AShip* Ship = Cast<AShip>(NewOwner);
	if(Ship) OwnerShip = Ship;
}

