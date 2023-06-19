#pragma once

#include "GenericTeamAgentInterface.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/Selectable.h"
#include "BaseUnit.generated.h"

UCLASS()
class RTSPROJECT_API ABaseUnit
	: public APawn,
	  public ISelectable,
	  public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ABaseUnit();
	void PreInitializeComponents() override;
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SetPlayerController(ARTSPlayerController* InPlayerController) { PlayerController = InPlayerController; }
	
	// Begin ISelectable override
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Selected(bool bInIsSelected);
	virtual void Selected_Implementation(bool bInIsSelected) override {};
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Highlighted(bool bInIsHighlighted);
	virtual void Highlighted_Implementation(bool bInIsHighlighted) override {};
	// End ISelectable override

	// Begin IGenericTeamAgentInterface override
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override { TeamId = InTeamID; }
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	// End IGenericTeamAgentInterface override

protected:
	UPROPERTY(BlueprintReadOnly, Category = "BaseUnit")
	ARTSPlayerController* PlayerController = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category = "BaseUnit")
	ARTSPlayerState* RTSPlayerState = nullptr;

	UPROPERTY(BlueprintReadOnly)
	bool bIsSelected = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsHighlighted = false;
	
	bool bJustCreated = false;
	
	FGenericTeamId TeamId;
};
