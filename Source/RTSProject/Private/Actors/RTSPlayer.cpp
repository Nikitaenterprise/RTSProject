#include "Actors/RTSPlayer.h"

#include "Core/RTSPlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"


ARTSPlayer::ARTSPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	SetRootComponent(Sphere);
	const ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("MaterialSphere'/Game/StarterContent/Props/MaterialSphere.MaterialSphere'"));
	if (SphereMesh.Succeeded())
	{
		Sphere->SetStaticMesh(SphereMesh.Object);
	}
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Sphere);
	SpringArm->TargetArmLength = 1500.0;
	SpringArm->SetRelativeRotation(FRotator(-40.0, 0, 0));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	FloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingMovement"));
	FloatingMovement->MaxSpeed = 5000.0;
	FloatingMovement->Acceleration = 8000.0;
}

void ARTSPlayer::BeginPlay()
{
	Super::BeginPlay();
	BindInputs();
}

void ARTSPlayer::BindInputs()
{
	if (IsValid(InputComponent) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("InputComponent is nullptr in ACamera::BeginPlay()"));
		UE_LOG(LogTemp, Error, TEXT("InputComponent is nullptr in ACamera::BeginPlay()"));
		return;
	}
	
	// Movement
	InputComponent->BindAxis(TEXT("MoveForward"), this, &ARTSPlayer::MoveForward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &ARTSPlayer::MoveRight);
	InputComponent->BindAction(TEXT("Shift"), IE_Pressed, this, &ARTSPlayer::MovementIncrease);
	InputComponent->BindAction(TEXT("Shift"), IE_Released, this, &ARTSPlayer::ResetMovementModifier);
	InputComponent->BindAction(TEXT("Alt"), IE_Pressed, this, &ARTSPlayer::MovementDecrease);
	InputComponent->BindAction(TEXT("Alt"), IE_Released, this, &ARTSPlayer::ResetMovementModifier);
	// Edge scrolling
	InputComponent->BindAxis(TEXT("MouseX"), this, &ARTSPlayer::EdgeScrollingX);
	InputComponent->BindAxis(TEXT("MouseY"), this, &ARTSPlayer::EdgeScrollingY);
	// Zoom
	InputComponent->BindAction(TEXT("MouseWheelYPositive"), IE_Pressed, this, &ARTSPlayer::MouseWheelYPositiveStart);
	InputComponent->BindAction(TEXT("MouseWheelYNegative"), IE_Pressed, this, &ARTSPlayer::MouseWheelYNegativeStart);
	InputComponent->BindAction(TEXT("ZoomReset"), IE_Pressed, this, &ARTSPlayer::ZoomReset);
	// Pan rotation
	InputComponent->BindAction(TEXT("MMB"), IE_Pressed, this, &ARTSPlayer::DisableCameraMovement);
	InputComponent->BindAction(TEXT("MMB"), IE_Released, this, &ARTSPlayer::EnableCameraMovement);
	InputComponent->BindAction(TEXT("PanReset"), IE_Pressed, this, &ARTSPlayer::PanReset);
	InputComponent->BindAxis(TEXT("MouseX"), this, &ARTSPlayer::RotatePanX);
	InputComponent->BindAxis(TEXT("MouseY"), this, &ARTSPlayer::RotatePanY);
}

void ARTSPlayer::MoveForward(float value)
{
	if (!bDisablePanRotation) Move(FVector(value, 0, 0));
}

void ARTSPlayer::MoveRight(float value)
{
	if (!bDisablePanRotation) Move(FVector(0, value, 0));
}

void ARTSPlayer::Move(const FVector& InputVector)
{
	const FVector NewLocation = InputVector * DefaultMovementSpeed * MovementSpeedModifier;
	const FTransform OldTransform = GetActorTransform();
	FVector Translation = UKismetMathLibrary::TransformDirection(OldTransform, NewLocation);
	Translation += OldTransform.GetLocation();
	const FTransform NewTransform = FTransform(OldTransform.GetRotation(), Translation, OldTransform.GetScale3D());
	SetActorLocation(FVector(NewTransform.GetLocation().X, NewTransform.GetLocation().Y, 100), true);
}

void ARTSPlayer::MovementIncrease()
{
	MovementSpeedModifier = 2;
}

void ARTSPlayer::MovementDecrease()
{
	MovementSpeedModifier = 0.3;
}

void ARTSPlayer::ResetMovementModifier()
{
	MovementSpeedModifier = 1;
}

void ARTSPlayer::EdgeScrolling()
{
	int32 SizeX = 0, SizeY = 0;
	float MouseX = 0, MouseY = 0;
	
	const auto* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->GetMousePosition(MouseX, MouseY);
		PlayerController->GetViewportSize(SizeX, SizeY);
	}

	const float RatioX = MouseX / static_cast<float>(SizeX);
	const float RatioY = MouseY / static_cast<float>(SizeY);
	int DX = 0, DY = 0;
	if		(RatioX >= 0.975 && RatioX > 0)	DX = 15;
	else if (RatioX <= 0.025 && RatioX > 0)	DX = -15;
	else if (RatioY >= 0.975 && RatioY > 0)	DY = -15;
	else if (RatioY <= 0.025 && RatioY > 0)	DY = 15;

	AddActorLocalOffset(FVector(DY, DX, 0), true);
}

void ARTSPlayer::EdgeScrollingX(float MouseX)
{
	if (AllowEdgeScrolling) EdgeScrolling();
}

void ARTSPlayer::EdgeScrollingY(float MouseY)
{
	if (AllowEdgeScrolling) EdgeScrolling();
}

void ARTSPlayer::MouseWheelYPositiveStart()
{
	if (!bDisableZooming) ZoomIn();
}

void ARTSPlayer::MouseWheelYNegativeStart()
{
	if (!bDisableZooming) ZoomOut();
}

void ARTSPlayer::ZoomIn() const
{
	SpringArm->TargetArmLength = FMath::Clamp<float>(SpringArm->TargetArmLength - CameraZoomSpeed, CameraMinZoom, CameraMaxZoom);
}

void ARTSPlayer::ZoomOut() const
{
	SpringArm->TargetArmLength = FMath::Clamp<float>(SpringArm->TargetArmLength + CameraZoomSpeed, CameraMinZoom, CameraMaxZoom);
}

void ARTSPlayer::ZoomReset()
{
	SpringArm->TargetArmLength = CameraDefaultZoom;
}

void ARTSPlayer::RotatePan(float x, float y)
{
	const FRotator Rotation = GetActorRotation();
	float Pitch = Rotation.Pitch, Yaw = Rotation.Yaw;
	Yaw += x * PanRotationSpeed;
	Pitch += y * PanRotationSpeed;
	SetActorRotation(FRotator(Pitch, Yaw, Rotation.Roll));
}

void ARTSPlayer::RotatePanX(float value)
{
	if (bDisablePanRotation) RotatePan(value, 0);
}

void ARTSPlayer::RotatePanY(float value)
{
	if (bDisablePanRotation) RotatePan(0, value);
}

void ARTSPlayer::PanReset()
{
	SetActorRotation(FRotator(0, 0, 0));
}

void ARTSPlayer::EnableCameraMovement()
{
	bDisablePanRotation = false;
}

void ARTSPlayer::DisableCameraMovement()
{
	bDisablePanRotation = true;
}