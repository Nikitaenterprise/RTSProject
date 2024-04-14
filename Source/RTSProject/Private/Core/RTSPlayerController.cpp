#include "Core/RTSPlayerController.h"

#include "Actors/Buildings/Building.h"
#include "Actors/FogOfWar.h"
#include "Actors/RTSPlayer.h"
#include "Actors/Units/Ship.h"
#include "Actors/Units/Squad.h"
#include "Actors/Units/Worker.h"
#include "AI/Orders/OrdersProcessor.h"
#include "AIController.h"
#include "Components/AttackComponent.h"
#include "Core/FactoryAssets.h"
#include "Core/RTSGameMode.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Input/PlayerInputDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnitConversion.h"
#include "UI/GameHUD.h"


void ARTSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Display, TEXT("Trying to find AGameHUD on level in ARTSPlayerController::BeginPlay()"));
	AGameHUD* TestGameHUD = Cast<AGameHUD>(GetHUD());
	if (!IsValid(TestGameHUD))
	{
		UE_LOG(LogTemp, Warning, TEXT("AGameHUD is nullptr in ARTSPlayerController::BeginPlay()"));
		return;
	}
	GameHUD = TestGameHUD;
	GameHUD->SetOwner(this);	

	UE_LOG(LogTemp, Display, TEXT("Trying to find AFogOfWar on level in ARTSPlayerController::BeginPlay()"));
	TArray<AActor*> ActorsOfClass;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFogOfWar::StaticClass(), ActorsOfClass);
	for (const auto& Actor : ActorsOfClass)
	{
		AFogOfWar* TestFogOfWar = Cast<AFogOfWar>(Actor);
		if (!IsValid(TestFogOfWar))
		{
			UE_LOG(LogTemp, Warning, TEXT("AFogOfWar is nullptr in ARTSPlayerController::BeginPlay()"));
		}
		else
		{
			FogOfWar = TestFogOfWar;
			FogOfWar->SetOwner(this);
		}
	}

	// If no AFogOfWar found then create one
	if (IsValid(FogOfWar) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't find AFogOfWar on level, trying to spawn one"));
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Owner = this;

		if (const auto* GameMode = ARTSGameMode::GetRTSGameMode(GetWorld()))
		{
			UClass* SpawnClass = GameMode->GetFactoryAssets()->GetFogOfWarClass().Get();
			AFogOfWar* TestFogOfWar = GetWorld()->SpawnActor<AFogOfWar>(SpawnClass, FVector(0, 0, 0), FRotator(0, 0, 0), Params);
			if (!IsValid(TestFogOfWar))
			{
				UE_LOG(LogTemp, Error, TEXT("AFogOfWar is nullptr in ARTSPlayerController::BeginPlay()"));
				return;
			}
			FogOfWar = TestFogOfWar;
		}
	}
	//FogOfWar->Initialize(this);

	OrdersProcessor = NewObject<UOrdersProcessor>();
	OrdersProcessor->Initialize(this);

	UE_LOG(LogTemp, Display, TEXT("Success of ARTSPlayerController::BeginPlay()"));
}

void ARTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	bShowMouseCursor = true;

	const FInputModeGameAndUI InputMode;
	SetInputMode(InputMode);

	UCommonUIActionRouterBase* ActionRouter = GetLocalPlayer()->GetSubsystem<UCommonUIActionRouterBase>();
	if (IsValid(ActionRouter))
	{
		FUIInputConfig NewInputConfig(ECommonInputMode::All, EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown, false);
		NewInputConfig.bIgnoreLookInput = true;
		NewInputConfig.bIgnoreMoveInput = false;
		ActionRouter->SetActiveUIInputConfig(NewInputConfig);
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (IsValid(EIC) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("EnhancedInputComponent is nullptr in ARTSPlayerController::SetupInputComponent()"));
		UE_LOG(LogTemp, Error, TEXT("EnhancedInputComponent is nullptr in ARTSPlayerController::SetupInputComponent()"));
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (IsValid(PlayerInputDataAsset) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("PlayerInputDataAsset is nullptr in ARTSPlayerController::SetupInputComponent()"));
		UE_LOG(LogTemp, Error, TEXT("PlayerInputDataAsset is nullptr in ARTSPlayerController::SetupInputComponent()"));
		return;
	}
    EnhancedInputSubsystem->AddMappingContext(PlayerInputDataAsset->InputMappingContext, 0);

	EIC->BindActionValueLambda(PlayerInputDataAsset->IALMB, ETriggerEvent::Completed, [&] (const FInputActionValue& InputActionValue)
	{
		OnLeftMouseButtonClicked.Broadcast(ETriggerEvent::Completed);
	});
	EIC->BindActionValueLambda(PlayerInputDataAsset->IARMB, ETriggerEvent::Completed, [&] (const FInputActionValue& InputActionValue)
	{
		RMBReleased();
		OnRightMouseButtonClicked.Broadcast(ETriggerEvent::Completed);
	});
	
	InputComponent->BindAction(TEXT("Damage"), IE_Released, this, &ThisClass::DamagePressed);

	InputComponent->BindAction(TEXT("SaveControlGroup0"), IE_Released, this, &ThisClass::SaveControlGroup0);
	InputComponent->BindAction(TEXT("SaveControlGroup1"), IE_Released, this, &ThisClass::SaveControlGroup1);
	InputComponent->BindAction(TEXT("SaveControlGroup2"), IE_Released, this, &ThisClass::SaveControlGroup2);

	InputComponent->BindAction(TEXT("LoadControlGroup0"), IE_Released, this, &ThisClass::LoadControlGroup0);
	InputComponent->BindAction(TEXT("LoadControlGroup1"), IE_Released, this, &ThisClass::LoadControlGroup1);
	InputComponent->BindAction(TEXT("LoadControlGroup2"), IE_Released, this, &ThisClass::LoadControlGroup2);

	// Setup control groups.
	ControlGroups.SetNum(3);
}

