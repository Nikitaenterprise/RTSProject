#include "Camera.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"


ACamera::ACamera()
{
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	SetRootComponent(Sphere);
	ConstructorHelpers::FObjectFinder<UStaticMesh> sphereMesh(TEXT("MaterialSphere'/Game/StarterContent/Props/MaterialSphere.MaterialSphere'"));
	if (sphereMesh.Succeeded())
	{
		Sphere->SetStaticMesh(sphereMesh.Object);
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

void ACamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

