#include "Camera.h"

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

void ACamera::BeginPlay()
{
	Super::BeginPlay();
}

void ACamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

void ACamera::EdgeScrolling(float dx, float dy)
{
	AddActorLocalOffset(FVector(dy, dx, 0), true);
}

void ACamera::ZoomIn() const
{
	SpringArm->TargetArmLength = FMath::Clamp<float>(SpringArm->TargetArmLength - CameraZoomSpeed, CameraMinZoom, CameraMaxZoom);
}

void ACamera::ZoomOut() const
{
	SpringArm->TargetArmLength = FMath::Clamp<float>(SpringArm->TargetArmLength + CameraZoomSpeed, CameraMinZoom, CameraMaxZoom);
}

void ACamera::ZoomReset() const
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

void ACamera::PanReset()
{
	SetActorRotation(FRotator(0, 0, 0));
}

