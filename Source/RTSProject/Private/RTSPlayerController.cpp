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

	GameHUD = Cast<AGameHUD>(GetHUD());
	if (GameHUD) GameHUD->BindController(this);
	else GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("GameHUD=nullptr in ARTSPlayerController"));

}

void ARTSPlayerController::Tick(float mainDeltaTime)
{
	Super::Tick(mainDeltaTime);

	if (bLMBPressed) UpdateSelection();
	else HighlightActorsUnderCursor();

	/*
	if (SelectedShips()) ShipHUD->ShowUI();
	else ShipHUD->HideUI();

	if (SelectedBuildings()) BuildingHUD->ShowUI();
	else BuildingHUD->HideUI();
	*/
}

void ARTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// Movement
	InputComponent->BindAxis(TEXT("MoveForward"), this, &ARTSPlayerController::MoveForward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &ARTSPlayerController::MoveRight);
	InputComponent->BindAction(TEXT("Shift"), IE_Pressed, this, &ARTSPlayerController::MovementIncrease);
	InputComponent->BindAction(TEXT("Shift"), IE_Released, this, &ARTSPlayerController::ResetMovementModifier);
	InputComponent->BindAction(TEXT("Alt"), IE_Pressed, this, &ARTSPlayerController::MovementDecrease);
	InputComponent->BindAction(TEXT("Alt"), IE_Released, this, &ARTSPlayerController::ResetMovementModifier);
	// Zoom
	InputComponent->BindAction(TEXT("MouseYPositive"), IE_Pressed, this, &ARTSPlayerController::MouseYPositiveStart);
	InputComponent->BindAction(TEXT("MouseYPositive"), IE_Released, this, &ARTSPlayerController::MouseYPositiveEnd);
	InputComponent->BindAction(TEXT("MouseYNegative"), IE_Pressed, this, &ARTSPlayerController::MouseYNegativeStart);
	InputComponent->BindAction(TEXT("MouseYNegative"), IE_Released, this, &ARTSPlayerController::MouseYNegativeEnd);
	InputComponent->BindAction(TEXT("ZoomReset"), IE_Pressed, this, &ARTSPlayerController::ZoomReset);
	// Edge scrolling
	InputComponent->BindAxis(TEXT("Mouse X"), this, &ARTSPlayerController::EdgeScrollingX);
	InputComponent->BindAxis(TEXT("Mouse Y"), this, &ARTSPlayerController::EdgeScrollingY);
	// Pan rotation
	InputComponent->BindAction(TEXT("PanReset"), IE_Pressed, this, &ARTSPlayerController::PanReset);
	InputComponent->BindAction(TEXT("Pan"), IE_Pressed, this, &ARTSPlayerController::DisableCameraMovement);
	InputComponent->BindAction(TEXT("Pan"), IE_Released, this, &ARTSPlayerController::EnableCameraMovement);
	InputComponent->BindAxis(TEXT("Mouse X"), this, &ARTSPlayerController::RotatePanX);
	InputComponent->BindAxis(TEXT("Mouse Y"), this, &ARTSPlayerController::RotatePanY);
	// Mouse clicks
	bShowMouseCursor = true;
	InputComponent->BindAction(TEXT("LeftClick"), IE_Pressed, this, &ARTSPlayerController::LMBPressed);
	InputComponent->BindAction(TEXT("LeftClick"), IE_Released, this, &ARTSPlayerController::LMBReleased);
	InputComponent->BindAction(TEXT("RightClick"), IE_Pressed, this, &ARTSPlayerController::RMBPressed);
	InputComponent->BindAction(TEXT("RightClick"), IE_Pressed, this, &ARTSPlayerController::RMBReleased);
}

void ARTSPlayerController::MoveForward(float value)
{
	if (!bDisablePanRotation) CameraRef->Move(FVector(value, 0, 0));
}

void ARTSPlayerController::MoveRight(float value)
{
	if (!bDisablePanRotation) CameraRef->Move(FVector(0, value, 0));
}

void ARTSPlayerController::MovementIncrease()
{
	bShiftPressed = true;
	CameraRef->MovementSpeedModifier = 2;
}

void ARTSPlayerController::MovementDecrease()
{
	bAltPressed = true;
	CameraRef->MovementSpeedModifier = 0.3;
}

void ARTSPlayerController::ResetMovementModifier()
{
	bShiftPressed = false;
	bAltPressed = false;
	CameraRef->MovementSpeedModifier = 1;
}

void ARTSPlayerController::MouseYPositiveStart()
{
	bMouseWheelYPositive = true;
	if (!bDisableZooming) CameraRef->ZoomIn();
}

void ARTSPlayerController::MouseYPositiveEnd()
{
	bMouseWheelYPositive = false;
}

void ARTSPlayerController::MouseYNegativeStart()
{
	bMouseWheelYNegative = true;
	if (!bDisableZooming) CameraRef->ZoomOut();
}

void ARTSPlayerController::MouseYNegativeEnd()
{
	bMouseWheelYNegative = false;
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

void ARTSPlayerController::RotatePanX(float value)
{
	if (bDisablePanRotation) CameraRef->RotatePan(value, 0);
}

void ARTSPlayerController::RotatePanY(float value)
{
	if (bDisablePanRotation) CameraRef->RotatePan(0, value);
}

void ARTSPlayerController::PanReset()
{
	CameraRef->PanReset();
}

void ARTSPlayerController::EnableCameraMovement()
{
	bDisablePanRotation = false;
}

void ARTSPlayerController::DisableCameraMovement()
{
	bDisablePanRotation = true;
}

void ARTSPlayerController::ZoomReset()
{
	CameraRef->ZoomReset();
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

