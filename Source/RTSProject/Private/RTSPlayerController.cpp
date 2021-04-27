#include "RTSPlayerController.h"

#include "Ship.h"
#include "Building.h"
#include "Camera.h"
#include "FactoryAssets.h"
#include "GameHUD.h"


#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
//#include "Blueprint/UserWidget.h"
//#include "Engine/Engine.h"


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

}

void ARTSPlayerController::Tick(float mainDeltaTime)
{
	Super::Tick(mainDeltaTime);

	if (bLMBPressed) UpdateSelection();
	else HighlightActorsUnderCursor();

	if (GameHUD)
	{
		if (ShipsSelected()) GameHUD->ShowShipHUD();
		else if (BuildingsSelected()) GameHUD->ShowBuildingHUD();
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
	GameHUD->OnInputStart();
}

void ARTSPlayerController::LMBReleased()
{
	bLMBPressed = false;
	GameHUD->OnInputRelease();
	// Adding new actors (ShouldBeSelected) to selection pool (SelectedActors)
	if (ShouldBeSelected.Num() != 0)
	{
		for (const auto& a : ShouldBeSelected)
		{
			if (PlayersActors.Contains(a)) SelectedActors.AddUnique(a);
		}
	}
}

void ARTSPlayerController::RMBPressed()
{
	bRMBPressed = true;
}

void ARTSPlayerController::RMBReleased()
{
	bRMBPressed = false;
	MoveSelectedActors();
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
		if (Interface) Interface->Execute_Selected(a, true);
	}
}

bool ARTSPlayerController::ShipsSelected()
{
	if (SelectedActors.Num() == 0) return false;
	for (auto& e : SelectedActors)
	{
		AShip* Ship = Cast<AShip>(e);
		if (IsValid(Ship)) return true;
	}
	return false;
}

bool ARTSPlayerController::BuildingsSelected()
{
	if (SelectedActors.Num() == 0) return false;
	for (auto& e : SelectedActors)
	{
		ABuilding* Building = Cast<ABuilding>(e);
		if (IsValid(Building)) return true;
	}
	return false;
}

void ARTSPlayerController::HighlightActorsUnderCursor()
{
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

void ARTSPlayerController::MoveSelectedActors()
{
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

