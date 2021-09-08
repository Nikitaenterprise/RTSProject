#include "Core/RTSPlayerController.h"

#include "Components/AttackComponent.h"
#include "Actors/Ship.h"
#include "Actors/Building.h"
#include "Actors/Camera.h"
#include "Core/FactoryAssets.h"
#include "UI/GameHUD.h"

#include "Miscellaneous/DamageDealer.h"
#include "DrawDebugHelpers.h"
#include "Actors/FogOfWar.h"

#include "Kismet/GameplayStatics.h"
#include "Math/UnitConversion.h"


ARTSPlayerController::ARTSPlayerController(){	
	
}

void ARTSPlayerController::TestThis(){
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan, TEXT("I'm PlayerController"));
	UE_LOG(LogTemp, Log, TEXT("I'm PlayerController"));
}

void ARTSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CameraRef = Cast<ACamera>(UGameplayStatics::GetPlayerPawn(this, 0));
	CameraRef->Initialize(this);

	AGameHUD* TestGameHUD = Cast<AGameHUD>(GetHUD());
	if (TestGameHUD) GameHUD = TestGameHUD;
	else UE_LOG(LogTemp, Error, TEXT("GameHUD is nullptr in ARTSPlayerController::BeginPlay()"));

	// Trying to find AFogOfWar class on level
	TArray<AActor*> ActorsOfClass;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFogOfWar::StaticClass(), ActorsOfClass);
	for(auto& a : ActorsOfClass)
	{
		AFogOfWar* TestFogOfWar = Cast<AFogOfWar>(a);
		if (TestFogOfWar) FogOfWar = TestFogOfWar;
	}
	// If no AFogOfWar found then create one
	if (!FogOfWar)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		UClass* SpawnClass = FactoryAssets->GetFogOfWarClass().Get();
		FogOfWar = GetWorld()->SpawnActor<AFogOfWar>(SpawnClass, FVector(0, 0, 0), FRotator(0, 0, 0), Params);
	}
	FogOfWar->Initialize(this);
}

void ARTSPlayerController::Tick(float mainDeltaTime)
{
	Super::Tick(mainDeltaTime);

	if (bLMBPressed) UpdateSelection();
	else HighlightActorsUnderCursor();
}

void ARTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction(TEXT("Shift"), IE_Pressed, this, &ARTSPlayerController::ShiftPressed);
	InputComponent->BindAction(TEXT("Shift"), IE_Released, this, &ARTSPlayerController::ShiftReleased);
	
	bShowMouseCursor = true;
	InputComponent->BindAction(TEXT("LMB"), IE_Pressed, this, &ARTSPlayerController::LMBPressed);
	InputComponent->BindAction(TEXT("LMB"), IE_Released, this, &ARTSPlayerController::LMBReleased);
	InputComponent->BindAction(TEXT("RMB"), IE_Pressed, this, &ARTSPlayerController::RMBPressed);
	InputComponent->BindAction(TEXT("RMB"), IE_Pressed, this, &ARTSPlayerController::RMBReleased);

	InputComponent->BindAction(TEXT("Damage"), IE_Released, this, &ARTSPlayerController::DamagePressed);
}

void ARTSPlayerController::ShiftPressed()
{
	bShiftPressed = true;
}

void ARTSPlayerController::ShiftReleased()
{
	bShiftPressed = false;
}

void ARTSPlayerController::LMBPressed()
{
	bLMBPressed = true;
	// If shift pressed then new selected units will add to already selected
	// thus SelectedActors shouldn't be emptied 
	if (!bShiftPressed)	SelectedActors.Empty();
	ShouldBeSelected.Empty();
	GameHUD->OnInputStart();
}

void ARTSPlayerController::LMBReleased()
{
	bLMBPressed = false;
	GameHUD->OnInputRelease();
	// Adding new actors (ShouldBeSelected) to selection pool (SelectedActors)
	if (ShouldBeSelected.Num() == 0) return;
	bool bOnlyBuildings = false, bOnlyShips = false;
	// Pre check fo actors in SelectedActors if shift is pressed
	// e.d. buildings or ships are already in SelectedActors
	if (SelectedActors.Num() != 0)
	{
		if (IsArrayContainThisTypeActors<AShip>(SelectedActors)) bOnlyShips = true;
		else if (IsArrayContainThisTypeActors<ABuilding>(SelectedActors)) bOnlyBuildings = true;
	}
	for (const auto& Actor : ShouldBeSelected)
	{
		if (PlayersActors.Contains(Actor))
		{
			// If SelectedActors has no actors then add first
			if (SelectedActors.Num() == 0)
			{
				SelectedActors.AddUnique(Actor);
				continue;
			}
			// If SelectedActors has at least one building then add
			// building but not ship
			if (!bOnlyShips && Cast<ABuilding>(Actor))
			{
				SelectedActors.AddUnique(Actor);
				bOnlyBuildings = true;
			}
			
			// If SelectedActors has at least one ship then add
			// ship but not building
			else if (!bOnlyBuildings && Cast<AShip>(Actor))
			{
				SelectedActors.AddUnique(Actor);
				bOnlyShips = true;
			}
		}
	}
	// Set ShouldBeSelected Execute_Highlighted to false
	// because actors was highlighted while rectangle was drawn
	for (auto& Actor : ShouldBeSelected)
	{
		IBaseBehavior* Interface = Cast<IBaseBehavior>(Actor);
		if (Interface) Interface->Execute_Highlighted(Actor, false);
	}
	
	// Set actors Execute_Selected to true
	for (auto& Actor : SelectedActors)
	{
		IBaseBehavior* Interface = Cast<IBaseBehavior>(Actor);
		if (Interface) Interface->Execute_Selected(Actor, true);
	}
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
	AShip* Ship = Cast<AShip>(HighlightedActor);
	if (Ship) DamageDealer::DealDamage(30, Ship);
}

void ARTSPlayerController::UpdateSelection()
{
	// All actors should be deselected unless shift is pressed
	// in this case SelectedActors won't be deselected
	for (auto& a : PlayersActors)
	{
		if (!SelectedActors.Contains(a))
		{
			IBaseBehavior* Interface = Cast<IBaseBehavior>(a);
			if (Interface) Interface->Execute_Selected(a, false);
		}
	}

	GameHUD->OnInputHold();

	// Select actors which came from GameHUD selection rectangle
	for (auto& a : ShouldBeSelected)
	{
		IBaseBehavior* Interface = Cast<IBaseBehavior>(a);
		if (Interface) Interface->Execute_Highlighted(a, true);
	}
}

void ARTSPlayerController::HighlightActorsUnderCursor()
{
	// If HighlightedActor is in SelectedActors then
	// it shouldn't be highlighted
	for (auto& a : SelectedActors)
	{
		if (SelectedActors.Contains(HighlightedActor)) return;
	}
	// Dehighlight unit
	IBaseBehavior* Interface = Cast<IBaseBehavior>(HighlightedActor);
	if (Interface) Interface->Execute_Highlighted(HighlightedActor, false);
	
	FHitResult Hit;
	const bool bHit = GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Camera), false, Hit);
	if (bHit)
	{
		HighlightedActor = Hit.GetActor();
		// Highlight unit
		Interface = Cast<IBaseBehavior>(HighlightedActor);
		if (Interface) Interface->Execute_Highlighted(HighlightedActor, true);
	}
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
			if (AttackComponent->bCanAttack &&
				AttackedActor != Ship &&
				AttackedActorAttackComponent->bCanBeAttacked)
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