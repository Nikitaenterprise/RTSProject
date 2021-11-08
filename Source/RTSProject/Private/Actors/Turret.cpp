#include "Actors/Turret.h"

#include "Core/RTSPlayerController.h"
#include "Actors/Ship.h"
#include "Miscellaneous/AnglesFunctions.h"
#include "Core/FactoryAssets.h"
#include "Components/HealthShieldComponent.h"
#include "Actors/Rocket.h"

#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "Miscellaneous/FactoriesFunctionLibrary.h"


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

	AShip* TestOwner = Cast<AShip>(GetOwner());
	if (!IsValid(TestOwner))
	{
		UE_LOG(LogTemp, Error, TEXT("TestOwner is nullptr in ATurret::BeginPlay()"));
		return;
	}
	OwnerShip = TestOwner;

	if (!IsValid(OwnerShip->PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr in ATurret::BeginPlay()"));
		return;
	}
	PlayerController = OwnerShip->PlayerController;

	// Decide on which side the turret is
	const FVector ShipForward = OwnerShip->GetActorForwardVector();
	const FVector FromCenterOfShipToTurret = GetActorLocation() - OwnerShip->GetActorLocation();
	const bool bClockwise = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(ShipForward, FromCenterOfShipToTurret);
	if (bClockwise) OnWhichSide = ESide::Right;
	if (!bClockwise) OnWhichSide = ESide::Left;

	SetFacingLeftRight();

	OwnerShip->bHasWorkingTurrets = true;

	// If no rocket is set in turret blueprint then set first from FactoryAssets
	if (RocketClass.Get() == nullptr)
	{
		if (PlayerController->GetFactoryAssets())
		{
			if (PlayerController->GetFactoryAssets()->GetRocketClass(0))
			{
				RocketClass = PlayerController->GetFactoryAssets()->GetRocketClass(0);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("RocketClass in FactoryAssets is empty"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FactoryAssets in PlayerController is nullptr"));
		}
	}
	OwnerShip->Turrets.AddUnique(this);

	FiredRockets.Reserve(20);
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HealthShieldComponent->IsDead() || !IsValid(OwnerShip))
	{
		Destroy();
		return;
	}

	if (OwnerShip->bIsSelected && !bIsOrderedToAttack)
	{
		//SetFacingOnMouse();
	}
	else if (bIsOrderedToAttack)
	{
		SetFacingOnActor(ActorToAttack);
	}
	else
	{
		SetFacingLeftRight();
	}

	CheckAngle();

	// Rotate turret this tick
	const FRotator Rotator = GetActorRotation() + DeltaRotation * DeltaTime;
	SetActorRotation(Rotator, ETeleportType::None);
	//const FRotator NewRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), Rotator, DeltaTime, RotationSpeed);
	//SetActorRotation(NewRotation, ETeleportType::None);
}

void ATurret::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(OwnerShip))
	{
		OwnerShip->Turrets.Remove(this);
		// If ship has no working turrets then set bool variable to false
		if (OwnerShip->Turrets.Num() == 0)
		{
			OwnerShip->bHasWorkingTurrets = false;
		}
		//SpawnEmitterAtLocation()
	}
	Super::EndPlay(EndPlayReason);
}


void ATurret::RequestAttack(const AActor* _ActorToAttack)
{
	if (!IsValid(_ActorToAttack))
	{
		return;
	}
	ActorToAttack = _ActorToAttack;
	bIsOrderedToAttack = true;

	// Binding Fire() function that will fire rocket when timer hits FireEveryThisSeconds
	GetWorldTimerManager().SetTimer(THForFiring,this, &ATurret::Fire, FireEveryThisSeconds, true);
}

void ATurret::Fire()
{
	// Stop timer if actor to attack is dead or no order to attack is present
	if (!IsValid(ActorToAttack) || !bIsOrderedToAttack)
	{
		bIsOrderedToAttack = false;
		bShouldFire = false;
		GetWorldTimerManager().ClearTimer(THForFiring);
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, TEXT("Turret is stopping firing"));
		return;
	}
	
	// Check if turret is facing actor to attack
	const FVector VectorFromTurretToActorToAttack = ActorToAttack->GetActorLocation() - GetActorLocation();
	const float AngleBetweenTurretAndActor = AnglesFunctions::FindAngleBetweenVectorsOn2D(GetActorForwardVector(), VectorFromTurretToActorToAttack);
	if (AngleBetweenTurretAndActor > 0 && AngleBetweenTurretAndActor < 10 || AngleBetweenTurretAndActor < 0 && AngleBetweenTurretAndActor > -10) this->bShouldFire = true;

	if (bShouldFire && UKismetMathLibrary::RandomFloat() > ChanceToFire)
	{
		// Fire rocket from arrow
		ARocket* SpawnedRocket = UFactoriesFunctionLibrary::NewRocket(
			GetWorld(), 
			RocketClass.Get(), 
			PlayerController, 
			this,
			GetActorLocation(),
			VectorFromTurretToActorToAttack.Rotation());
		bShouldFire = false;
		FiredRockets.Add(SpawnedRocket);
	}
}

void ATurret::SetFacingOnMouse()
{
	FHitResult Hit;
	PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Camera), false, Hit);
	const FVector VectorFromTurretToCursor = Hit.Location - GetActorLocation();
	const FRotator Rotator = AnglesFunctions::FindYawRotatorOn2D(GetActorForwardVector(), VectorFromTurretToCursor);
	DeltaRotation = FRotator(0, Rotator.Yaw, 0);
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
	DeltaRotation = FRotator(0, Rotator.Yaw, 0);
}

void ATurret::SetFacingOnActor(const AActor* ActorToSetFacingTo)
{
	const FVector VectorFromTurretToActor = ActorToSetFacingTo->GetActorLocation() - GetActorLocation();
	DeltaRotation = AnglesFunctions::FindYawRotatorOn2D(GetActorForwardVector(), VectorFromTurretToActor);
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

	if (Angle > 0 && Angle > MaxAngleDeviation) DeltaRotation = FRotator(0, -0.1, 0);
	if (Angle < 0 && Angle < -1 * MaxAngleDeviation) DeltaRotation = FRotator(0, 0.1, 0);
}