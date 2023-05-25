#include "Actors/Units/Squad.h"

#include "Components/Movement/SimpleMovementComponent.h"
#include "Interfaces/Squadable.h"

ASquad::ASquad()
{
	MovementComponent = CreateDefaultSubobject<USimpleMovementComponent>(TEXT("SimpleMovementComponent"));
}

void ASquad::BeginPlay()
{
	Super::BeginPlay();
}

void ASquad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Squadron.Num() != 0)
	{
		
	}
}

void ASquad::AddToSquad(AActor* InActor)
{
	if (IsValid(InActor) == false)
	{
		return;
	}
	
	if (const ISquadable* Squadable = Cast<ISquadable>(InActor))
	{
		Squadron.Add(InActor);
		auto* ActorMovementComponent = Squadable->GetMovementComponent();
	}
}

void ASquad::RemoveFromSquad(AActor* InActor)
{
	Squadron.Remove(InActor);
}

void ASquad::SetSquadLeader(AActor* InSquadLeader)
{
	if (InSquadLeader && Squadron.Contains(InSquadLeader))
	{
		SquadLeader = InSquadLeader;
	}
}
