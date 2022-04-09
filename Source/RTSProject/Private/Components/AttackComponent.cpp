#include "Components/AttackComponent.h"

#include "Core/RTSPlayerController.h"
#include "Actors/Ship.h"


UAttackComponent::UAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UAttackComponent::InitializeComponent()
{
	Super::InitializeComponent();

	AShip* TestOwnerShip = Cast<AShip>(GetOwner());
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