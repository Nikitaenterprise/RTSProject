#include "RTSPlayerController.h"

#include "Ship.h"
#include "Building.h"
#include "Camera.h"
#include "FactoryAssets.h"
#include "GameHUD.h"

#include "DamageDealer.h"
#include "FogOfWar.h"

#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"


ARTSPlayerController::ARTSPlayerController(){	
	
}

void ARTSPlayerController::TestThis(){
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan, TEXT("I'm PlayerController"));
}

UFactoryAssets* ARTSPlayerController::GetFactoryAssets()
{
	if (FactoryAssets) return FactoryAssets;
	return nullptr;
}

void ARTSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CameraRef = Cast<ACamera>(UGameplayStatics::GetPlayerPawn(this, 0));
	CameraRef->Initialize(this);
	
	GameHUD = Cast<AGameHUD>(GetHUD());
	if (GameHUD) GameHUD->BindController(this);
	else GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("GameHUD=nullptr in ARTSPlayerController"));

	// Setup FogOfWar
	// Find FOW actor in world
	TArray<AActor*> FogOfWarArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFogOfWar::StaticClass(), FogOfWarArray);
	for(auto&a:FogOfWarArray)
	{
		AFogOfWar* CastTo = Cast<AFogOfWar>(a);
		if (CastTo)
		{
			FogOfWar = CastTo;
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, TEXT("working"));
		}
	}
	// If there are no FOW actor placed in world then spawn it
	if (!FogOfWar)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FogOfWar = GetWorld()->SpawnActor<AFogOfWar>(GetFactoryAssets()->FogOfWarClass.Get(), FVector(0,0,0), FRotator(0,0,0), Params);
	}
	
}

void ARTSPlayerController::Tick(float mainDeltaTime)
{
	Super::Tick(mainDeltaTime);

	if (bLMBPressed) UpdateSelection();
	else HighlightActorsUnderCursor();

	if (GameHUD)
	{
		if (IsArrayContainThisTypeActors<AShip>(SelectedActors)) GameHUD->ShowShipHUD();
		else if (IsArrayContainThisTypeActors<ABuilding>(SelectedActors)) GameHUD->ShowBuildingHUD();
		else GameHUD->ShowBasicButtonsHUD();	
	}
}

void ARTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction(TEXT("Shift"), IE_Pressed, this, &ARTSPlayerController::ShiftPressed);
	InputComponent->BindAction(TEXT("Shift"), IE_Released, this, &ARTSPlayerController::ShiftReleased);
		
	// Edge scrolling
	InputComponent->BindAxis(TEXT("MouseX"), this, &ARTSPlayerController::EdgeScrollingX);
	InputComponent->BindAxis(TEXT("MouseY"), this, &ARTSPlayerController::EdgeScrollingY);
	
	// Mouse clicks
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

void ARTSPlayerController::EdgeScrollingX(float value)
{
	float MouseX = 0, MouseY = 0;
	int32 SizeX = 0, SizeY = 0;
	GetMousePosition(MouseX, MouseY);
	GetViewportSize(SizeX, SizeY);

	const float RatioX = MouseX / static_cast<float>(SizeX);
	if (RatioX >= 0.975)		CameraRef->EdgeScrolling(15, 0);
	else if (RatioX <= 0.025)	CameraRef->EdgeScrolling(-15, 0);
	else						CameraRef->EdgeScrolling(0, 0);
}

void ARTSPlayerController::EdgeScrollingY(float value)
{
	float MouseX = 0, MouseY = 0;
	int32 SizeX = 0, SizeY = 0;
	GetMousePosition(MouseX, MouseY);
	GetViewportSize(SizeX, SizeY);

	const float RatioY = MouseY / static_cast<float>(SizeY);
	if (RatioY >= 0.975)		CameraRef->EdgeScrolling(0, -15);
	else if (RatioY <= 0.025)	CameraRef->EdgeScrolling(0, 15);
	else						CameraRef->EdgeScrolling(0, 0);
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
	for (const auto& a : ShouldBeSelected)
	{
		if (PlayersActors.Contains(a))
		{
			// If SelectedActors has no actors then add first
			if (SelectedActors.Num() == 0)
			{
				SelectedActors.AddUnique(a);
				continue;
			}
			// If SelectedActors has at least one building then add
			// building but not ship
			if (!bOnlyShips && Cast<ABuilding>(a))
			{
				SelectedActors.AddUnique(a);
				bOnlyBuildings = true;
			}
			
			// If SelectedActors has at least one ship then add
			// ship but not building
			else if (!bOnlyBuildings && Cast<AShip>(a))
			{
				SelectedActors.AddUnique(a);
				bOnlyShips = true;
			}
		}
	}
	// Set ShouldBeSelected Execute_Highlighted to false
	// because actors was highlighted while rectangle was drawn
	for (auto& a : ShouldBeSelected)
	{
		IBaseBehavior* Interface = Cast<IBaseBehavior>(a);
		if (Interface) Interface->Execute_Highlighted(a, false);
	}
	
	// Set actors Execute_Selected to true
	for (auto& a : SelectedActors)
	{
		IBaseBehavior* Interface = Cast<IBaseBehavior>(a);
		if (Interface) Interface->Execute_Selected(a, true);
	}
}

void ARTSPlayerController::RMBPressed()
{
	bRMBPressed = true;
}

void ARTSPlayerController::RMBReleased()
{
	bRMBPressed = false;
	MoveSelectedShips();
	SetSpawnPointForSelectedBuildings();
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

void ARTSPlayerController::MoveSelectedShips()
{
	if (SelectedActors.Num() == 0) return;
	for (auto& a : SelectedActors)
	{
		AShip* Ship = Cast<AShip>(a);
		if (Ship)
		{
			FHitResult Hit;
			const bool bHit = GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Camera), false, Hit);
			if (bHit)
			{
				float Z = Ship->GetActorLocation().Z;
				float X = Hit.Location.X, Y = Hit.Location.Y;
				Ship->Move(FVector(X, Y, Z));
			}
		}
	}
}

void ARTSPlayerController::SetSpawnPointForSelectedBuildings()
{
	if (SelectedActors.Num() == 0) return;
	for (auto& a : SelectedActors)
	{
		ABuilding* Building = Cast<ABuilding>(a);
		if(Building)
		{
			FHitResult Hit;
			const bool bHit = GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Camera), false, Hit);
			if (bHit)
			{
				float Z = Building->GetActorLocation().Z;
				float X = Hit.Location.X, Y = Hit.Location.Y;
				Building->SetSpawnPointLocation(FVector(X, Y, Z));
			}
		}
	}
}
