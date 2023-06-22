#include "Actors/Units/BaseUnit.h"

#include "Actors/RTSPlayer.h"
#include "Core/RTSPlayerController.h"
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
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr in AShip::BeginPlay()"));
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