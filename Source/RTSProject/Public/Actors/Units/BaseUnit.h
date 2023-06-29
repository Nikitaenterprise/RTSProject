#pragma once

#include "GenericTeamAgentInterface.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/Selectable.h"
#include "BaseUnit.generated.h"

class UUnitIndicatorAdditionalDescriptionWidget;
class ARTSPlayerController;
class ARTSPlayer;
class ARTSPlayerState;
class USceneComponent;
class UUnitIndicatorComponent;

UCLASS()
class RTSPROJECT_API ABaseUnit
	: public APawn,
	  public ISelectable,
	  public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ABaseUnit(const FObjectInitializer& ObjectInitializer);
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	ARTSPlayerState* GetRTSPlayerState() const { return RTSPlayerState; }
	
	// Begin ISelectable override
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Selected(bool bInIsSelected);
	virtual void Selected_Implementation(bool bInIsSelected) override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Highlighted(bool bInIsHighlighted);
	virtual void Highlighted_Implementation(bool bInIsHighlighted) override;
	// End ISelectable override

	// Begin IGenericTeamAgentInterface override
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override { TeamId = InTeamID; }
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	// End IGenericTeamAgentInterface override

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base")
	USceneComponent* SceneComponent = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BaseUnit")
	UUnitIndicatorComponent* UnitIndicatorComponent {nullptr};
	
	UPROPERTY(BlueprintReadOnly, Category = "BaseUnit")
	ARTSPlayerController* PlayerController {nullptr};
	
	UPROPERTY(BlueprintReadOnly, Category = "BaseUnit")
	ARTSPlayerState* RTSPlayerState {nullptr};

	UPROPERTY(BlueprintReadOnly, Category = "BaseUnit")
	ARTSPlayer* RTSPlayer {nullptr};

	UPROPERTY(BlueprintReadOnly)
	bool bIsSelected = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsHighlighted = false;
	
	bool bJustCreated = false;
	
	UPROPERTY(EditInstanceOnly)
	FGenericTeamId TeamId;
};
