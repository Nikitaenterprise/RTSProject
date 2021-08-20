#include "Components/MiniMapIconComponent.h"

UMiniMapIconComponent::UMiniMapIconComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMiniMapIconComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UMiniMapIconComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

