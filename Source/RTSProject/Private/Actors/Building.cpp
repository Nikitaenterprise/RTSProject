#include "Actors/Building.h"

#include "AbilitySystemComponent.h"
#include "Miscellaneous/FactoriesFunctionLibrary.h"
#include "Components/FogOfWarInfluencerComponent.h"
#include "Core/RTSPlayerController.h"
#include "UI/HealthShieldBarHUD.h"
#include "Components/HealthShieldComponent.h"
#include "Components/MiniMapIconComponent.h"
#include "Components/MiniMapInfluencerComponent.h"
#include "Actors/Ship.h"
#include "UI/GameHUD.h"
#include "UI/SelectionRectangleWidget.h"
#include "AttributeSet.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GAS/BuildingAttributeSet.h"
#include "UI/SelectionRectangleWidget.h"

ABuilding::ABuilding()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());

	HealthShieldBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthShieldBar"));
	HealthShieldBar->SetupAttachment(GetRootComponent());
	HealthShieldBar->SetDrawSize(FVector2D(150, 150));
	HealthShieldBar->SetWidgetSpace(EWidgetSpace::Screen);

	FOWInfluencerComponent = CreateDefaultSubobject<UFogOfWarInfluencerComponent>(TEXT("FOWInfluencerComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	BuildingAttributeSet = CreateDefaultSubobject<UBuildingAttributeSet>(TEXT("BuildingAttributeSet"));
	SpawnPoint = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SpawnPoint"));
	HealthShieldComponent = CreateDefaultSubobject<UHealthShieldComponent>(TEXT("HealthShieldComponent"));
	MiniMapInfluencerComponent = CreateDefaultSubobject<UMiniMapInfluencerComponent>(TEXT("MiniMapInfluencerComponent"));
	MiniMapIconComponent = CreateDefaultSubobject<UMiniMapIconComponent>(TEXT("MiniMapIconComponent"));
}

void ABuilding::BeginPlay()
{
	Super::BeginPlay();

	bJustCreated = true;
	
	ARTSPlayerController* TestController = Cast<ARTSPlayerController>(GetOwner());
	if (!IsValid(TestController))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr in ABuilding::BeginPlay()"));
		return;
	}
	PlayerController = TestController;
	PlayerController->PlayersActors.AddUnique(this);

	HealthShieldBarHUD = Cast<UHealthShieldBarHUD>(HealthShieldBar->GetWidget());
	HealthShieldBarHUD->BindHealthShieldValues(HealthShieldComponent->GetHealthPercentPtr(), HealthShieldComponent->GetShieldPercentPtr());
	HealthShieldBar->SetVisibility(false);

	DebugInputComponent = PlayerController->InputComponent;
	if (!IsValid(DebugInputComponent))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("InputComponent is nullptr in AShip::Initialize()"));
		UE_LOG(LogTemp, Error, TEXT("InputComponent is nullptr in AShip::Initialize()"));
	}
	DebugInputComponent->BindAction(TEXT("LMB"), IE_Pressed, this, &ABuilding::LMBPressed);
	DebugInputComponent->BindAction(TEXT("LMB"), IE_Released, this, &ABuilding::LMBReleased);

	SpawnPoint->SetRelativeLocation(FVector(150, 0, 0));
	SpawnPoint->SetVisibility(false);
	
	AbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(BuildingAttributeSet);
	BuildingUnitHandle = AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(BuildUnitAbility, 1, INDEX_NONE, this));
	BuildingAttributeSet->OnHealthZeroed.BindLambda([This = TWeakObjectPtr<ThisClass>(this)]()
	{
		if (This.IsValid())
		{
			This->Destroy();
		}
	});
}

void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HealthShieldComponent->IsDead())
	{
		Destroy();
	}

	if (bJustCreated && !bLMBPressed)
	{
		if (PlayerController->GameHUD) PlayerController->GameHUD->LockSelectionRectangle();
		UpdatePositionWhenCreated();
	}
	else if (bLMBPressed)
	{
		bJustCreated = false;
		if (PlayerController->GameHUD) PlayerController->GameHUD->UnlockSelectionRectangle();
	}
	//if (ConstructionState == EConstructionState::RequestedConstruction) StartBuildingUnit();
}

void ABuilding::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	PlayerController->PlayersActors.Remove(this);
	//SpawnEmitterAtLocation()
	Super::EndPlay(EndPlayReason);
}

void ABuilding::LMBPressed()
{
	bLMBPressed = true;
}

void ABuilding::LMBReleased()
{
	bLMBPressed = false;
}

void ABuilding::Selected_Implementation(bool _bIsSelected)
{
	bIsSelected = _bIsSelected;
	if (bIsSelected)
	{
		HealthShieldBar->SetVisibility(true);
		SpawnPoint->SetVisibility(true);
	}
	else
	{
		HealthShieldBar->SetVisibility(false);
		SpawnPoint->SetVisibility(false);
	}
}

void ABuilding::Highlighted_Implementation(bool _bIsHighlighted)
{
	if (!bIsSelected)
	{
		bIsHighlighted = _bIsHighlighted;
		if (bIsHighlighted)
		{
			HealthShieldBar->SetVisibility(true);
		}
		else
		{
			HealthShieldBar->SetVisibility(false);
		}
	}
}

void ABuilding::UpdatePositionWhenCreated()
{
	FHitResult Hit;
	const bool bHit = PlayerController->GetHitResultUnderCursorByChannel(
				UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), 
				false,
				Hit);

	FVector Location;
	if (bHit) Location = Hit.Location;
	else Location = FVector(0, 0, 0);
	// Clamp to ground
	Location.Z = 0;
	
	SetActorLocation(Location, false, nullptr, ETeleportType::None);
}

int ABuilding::GetBuildingQueueSizeByClass(TSubclassOf<AActor> ActorClass) const
{
	if (BuildingQueue.Num() == 0) return 0;
	int NumberOfActorsToBuild = 0;
	for(auto& a : BuildingQueue)
	{
		if (a == ActorClass) NumberOfActorsToBuild++;
	}
	return NumberOfActorsToBuild;
}

void ABuilding::RequestBuildingUnit(TSubclassOf<AActor> ActorClass)
{
	BuildingQueue.Add(ActorClass);
	if (BuildingUnitHandle.IsValid())
	{
		if (!bIsBuildingUnit)
		{
			AbilitySystemComponent->TryActivateAbility(BuildingUnitHandle, false);
		}
	}
}