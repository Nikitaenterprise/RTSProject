#include "Ship.h"

#include "RTSPlayerController.h"
#include "HealthShield.h"
#include "Turret.h"
#include "HealthShieldBarHUD.h"
#include "GameHUD.h"
#include "ShipMovementComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
//#include "Blueprint/UserWidget.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
//#include "AIController.h"
//#include "NavigationPath.h"
//#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
//#include "Components/ArrowComponent.h"
//#include "GameFramework/CharacterMovementComponent.h"

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
	
	/*FString str = UKismetSystemLibrary::GetDisplayName(this);
	FString b = bIsSelected ? TEXT("true") : TEXT("false");
	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Yellow, FString::Printf(TEXT("obj=%s, bSelected=%s"), *str, *b));*/

	DeltaTime = _mainDeltaTime;
	PastTime += _mainDeltaTime;
	if (HealthShieldComponent->IsDead()) Destroy(false, true);

	bIsMoving = MovementComponent->Velocity.Size() > 0;
	if (bIsMoving && UKismetMathLibrary::NearlyEqual_FloatFloat(PastTime, DrawNavLineOncePerThisSeconds)) DrawNavLine();
	
}

void AShip::Initialize(ARTSPlayerController* _Controller)
{
	if (_Controller)
	{
		PlayerController = _Controller;

		if(MovementComponent)
		{
			MovementComponent->PlayerController = _Controller;
			MovementComponent->OwnerShip = this;
			MovementComponent->Initialize();	
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("MovementComponent in AShip->Init() is null"));
		}
		
		HealthShieldBarHUD = Cast<UHealthShieldBarHUD>(HealthShieldBar->GetWidget());
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

bool AShip::Move(const FVector _TargetLocation)
{
	if (!MovementComponent)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("MovementComponent in AShip->Move() is null"));
		return false;
	}
	const bool successful = MovementComponent->RequestNavMoving(_TargetLocation);
	if (!successful) return false;
	NavPathCoords = MovementComponent->GetNavPathCoords();
	DrawNavLine();
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

