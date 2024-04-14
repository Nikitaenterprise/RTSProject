#include "Actors/Units/Fighter.h"

#include "Components/Movement/BoidMovementComponent.h"

AFighter::AFighter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBoidMovementComponent>(FName("BoidMovementComponent")))
{
	MovementComponent = CreateDefaultSubobject<UBoidMovementComponent>(TEXT("BoidMovementComponent"));
}

void AFighter::BeginPlay()
{
	Super::BeginPlay();
}

void AFighter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