void ARTSPlayerController::LMBPressed()
{
}

void ARTSPlayerController::RMBReleased()
{
	ExecuteCommandToSelectedActors<AWorker>([This=TWeakObjectPtr<ThisClass>(this)](auto&& ...Args)-> bool
	{
		if (This.IsValid() == false)
		{
			return false;  
		}
		if (This->OrdersProcessor == nullptr)
		{
			return false;
		}

		auto Order = [This](const AShip* InShip, const FHitResult& HitResult)
		{
			const FVector OrderLocation(HitResult.Location.X, HitResult.Location.Y, InShip->GetActorLocation().Z);
			return This->OrdersProcessor->ProcessOrder(EOrderType::GatherResource, OrderLocation, HitResult);
		};

		return Order(Args...);
	});
	ExecuteCommandToSelectedActors<AShip>([This=TWeakObjectPtr<ThisClass>(this)](auto&& ...Args)-> bool
	{
		if (This.IsValid() == false)
		{
	 		return false;  
		}
		if (This->OrdersProcessor == nullptr)
		{
	 		return false;
		}
	 	
		auto Order = [This](const AShip* InShip, const FHitResult& HitResult)
		{
	 		const FVector OrderLocation(HitResult.Location.X, HitResult.Location.Y, InShip->GetActorLocation().Z);
	 		return This->OrdersProcessor->ProcessOrder(EOrderType::MoveOrder, OrderLocation, HitResult);
		};
	 	
		return Order(Args...);
	});
	ExecuteCommandToSelectedActors<AShip>([This=TWeakObjectPtr<ThisClass>(this)](auto&& ...Args)-> bool
	{
		if (This.IsValid())
		{
	 		return This->AttackBySelectedActors(Args...);
		}
		return false;
	});
	ExecuteCommandToSelectedActors<ABuilding>([This=TWeakObjectPtr<ThisClass>(this)](auto&& ...Args)-> bool
	{
		if (This.IsValid())
		{
			return This->SetSpawnPointForSelectedBuildings(Args...);
		}
		return false;
	});
	ExecuteCommandToSelectedActors<ASquad>([This = TWeakObjectPtr<ThisClass>(this)](auto&& ...Args)-> bool
	{
		if (This.IsValid())
		{
			return This->MoveSquadron(Args...);
		}
		return false;
	});
}

void ARTSPlayerController::DamagePressed()
{
}

TArray<ABuilding*> ARTSPlayerController::GetSelectedBuildings()
{
	return GetSelectedActorsByType<ABuilding>();
}

TArray<AShip*> ARTSPlayerController::GetSelectedShips()
{
	return GetSelectedActorsByType<AShip>();
}

bool ARTSPlayerController::AttackBySelectedActors(AShip* Ship, FHitResult HitResult)
{
	UAttackComponent* AttackComponent = Ship->FindComponentByClass<UAttackComponent>();
	if (AttackComponent)
	{
		const AActor* AttackedActor = HitResult.GetActor();
		const UAttackComponent* AttackedActorAttackComponent = AttackedActor->FindComponentByClass<UAttackComponent>();
		if (AttackedActorAttackComponent)
		{
			if (AttackComponent->GetCanAttack() &&
				AttackedActor != Ship &&
				AttackedActorAttackComponent->GetCanBeAttacked())
			{
				AttackComponent->RequestAttack(AttackedActor);
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

bool ARTSPlayerController::MoveSquadron(ASquad* Squadron, FHitResult HitResult)
{
	Squadron->MoveTo(FVector(HitResult.Location.X, HitResult.Location.Y, 150));
	return false;
}

void ARTSPlayerController::SaveControlGroup0() { SaveControlGroup(0); }
void ARTSPlayerController::SaveControlGroup1() { SaveControlGroup(1); }
void ARTSPlayerController::SaveControlGroup2() { SaveControlGroup(2); }

void ARTSPlayerController::SaveControlGroup(int32 Index)
{
	if (Index < 0 || Index > 2)
	{
		return;
	}

	// Save control group.
	ControlGroups[Index] = SelectedActors;

	UE_LOG(LogTemp, Log, TEXT("Saved selection to control group %d."), Index);
}

void ARTSPlayerController::LoadControlGroup0() { LoadControlGroup(0); }
void ARTSPlayerController::LoadControlGroup1() { LoadControlGroup(1); }
void ARTSPlayerController::LoadControlGroup2() { LoadControlGroup(2); }

void ARTSPlayerController::LoadControlGroup(int32 Index)
{
	if (Index < 0 || Index > 2)
	{
		return;
	}
	
	SelectedActors = ControlGroups[Index];

	UE_LOG(LogTemp, Log, TEXT("Loaded selection from control group %d."), Index);
}
