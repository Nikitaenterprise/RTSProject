#include "Actors/Units/Turret.h"

#include "AbilitySystemComponent.h"
#include "Core/RTSPlayerController.h"
#include "Actors/Units/Ship.h"
#include "Miscellaneous/AnglesFunctions.h"
#include "Core/FactoryAssets.h"
#include "GAS/HealthShieldAttributeSet.h"
#include "GAS/TurretAttributeSet.h"
#include "GAS/TurretFireAbility.h"
#include "Actors/Projectile.h"
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
	AbilitySystemComponent=CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	HealthShieldAttributeSet = CreateDefaultSubobject<UHealthShieldAttributeSet>(TEXT("HealthShieldAttributeSet"));
	TurretAttributeSet = CreateDefaultSubobject<UTurretAttributeSet>(TEXT("TurretAttributeSet"));
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

	if (!IsValid(OwnerShip->GetPlayerController()))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr in ATurret::BeginPlay()"));
		return;
	}
	PlayerController = OwnerShip->GetPlayerController();

	AbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(HealthShieldAttributeSet);
	AbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(TurretAttributeSet);
	TurretFireAbilityHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(TurretFireAbility, 1, INDEX_NONE, this));
	HealthShieldAttributeSet->OnHealthZeroed.BindLambda([This = TWeakObjectPtr<ThisClass>(this)]()
	{
		if(This.IsValid())
		{
			This->Destroy();
		}
	});
	
	// Decide on which side the turret is
	const FVector ShipForward = OwnerShip->GetActorForwardVector();
	const FVector FromCenterOfShipToTurret = GetActorLocation() - OwnerShip->GetActorLocation();
	const bool bClockwise = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(ShipForward, FromCenterOfShipToTurret);
	if (bClockwise) OnWhichSide = ESide::Right;
	if (!bClockwise) OnWhichSide = ESide::Left;

	SetFacingLeftRight();

	OwnerShip->SetHasWorkingTurrets(true);

	// If no rocket is set in turret blueprint then set first from FactoryAssets
	if (ProjectileClass.Get() == nullptr)
	{
		if (PlayerController->GetFactoryAssets())
		{
			if (PlayerController->GetFactoryAssets()->GetRocketClass(0))
			{
				ProjectileClass = PlayerController->GetFactoryAssets()->GetRocketClass(0);
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
	OwnerShip->GetTurrets().AddUnique(this);
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OwnerShip->IsSelected() && !bIsOrderedToAttack)
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
		OwnerShip->GetTurrets().Remove(this);
		// If ship has no working turrets then set bool variable to false
		if (OwnerShip->GetTurrets().Num() == 0)
		{
			OwnerShip->SetHasWorkingTurrets(false);
		}
		//SpawnEmitterAtLocation()
	}
	Super::EndPlay(EndPlayReason);
}

void ATurret::RequestAttack(const AActor* NewActorToAttack)
{
	if (!IsValid(NewActorToAttack))
	{
		return;
	}
	ActorToAttack = NewActorToAttack;
	bIsOrderedToAttack = true;
	// Clear previous timer if it was set
	GetWorldTimerManager().ClearTimer(FiringTimerHandle);
	// Binding Fire() function that will fire rocket when timer hits FireEveryThisSeconds
	GetWorldTimerManager().SetTimer(FiringTimerHandle,this, &ThisClass::Fire, TurretAttributeSet->GetFireRate(), true);
}

void ATurret::Fire()
{
	// Stop timer if actor to attack is dead or no order to attack is present
	if (!IsValid(ActorToAttack) || !bIsOrderedToAttack)
	{
		bIsOrderedToAttack = false;
		bShouldFire = false;
		GetWorldTimerManager().ClearTimer(FiringTimerHandle);
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, TEXT("Turret stops firing"));
		return;
	}
	
	// Check if turret is facing actor to attack
	const FVector VectorFromTurretToActorToAttack = ActorToAttack->GetActorLocation() - GetActorLocation();
	const float AngleBetweenTurretAndActor = AnglesFunctions::FindAngleBetweenVectorsOn2D(GetActorForwardVector(), VectorFromTurretToActorToAttack);
	if (AngleBetweenTurretAndActor > 0 && AngleBetweenTurretAndActor < 10 || AngleBetweenTurretAndActor < 0 && AngleBetweenTurretAndActor > -10) this->bShouldFire = true;

	if (bShouldFire && UKismetMathLibrary::RandomFloat() < TurretAttributeSet->GetChanceToFire())
	{
		// Fire rocket from arrow
		AProjectile* Projectile = UFactoriesFunctionLibrary::NewRocket(
			GetWorld(), 
			ProjectileClass.Get(), 
			PlayerController, 
			this,
			GetActorLocation(),
			VectorFromTurretToActorToAttack.Rotation());
		if (Projectile)
		{
			Projectile->SetTarget(ActorToAttack);
		}
		bShouldFire = false;
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