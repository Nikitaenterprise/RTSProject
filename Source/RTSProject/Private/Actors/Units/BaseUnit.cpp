#include "Actors/Units/BaseUnit.h"

#include "Actors/RTSPlayer.h"
#include "Components/UnitIndicatorComponent.h"
#include "Core/RTSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Systems/RTSPlayerState.h"
#include "UI/GameHUD.h"


ABaseUnit::ABaseUnit(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	UnitIndicatorComponent = CreateDefaultSubobject<UUnitIndicatorComponent>(TEXT("UnitIndicatorComponent"));
	UnitIndicatorComponent->SetupAttachment(GetRootComponent());
}

void ABaseUnit::PreInitializeComponents()
{
	PlayerController = Cast<ARTSPlayerController>(GetOwner());
	if (IsValid(PlayerController) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ABaseUnit::PreInitializeComponents() PlayerController is nullptr"));
	}
	else
	{
		if (const auto* GameHUD = PlayerController->GetGameHUD())
		{
			MouseCursorWidget = GameHUD->GetMouseCursorWidget();
		}
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
	UnitIndicatorComponent->SetWidgetSpace(EWidgetSpace::Screen);
	UnitIndicatorComponent->SetDrawSize(FVector2D(300, 100));
	
	if (HasAnyFlags(RF_WasLoaded | RF_Transactional))
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

void ABaseUnit::Selected_Implementation(bool bInIsSelected)
{
	ISelectable::Selected_Implementation(bInIsSelected);

	bIsSelected = bInIsSelected;
	if (bIsSelected)
	{
		UnitIndicatorComponent->SetVisibility(true);
	}
	else
	{
		UnitIndicatorComponent->SetVisibility(false);
	}
}

void ABaseUnit::Highlighted_Implementation(bool bInIsHighlighted)
{
	ISelectable::Highlighted_Implementation(bInIsHighlighted);

	if (!bIsSelected)
	{
		bIsHighlighted = bInIsHighlighted;
		if (bIsHighlighted)
		{
			UnitIndicatorComponent->SetVisibility(true);
		}
		else
		{
			UnitIndicatorComponent->SetVisibility(false);
		}
	}
}
