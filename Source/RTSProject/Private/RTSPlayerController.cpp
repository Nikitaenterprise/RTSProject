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
	else GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("GameHUD=null in ARTSPlayerController"));

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
	InputComponent->BindAction(TEXT("MovementIncrease"), IE_Pressed, this, &ARTSPlayerController::MovementIncrease);
	InputComponent->BindAction(TEXT("MovementIncrease"), IE_Released, this, &ARTSPlayerController::ResetMovementModifier);
	InputComponent->BindAction(TEXT("MovementDecrease"), IE_Pressed, this, &ARTSPlayerController::MovementDecrease);
	InputComponent->BindAction(TEXT("MovementDecrease"), IE_Released, this, &ARTSPlayerController::ResetMovementModifier);
	// Zoom
	InputComponent->BindAction(TEXT("ZoomIn"), IE_Pressed, this, &ARTSPlayerController::ZoomIn);
	InputComponent->BindAction(TEXT("ZoomOut"), IE_Pressed, this, &ARTSPlayerController::ZoomOut);
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

void ARTSPlayerController::Move(FVector &v)
{
	v *= DefaultMovementSpeed * MovementSpeedModifier;
	FTransform t = CameraRef->GetActorTransform();
	FVector v1 = UKismetMathLibrary::TransformDirection(t, v);
	v1 += t.GetLocation();
	FTransform t1 = FTransform(t.GetRotation(), v1, t.GetScale3D());
	CameraRef->SetActorLocation(FVector(t1.GetLocation().X, t1.GetLocation().Y, 100), true);
}

void ARTSPlayerController::MoveForward(float value)
{
	auto a = FVector(value, 0, 0);
	if (!bDisableCameraMovement) Move(a);
}

void ARTSPlayerController::MoveRight(float value)
{
	auto a = FVector(0, value, 0);
	if (!bDisableCameraMovement) Move(a);
}

void ARTSPlayerController::MovementIncrease()
{
	MovementSpeedModifier = 2;
}

void ARTSPlayerController::MovementDecrease()
{
	MovementSpeedModifier = 0.3;
}

void ARTSPlayerController::ResetMovementModifier()
{
	MovementSpeedModifier = 1;
}

void ARTSPlayerController::ZoomIn()
{
	float previousLenght = CameraRef->SpringArm->TargetArmLength;
	float newLenght = FMath::Clamp<float>(previousLenght - CameraZoomSpeed, CameraMinZoom, CameraMaxZoom);
	CameraRef->SpringArm->TargetArmLength = newLenght;
}

void ARTSPlayerController::ZoomOut()
{
	float previousLenght = CameraRef->SpringArm->TargetArmLength;
	float newLenght = FMath::Clamp<float>(previousLenght + CameraZoomSpeed, CameraMinZoom, CameraMaxZoom);
	CameraRef->SpringArm->TargetArmLength = newLenght;
}

void ARTSPlayerController::ZoomReset()
{
	CameraRef->SpringArm->TargetArmLength = CameraDefaultZoom;
}

void ARTSPlayerController::EdgeScrollingX(float value)
{
	float MouseX = 0, MouseY = 0;
	int32 SizeX = 0, SizeY = 0;
	GetMousePosition(MouseX, MouseY);
	GetViewportSize(SizeX, SizeY);

	float RatioX = MouseX / SizeX;
	if (RatioX >= 0.975)		EdgeScrolling(15, 0);
	else if (RatioX <= 0.025)	EdgeScrolling(-15, 0);
	else						EdgeScrolling(0, 0);
}

void ARTSPlayerController::EdgeScrollingY(float value)
{
	float MouseX = 0, MouseY = 0;
	int32 SizeX = 0, SizeY = 0;
	GetMousePosition(MouseX, MouseY);
	GetViewportSize(SizeX, SizeY);

	float RatioY = MouseY / SizeY;
	if (RatioY >= 0.975)		EdgeScrolling(0, -15);
	else if (RatioY <= 0.025)	EdgeScrolling(0, 15);
	else						EdgeScrolling(0, 0);
}

void ARTSPlayerController::EdgeScrolling(float dx, float dy)
{
	CameraRef->AddActorLocalOffset(FVector(dy, dx, 0), true);
}

