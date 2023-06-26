#include "Actors/Units/BaseUnit.h"

#include "Actors/RTSPlayer.h"
#include "Core/RTSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Systems/RTSPlayerState.h"


ABaseUnit::ABaseUnit(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseUnit::PreInitializeComponents()
{
	PlayerController = Cast<ARTSPlayerController>(GetOwner());
	if (IsValid(PlayerController) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ABaseUnit::PreInitializeComponents() PlayerController is nullptr"));
	}

	if (const auto* RTSPlayerController = Cast<AController>(GetOwner()))
	{
		if (auto* PlayerPawn = RTSPlayerController->GetPawn())
		{
			RTSPlayer = Cast<ARTSPlayer>(PlayerPawn);
			RTSPlayerState = PlayerPawn->GetPlayerState<ARTSPlayerState>();
		}
	}
	
	Super::PreInitializeComponents();
}

void ABaseUnit::BeginPlay()
{
	Super::BeginPlay();

	bJustCreated = true;
	
	if (HasAnyFlags(RF_WasLoaded))
	{
		// Properly initialize unit if added to level in editor
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARTSPlayer::StaticClass(), Actors);
		for (const auto& Actor : Actors)
		{
			auto* Player = Cast<ARTSPlayer>(Actor);
			if (Player == nullptr)
			{
				continue;
			}
			
			auto* TempRTSPlayerState = Player->GetPlayerState<ARTSPlayerState>();
			if (TempRTSPlayerState == nullptr)
			{
				continue;
			}

			if (TempRTSPlayerState->GetGenericTeamId() == TeamId)
			{
				RTSPlayer = Player;
				PlayerController = Cast<ARTSPlayerController>(RTSPlayer->Controller);
				RTSPlayerState = TempRTSPlayerState;
				RTSPlayerState->AddToPlayersUnits(this);
				break;
			}
		}
		bJustCreated = false;
	}
}

void ABaseUnit::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (RTSPlayerState)
	{
		RTSPlayerState->RemoveFromPlayersUnits(this);
	}
	
	if (PlayerController)
	{
		PlayerController->RemoveFromSelectedActors(this);
	}
	
	Super::EndPlay(EndPlayReason);
}