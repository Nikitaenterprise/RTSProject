#pragma once

#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerState.h"
#include "RTSPlayerState.generated.h"

UCLASS()
class RTSPROJECT_API ARTSPlayerState
	: public APlayerState,
	  public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ARTSPlayerState();

	/** Begin IGenericTeamAgentInterface */
	UFUNCTION(BlueprintCallable)
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamId) override;
	/** End IGenericTeamAgentInterface */

	void AddToPlayersUnits(AActor* ActorToAdd) { PlayersUnits.AddUnique(ActorToAdd); }
	void RemoveFromPlayersUnits(AActor* ActorToRemove) { PlayersUnits.Remove(ActorToRemove); }
	const TArray<AActor*>& GetPlayersUnits() const { return PlayersUnits; }
	TArray<AActor*>& GetPlayersUnits() { return PlayersUnits; }
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> PlayersUnits;
	
	FGenericTeamId TeamId;
};
