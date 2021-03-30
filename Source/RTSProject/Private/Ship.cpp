#include "Ship.h"

#include "RTSPlayerController.h"
#include "HealthShield.h"
#include "Turret.h"
#include "AnglesFunctions.h"
#include "HealthShieldBarHUD.h"
#include "GameHUD.h"
#include "ShipMovementComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AShip::AShip(const FObjectInitializer& OI)
	: Super(OI.SetDefaultSubobjectClass<UShipMovementComponent>(FName("ShipMovementComponent")))
{
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());
	RootComponent = StaticMesh;
	
	HealthShieldComponent = CreateDefaultSubobject<UHealthShield>(TEXT("HealthShield"));
	
	SelectionCircle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionCircle"));
	SelectionCircle->SetupAttachment(GetRootComponent());
	SelectionCircle->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HealthShieldBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthShieldBar"));
	HealthShieldBar->SetupAttachment(GetRootComponent());
	HealthShieldBar->SetDrawSize(FVector2D(150, 150));
	HealthShieldBar->SetWidgetSpace(EWidgetSpace::Screen);

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
	
	FString str = UKismetSystemLibrary::GetDisplayName(this);
	FString b = bIsSelected ? TEXT("true") : TEXT("false");
	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Yellow, FString::Printf(TEXT("obj=%s, bSelected=%s"), *str, *b));

	DeltaTime = _mainDeltaTime;
	PastTime += _mainDeltaTime;
	if (HealthShieldComponent->IsDead()) Destroy(false, true);

	bIsMoving = MovementComponent->Velocity.Size() > 0;
	if (bIsMoving && UKismetMathLibrary::NearlyEqual_FloatFloat(PastTime, DrawNavLineOncePerThisSeconds)) DrawNavLine();
	
}

void AShip::BindHUD()
{
	if (PlayerController) 
	{
		HealthShieldBarHUD = Cast<UHealthShieldBarHUD>(HealthShieldBar->GetWidget());
	}
}

void AShip::BindController(ARTSPlayerController* _Controller)
{
	if (_Controller) PlayerController = _Controller;
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
		//HealthShieldBarHUD->SetVisibility(ESlateVisibility::Visible);
		SetHealthShieldBar();
		SelectionCircle->SetVisibility(true);
	}
	else
	{
		HealthShieldBar->SetVisibility(false);
		//HealthShieldBarHUD->SetVisibility(ESlateVisibility::Hidden);
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
			//HealthShieldBarHUD->ShowUI();
			HealthShieldBar->SetVisibility(true);
			SelectionCircle->SetVisibility(true);
			SetHealthShieldBar();
		}
		else
		{
			//HealthShieldBarHUD->HideUI();
			HealthShieldBar->SetVisibility(false);
			SelectionCircle->SetVisibility(false);
		}
	}
}

void AShip::SetHealthShieldBar()
{
	HealthShieldBarHUD->ShieldPercent = HealthShieldComponent->getShieldPercent();
	HealthShieldBarHUD->HealthPercent = HealthShieldComponent->getHealthPercent();
}

bool AShip::Moving(const FVector& v)
{
	bShouldMove = true;
	FVector Direction = v - GetActorLocation();
	Direction.Normalize();
	FVector ForwardVector = GetActorForwardVector();
	ForwardVector.Normalize();
	
	float angle = AnglesFunctions::FindAngleBetweenVectorsOn2D(Direction, ForwardVector);
	if (angle > 5)
	{
		bool clockwise = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(Direction, ForwardVector);
		if (clockwise)
		{
			SetActorRotation(FRotator(0, (GetActorRotation().Yaw - (DeltaTime * angle)), 0), ETeleportType::None);
		}
		else if (!clockwise)
		{
			SetActorRotation(FRotator(0, (GetActorRotation().Yaw + (DeltaTime * angle)), 0), ETeleportType::None);
		}
		
		FVector2D NewForward(GetActorForwardVector().X, GetActorForwardVector().Y);
		
		NewForward = NewForward * TurnForwardSpeed * DeltaTime;
		
		FVector2D NewLocation(GetActorLocation().X, GetActorLocation().Y);
		FVector location((NewLocation + NewForward).X, (NewLocation + NewForward).Y, GetActorLocation().Z);
		SetActorLocation(location, false, nullptr, ETeleportType::None);
	}
	else
	{
		FVector2D NewForward(GetActorForwardVector().X, GetActorForwardVector().Y);
		
		NewForward = NewForward * ForwardSpeed * DeltaTime;
		
		FVector2D NewLocation(GetActorLocation().X, GetActorLocation().Y);
		FVector location((NewLocation + NewForward).X, (NewLocation + NewForward).Y, GetActorLocation().Z);
		SetActorLocation(location, false, nullptr, ETeleportType::None);
	}
	
	if ((v - GetActorLocation()).SizeSquared() > 400) bShouldMove = false;

	return true;
}


bool AShip::CustomMoving(const FVector& DestinationLocation)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Red, TEXT("Failed to cast AIController in Ship"));
		return false;
	}
	
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(
							GetWorld(),
							GetActorLocation(),
							DestinationLocation);
	if (!NavPath) return false;
	
	NavPathCoords = NavPath->PathPoints;
	
	AIController->MoveToLocation(DestinationLocation, 50, 
								false, true, 
								true, true,
								nullptr, true);
	
	
	
	return true;
	
}

void AShip::DrawNavLine()
{
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

