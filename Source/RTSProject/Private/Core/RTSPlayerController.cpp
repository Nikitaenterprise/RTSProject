#include "Core/RTSPlayerController.h"

#include "Components/AttackComponent.h"
#include "Actors/Ship.h"
#include "Actors/Building.h"
#include "Actors/Camera.h"
#include "Core/FactoryAssets.h"
#include "UI/GameHUD.h"
#include "DrawDebugHelpers.h"
#include "Actors/FogOfWar.h"

#include "Kismet/GameplayStatics.h"
#include "Math/UnitConversion.h"


ARTSPlayerController::ARTSPlayerController() {

}

void ARTSPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ARTSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Display, TEXT("Trying to find ACamera on level in ARTSPlayerController::BeginPlay()"));
	ACamera* TestCamera = Cast<ACamera>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!IsValid(TestCamera))
	{
		UE_LOG(LogTemp, Warning, TEXT("ACamera is nullptr in ARTSPlayerController::BeginPlay()"));
		return;
	}
	CameraRef = TestCamera;
	//CameraRef->Initialize(this);
	CameraRef->SetOwner(this);

	UE_LOG(LogTemp, Display, TEXT("Trying to find AGameHUD on level in ARTSPlayerController::BeginPlay()"));
	AGameHUD* TestGameHUD = Cast<AGameHUD>(GetHUD());
	if (!IsValid(TestGameHUD))
	{
		UE_LOG(LogTemp, Warning, TEXT("AGameHUD is nullptr in ARTSPlayerController::BeginPlay()"));
		return;
	}
	GameHUD = TestGameHUD;
	GameHUD->SetOwner(this);	

	UE_LOG(LogTemp, Display, TEXT("Trying to find AFogOfWar on level in ARTSPlayerController::BeginPlay()"));
	TArray<AActor*> ActorsOfClass;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFogOfWar::StaticClass(), ActorsOfClass);
	for (auto& Actor : ActorsOfClass)
	{
		AFogOfWar* TestFogOfWar = Cast<AFogOfWar>(Actor);
		if (!IsValid(TestFogOfWar))
		{
			UE_LOG(LogTemp, Warning, TEXT("AFogOfWar is nullptr in ARTSPlayerController::BeginPlay()"));
		}
		else
		{
			FogOfWar = TestFogOfWar;
			FogOfWar->SetOwner(this);
		}
	}

	// If no AFogOfWar found then create one
	if (!IsValid(FogOfWar))
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't find AFogOfWar on level, trying to spawn one"));
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Owner = this;
		UClass* SpawnClass = FactoryAssets->GetFogOfWarClass().Get();
		AFogOfWar* TestFogOfWar = GetWorld()->SpawnActor<AFogOfWar>(SpawnClass, FVector(0, 0, 0), FRotator(0, 0, 0), Params);
		if (!IsValid(TestFogOfWar))
		{
			UE_LOG(LogTemp, Error, TEXT("AFogOfWar is nullptr in ARTSPlayerController::BeginPlay()"));
			return;
		}
		FogOfWar = TestFogOfWar;
	}
	//FogOfWar->Initialize(this);

	const FInputModeGameAndUI InputMode;
	SetInputMode(InputMode);

	UE_LOG(LogTemp, Display, TEXT("Success of ARTSPlayerController::BeginPlay()"));
}

void ARTSPlayerController::Tick(float mainDeltaTime)
{
	Super::Tick(mainDeltaTime);
}

void ARTSPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ARTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	bShowMouseCursor = true;
	InputComponent->BindAction(TEXT("LMB"), IE_Pressed, this, &ARTSPlayerController::LMBPressed);
	InputComponent->BindAction(TEXT("LMB"), IE_Released, this, &ARTSPlayerController::LMBReleased);
	InputComponent->BindAction(TEXT("RMB"), IE_Pressed, this, &ARTSPlayerController::RMBPressed);
	InputComponent->BindAction(TEXT("RMB"), IE_Pressed, this, &ARTSPlayerController::RMBReleased);
	
	InputComponent->BindAction(TEXT("Damage"), IE_Released, this, &ARTSPlayerController::DamagePressed);
}

void ARTSPlayerController::LMBPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Cyan, TEXT("LMBPressed"));
	bLMBPressed = true;
}

void ARTSPlayerController::LMBReleased()
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Cyan, TEXT("LMBReleased"));
	bLMBPressed = false;
}

void ARTSPlayerController::RMBPressed()
{
	bRMBPressed = true;
}

void ARTSPlayerController::RMBReleased()
{
	bRMBPressed = false;
	ExecuteCommandToSelectedActors<AShip>(&ARTSPlayerController::MoveSelectedActors);
	ExecuteCommandToSelectedActors<AShip>(&ARTSPlayerController::AttackBySelectedActors);
	ExecuteCommandToSelectedActors<ABuilding>(&ARTSPlayerController::SetSpawnPointForSelectedBuildings);
}

void ARTSPlayerController::DamagePressed()
{
}

TArray<ABuilding*> ARTSPlayerController::GetSelectedBuildings()
{
	return GetSelectedActorsByType<ABuilding>();
}

TArray<AShip*> ARTSPlayerController::GetSelectedShips()
{
	return GetSelectedActorsByType<AShip>();
}

bool ARTSPlayerController::MoveSelectedActors(AShip* Ship, FHitResult HitResult)
{
	Ship->RequestMove(FVector(HitResult.Location.X, HitResult.Location.Y, Ship->GetActorLocation().Z));
	return true;
}

bool ARTSPlayerController::AttackBySelectedActors(AShip* Ship, FHitResult HitResult)
{
	UAttackComponent* AttackComponent = Ship->FindComponentByClass<UAttackComponent>();
	if (AttackComponent)
	{
		AActor* AttackedActor = HitResult.Actor.Get();
		UAttackComponent* AttackedActorAttackComponent = AttackedActor->FindComponentByClass<UAttackComponent>();
		if (AttackedActorAttackComponent)
		{
			if (AttackComponent->GetCanAttack() &&
				AttackedActor != Ship &&
				AttackedActorAttackComponent->GetCanBeAttacked())
			{
				Ship->RequestAttack(AttackedActor);
			}
			else return false;
		}
		else return false;
	}
	else return false;
	return true;
}

bool ARTSPlayerController::SetSpawnPointForSelectedBuildings(ABuilding* Building, FHitResult HitResult)
{
	Building->SetSpawnPointLocation(FVector(HitResult.Location.X, HitResult.Location.Y, Building->GetActorLocation().Z));
	return true;
}