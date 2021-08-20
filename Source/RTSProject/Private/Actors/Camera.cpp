#include "Actors/Camera.h"

#include "Core/RTSPlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"


ACamera::ACamera()
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

void ACamera::Initialize(ARTSPlayerController* RTSController)
{
	if(RTSController)
	{
		PlayerController = RTSController;
		InputComponent = PlayerController->InputComponent;
		if (InputComponent)
		{
			// Movement
			InputComponent->BindAxis(TEXT("MoveForward"), this, &ACamera::MoveForward);
			InputComponent->BindAxis(TEXT("MoveRight"), this, &ACamera::MoveRight);
			InputComponent->BindAction(TEXT("Shift"), IE_Pressed, this, &ACamera::MovementIncrease);
			InputComponent->BindAction(TEXT("Shift"), IE_Released, this, &ACamera::ResetMovementModifier);
			InputComponent->BindAction(TEXT("Alt"), IE_Pressed, this, &ACamera::MovementDecrease);
			InputComponent->BindAction(TEXT("Alt"), IE_Released, this, &ACamera::ResetMovementModifier);
			// Edge scrolling
			InputComponent->BindAxis(TEXT("MouseX"), this, &ACamera::EdgeScrollingX);
			InputComponent->BindAxis(TEXT("MouseY"), this, &ACamera::EdgeScrollingY);
			// Zoom
			InputComponent->BindAction(TEXT("MouseWheelYPositive"), IE_Pressed, this, &ACamera::MouseWheelYPositiveStart);
			InputComponent->BindAction(TEXT("MouseWheelYNegative"), IE_Pressed, this, &ACamera::MouseWheelYNegativeStart);
			InputComponent->BindAction(TEXT("ZoomReset"), IE_Pressed, this, &ACamera::ZoomReset);
			// Pan rotation
			InputComponent->BindAction(TEXT("MMB"), IE_Pressed, this, &ACamera::DisableCameraMovement);
			InputComponent->BindAction(TEXT("MMB"), IE_Released, this, &ACamera::EnableCameraMovement);
			InputComponent->BindAction(TEXT("PanReset"), IE_Pressed, this, &ACamera::PanReset);
			InputComponent->BindAxis(TEXT("MouseX"), this, &ACamera::RotatePanX);
			InputComponent->BindAxis(TEXT("MouseY"), this, &ACamera::RotatePanY);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("InputComponent in AShip->Init() is null"));
		}
	}
}

void ACamera::BeginPlay()
{
	Super::BeginPlay();
}

void ACamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACamera::MoveForward(float value)
{
	if (!bDisablePanRotation) Move(FVector(value, 0, 0));
}

void ACamera::MoveRight(float value)
{
	if (!bDisablePanRotation) Move(FVector(0, value, 0));
}

void ACamera::Move(const FVector& InputVector)
{
	const FVector NewLocation = InputVector * DefaultMovementSpeed * MovementSpeedModifier;
	const FTransform OldTransform = GetActorTransform();
	FVector Translation = UKismetMathLibrary::TransformDirection(OldTransform, NewLocation);
	Translation += OldTransform.GetLocation();
	const FTransform NewTransform = FTransform(OldTransform.GetRotation(), Translation, OldTransform.GetScale3D());
	SetActorLocation(FVector(NewTransform.GetLocation().X, NewTransform.GetLocation().Y, 100), true);
}

void ACamera::MovementIncrease()
{
	MovementSpeedModifier = 2;
}

void ACamera::MovementDecrease()
{
	MovementSpeedModifier = 0.3;
}

void ACamera::ResetMovementModifier()
{
	MovementSpeedModifier = 1;
}

void ACamera::EdgeScrolling()
{
	int32 SizeX = 0, SizeY = 0;
	float MouseX = 0, MouseY = 0;
	PlayerController->GetMousePosition(MouseX, MouseY);
	PlayerController->GetViewportSize(SizeX, SizeY);

	const float RatioX = MouseX / static_cast<float>(SizeX),
				 RatioY = MouseY / static_cast<float>(SizeY);
	int DX = 0, DY = 0;
	if		(RatioX >= 0.975 && RatioX > 0)	DX = 15;
	else if (RatioX <= 0.025 && RatioX > 0)	DX = -15;
	else if (RatioY >= 0.975 && RatioY > 0)	DY = -15;
	else if (RatioY <= 0.025 && RatioY > 0)	DY = 15;

	AddActorLocalOffset(FVector(DY, DX, 0), true);
}

void ACamera::EdgeScrollingX(float MouseX)
{
	if (AllowEdgeScrolling) EdgeScrolling();
}

void ACamera::EdgeScrollingY(float MouseY)
{
	if (AllowEdgeScrolling) EdgeScrolling();
}

void ACamera::MouseWheelYPositiveStart()
{
	if (!bDisableZooming) ZoomIn();
}

void ACamera::MouseWheelYNegativeStart()
{
	if (!bDisableZooming) ZoomOut();
}

void ACamera::ZoomIn() const
{
	SpringArm->TargetArmLength = FMath::Clamp<float>(SpringArm->TargetArmLength - CameraZoomSpeed, CameraMinZoom, CameraMaxZoom);
}

void ACamera::ZoomOut() const
{
	SpringArm->TargetArmLength = FMath::Clamp<float>(SpringArm->TargetArmLength + CameraZoomSpeed, CameraMinZoom, CameraMaxZoom);
}

void ACamera::ZoomReset()
{
	SpringArm->TargetArmLength = CameraDefaultZoom;
}

void ACamera::RotatePan(float x, float y)
{
	const FRotator Rotation = GetActorRotation();
	float Pitch = Rotation.Pitch, Yaw = Rotation.Yaw;
	Yaw += x * PanRotationSpeed;
	Pitch += y * PanRotationSpeed;
	SetActorRotation(FRotator(Pitch, Yaw, Rotation.Roll));
}

void ACamera::RotatePanX(float value)
{
	if (bDisablePanRotation) RotatePan(value, 0);
}

void ACamera::RotatePanY(float value)
{
	if (bDisablePanRotation) RotatePan(0, value);
}

void ACamera::PanReset()
{
	SetActorRotation(FRotator(0, 0, 0));
}

void ACamera::EnableCameraMovement()
{
	bDisablePanRotation = false;
}

void ACamera::DisableCameraMovement()
{
	bDisablePanRotation = true;
}