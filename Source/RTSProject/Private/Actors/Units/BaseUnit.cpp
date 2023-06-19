#include "Actors/Units/BaseUnit.h"

#include "Core/RTSPlayerController.h"
#include "Systems/RTSPlayerState.h"


ABaseUnit::ABaseUnit()
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
		if (const auto* PlayerPawn = RTSPlayerController->GetPawn())
		{
			RTSPlayerState = PlayerPawn->GetPlayerState<ARTSPlayerState>();
		}
	}
	
	Super::PreInitializeComponents();
}

void ABaseUnit::BeginPlay()
{
	Super::BeginPlay();
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