void ARTSPlayerController::RotatePanX(float value)
{
	if (bDisableCameraMovement) RotatePan(value, 0);
}

void ARTSPlayerController::RotatePanY(float value)
{
	if (bDisableCameraMovement) RotatePan(0, value);
}

void ARTSPlayerController::RotatePan(float x, float y)
{
	FRotator rotation = CameraRef->GetActorRotation();
	float pitch = rotation.Pitch, yaw = rotation.Yaw, roll = rotation.Roll;
	yaw += x * PanRotationSpeed;
	pitch += y * PanRotationSpeed;
	CameraRef->SetActorRotation(FRotator(pitch, yaw, roll));
}

void ARTSPlayerController::PanReset()
{
	CameraRef->SetActorRotation(FRotator(0, 0, 0));
}

void ARTSPlayerController::EnableCameraMovement()
{
	bDisableCameraMovement = false;
}

void ARTSPlayerController::DisableCameraMovement()
{
	bDisableCameraMovement = true;
}

void ARTSPlayerController::LMBPressed()
{
	bLMBPressed = true;
	DeselectUnits();
	SelectedActors.Empty();
	GameHUD->OnInputStart();
}

void ARTSPlayerController::LMBReleased()
{
	bLMBPressed = false;
	GameHUD->OnInputRelease();
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

bool ARTSPlayerController::SelectedShips()
{
	for (auto& e : SelectedActors)
	{
		AShip* Ship = Cast<AShip>(e);
		if (IsValid(Ship)) return true;
	}
	return false;
}

bool ARTSPlayerController::SelectedBuildings()
{
	for (auto& e : SelectedActors)
	{
		ABuilding* Building = Cast<ABuilding>(e);
		if (IsValid(Building)) return true;
	}
	return false;
}

void ARTSPlayerController::UpdateSelection()
{
	DeselectUnits();
	GameHUD->OnInputHold();
	/*for (const auto& a : SelectedActors) {
		FString str = UKismetSystemLibrary::GetDisplayName(Cast<AShip>(a));
		GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, str);
	}*/
	SelectUnits();
}

void ARTSPlayerController::HighlightActorsUnderCursor()
{
	DehighlightUnit();
	FHitResult hit;
	bool bHit = GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Camera), false, hit);
	if (bHit) HighlightedActor = hit.GetActor();
	HighlightUnit();
}

void ARTSPlayerController::HighlightUnit()
{
	IBaseBehavior* Interface = Cast<IBaseBehavior>(HighlightedActor);
	if (Interface) Interface->Execute_Highlighted(HighlightedActor, true);
}

void ARTSPlayerController::DehighlightUnit()
{
	IBaseBehavior* Interface = Cast<IBaseBehavior>(HighlightedActor);
	if (Interface) Interface->Execute_Highlighted(HighlightedActor, false);

	for (auto& a : PlayersActors)
	{
		Interface = Cast<IBaseBehavior>(a);
		if(Interface) Interface->Execute_Highlighted(a, false);
	}
}

void ARTSPlayerController::SelectUnits()
{
	if (SelectedActors.Num() == 0) return;
	IBaseBehavior* Interface;
	for (auto& a : SelectedActors) 
	{
		if (PlayersActors.Contains(a)) 
		{
			Interface = Cast<IBaseBehavior>(a);
			if (Interface) Interface->Execute_Selected(a, true);
		}
	}
}

void ARTSPlayerController::DeselectUnits()
{
	IBaseBehavior* Interface;
	for (auto& a : SelectedActors)
	{
		Interface = Cast<IBaseBehavior>(a);
		if (Interface) Interface->Execute_Selected(a, false);
	}
	for (auto& a : PlayersActors)
	{
		Interface = Cast<IBaseBehavior>(a);
		if (Interface) Interface->Execute_Selected(a, false);
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
			bool bHit = GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Camera), false, Hit);
			if (bHit)
			{
				float Z = Ship->GetActorLocation().Z;
				float X = Hit.Location.X, Y = Hit.Location.Y;
				Ship->Move(FVector(X, Y, Z));
			}
		}
	}

}

