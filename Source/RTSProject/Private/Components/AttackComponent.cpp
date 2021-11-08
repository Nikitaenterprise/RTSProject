#include "Components/AttackComponent.h"

#include "Core/RTSPlayerController.h"
#include "Actors/Ship.h"


UAttackComponent::UAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UAttackComponent::InitializeComponent()
{
	Super::InitializeComponent();

	AActor* TestOwner = Cast<AActor>(GetOwner());
	if (!IsValid(TestOwner))
	{
		UE_LOG(LogTemp, Error, TEXT("TestOwner is nullptr in UAttackComponent::InitializeComponent()"));
		return;
	}
	Owner = TestOwner;

	AShip* TestOwnerShip = Cast<AShip>(Owner);
	if (!TestOwnerShip)
	{
		UE_LOG(LogTemp, Error, TEXT("TestOwnerShip is nullptr in UAttackComponent::InitializeComponent() it's because this component is not part of AShip class"));
		return;
	}
	ARTSPlayerController* TestController = TestOwnerShip->PlayerController;
	if (!IsValid(TestController))
	{
		UE_LOG(LogTemp, Error, TEXT("TestController is nullptr in UAttackComponent::InitializeComponent()"));
		return;
	}
	PlayerController = TestController;
}


void UAttackComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttackComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}