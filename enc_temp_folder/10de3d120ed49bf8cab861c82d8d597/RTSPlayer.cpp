#include "Actors/RTSPlayer.h"

#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/RTSPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/PlayerInputDataAsset.h"
#include "InputMappingContext.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"


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

	EnableInput(Cast<APlayerController>(GetController()));

	FVector InitialLocation = GetActorLocation();
	InitialLocation.Z = 100;
	SetActorLocation(InitialLocation, true);
}

void ARTSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Hack for Enhanced input not capturing mouse after click
	if (const auto* PlayerController = Cast<APlayerController>(GetController()))
	{
		FVector Mouse;
		PlayerController->GetMousePosition(Mouse.X, Mouse.Y);
		FInputActionValue Action(Mouse);
		MouseMove(Action);
	}
}

void ARTSPlayer::EnableInput(APlayerController* PlayerController)
{
	Super::EnableInput(PlayerController);

	if (IsValid(PlayerController) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("PlayerController is nullptr in ARTSPlayer::EnableInput()"));
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr in ARTSPlayer::EnableInput()"));
		return;
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
    //EnhancedInputSubsystem->ClearAllMappings();

	if (IsValid(PlayerInputDataAsset) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("PlayerInputDataAsset is nullptr in ARTSPlayer::EnableInput()"));
		UE_LOG(LogTemp, Error, TEXT("PlayerInputDataAsset is nullptr in ARTSPlayer::EnableInput()"));
		return;
	}
    EnhancedInputSubsystem->AddMappingContext(PlayerInputDataAsset->InputMappingContext, 0);

	if (IsValid(EIC) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("EnhancedInputComponent is nullptr in ARTSPlayer::EnableInput()"));
		UE_LOG(LogTemp, Error, TEXT("EnhancedInputComponent is nullptr in ARTSPlayer::EnableInput()"));
		return;
	}
	
	EIC->BindAction(PlayerInputDataAsset->IAKeyboardMovement, ETriggerEvent::Triggered, this, &ARTSPlayer::KeyboardMove);
	EIC->BindAction(PlayerInputDataAsset->IAMouseMovement, ETriggerEvent::Triggered, this, &ARTSPlayer::MouseMove);

	EIC->BindActionValueLambda(PlayerInputDataAsset->IALMB, ETriggerEvent::Started, [&] (const FInputActionValue& InputActionValue)
	{
		OnLeftMouseButtonClicked.Broadcast();
	});
	EIC->BindActionValueLambda(PlayerInputDataAsset->IARMB, ETriggerEvent::Started, [&] (const FInputActionValue& InputActionValue)
	{
		OnRightMouseButtonClicked.Broadcast();
	});

	EIC->BindAction(PlayerInputDataAsset->IACameraZoom, ETriggerEvent::Triggered, this, &ARTSPlayer::Zoom);
	EIC->BindAction(PlayerInputDataAsset->IAZoomReset, ETriggerEvent::Triggered, this, &ARTSPlayer::ZoomReset);

	EIC->BindActionValueLambda(PlayerInputDataAsset->IAPanRotation, ETriggerEvent::Started, [&](const FInputActionValue& InputActionValue)
	{
		bEnablePanRotation = true;
		bDisableZooming = true;
	});
	EIC->BindActionValueLambda(PlayerInputDataAsset->IAPanRotation, ETriggerEvent::Completed, [&](const FInputActionValue& InputActionValue)
	{
		bEnablePanRotation = false;
		bDisableZooming = false;
	});
	EIC->BindActionValueLambda(PlayerInputDataAsset->IAPanReset, ETriggerEvent::Triggered, [&](const FInputActionValue& InputActionValue)
	{
		SetActorRotation(FRotator(0, 0, 0));
	});
	EIC->BindActionValueLambda(PlayerInputDataAsset->IAIncreaseMovementSpeed, ETriggerEvent::Started, [&](const FInputActionValue& InputActionValue)
	{
		MovementSpeedModifier = 2.f;
	});
	EIC->BindActionValueLambda(PlayerInputDataAsset->IAIncreaseMovementSpeed, ETriggerEvent::Completed, [&](const FInputActionValue& InputActionValue)
	{
		MovementSpeedModifier = 1.f;
	});
	EIC->BindActionValueLambda(PlayerInputDataAsset->IADecreaseMovementSpeed, ETriggerEvent::Started, [&](const FInputActionValue& InputActionValue)
	{
		MovementSpeedModifier = 0.3f;
	});
	EIC->BindActionValueLambda(PlayerInputDataAsset->IADecreaseMovementSpeed, ETriggerEvent::Completed, [&](const FInputActionValue& InputActionValue)
	{
		MovementSpeedModifier = 1.f;
	});
}

void ARTSPlayer::KeyboardMove(const FInputActionValue& InputActionValue)
{
	const FVector2D MoveValue = InputActionValue.Get<FVector2D>();
	if (bEnablePanRotation)
	{
		return;
	}
	if (MoveValue.X != 0.f)
	{
		Move(FVector(MoveValue.X, 0, 0));
	}
	if (MoveValue.Y != 0.f)
	{
		Move(FVector(0, MoveValue.Y, 0));
	}
}

void ARTSPlayer::MouseMove(const FInputActionValue& InputActionValue)
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::White, FString("MouseMove"));

	const FVector2D MoveValue = InputActionValue.Get<FVector2D>();
	if (AllowEdgeScrolling)
	{
		EdgeScrolling();
	}
	if (bEnablePanRotation)
	{
		RotatePan(MoveValue.X, MoveValue.Y);
	}
}

void ARTSPlayer::Zoom(const FInputActionValue& InputActionValue)
{
	if (bDisableZooming)
	{
		return;
	}
	const bool bZoomIn = InputActionValue.Get<float>() < 0;
	const float AmountToZoom = CameraZoomSpeed * (bZoomIn ? 1 : -1);
	if (IsValid(SpringArm) == false)
	{
		return;
	}
	SpringArm->TargetArmLength = FMath::Clamp<float>(SpringArm->TargetArmLength + AmountToZoom, CameraMinZoom, CameraMaxZoom);
}

void ARTSPlayer::ZoomReset(const FInputActionValue& InputActionValue)
{
	if (IsValid(SpringArm) == false)
	{
		return;
	}
	SpringArm->TargetArmLength = CameraDefaultZoom;
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

void ARTSPlayer::RotatePan(float x, float y)
{
	const FRotator Rotation = GetActorRotation();
	float Pitch = Rotation.Pitch, Yaw = Rotation.Yaw;
	Yaw += x * PanRotationSpeed;
	Pitch += y * PanRotationSpeed;
	SetActorRotation(FRotator(Pitch, Yaw, Rotation.Roll));
}