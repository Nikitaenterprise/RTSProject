#include "Components/Movement/SimpleMovementComponent.h"

USimpleMovementComponent::USimpleMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USimpleMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USimpleMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

