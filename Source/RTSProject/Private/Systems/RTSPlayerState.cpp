#include "Systems/RTSPlayerState.h"


ARTSPlayerState::ARTSPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARTSPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ARTSPlayerState::SetGenericTeamId(const FGenericTeamId& InTeamId)
{
	TeamId = InTeamId;
}

