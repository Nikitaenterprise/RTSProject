#include "Actors/Units/Fighter.h"

#include "Components/Movement/BoidMovementComponent.h"

AFighter::AFighter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USimpleMovementComponent>(FName("SimpleMovementComponent")))
{
	MovementComponent = CreateDefaultSubobject<USimpleMovementComponent>(TEXT("SimpleMovementComponent"));
}

void AFighter::BeginPlay()
{
	Super::BeginPlay();
}

void AFighter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

