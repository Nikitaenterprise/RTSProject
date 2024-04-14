#include "Actors/Units/Squad.h"

#include "Actors/Units/Fighter.h"
#include "Components/Movement/SimpleMovementComponent.h"
#include "Interfaces/Squadable.h"
#include "Kismet/KismetMathLibrary.h"
#include "Systems/UnitMovement/BoidsMovementSystem.h"

ASquad::ASquad(const FObjectInitializer& OI) : Super(OI)
{
	MovementComponent = CreateDefaultSubobject<USimpleMovementComponent>(TEXT("SimpleMovementComponent"));
}

void ASquad::BeginPlay()
{
	Super::BeginPlay();

	Squadron.Reserve(InitialSquadronSize);
	for (int32 Index = 0; Index < InitialSquadronSize; Index++)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		const auto& Location = UKismetMathLibrary::RandomPointInBoundingBox(GetActorLocation(), FVector(100, 100, 100));
		AActor* NewFighter = GetWorld()->SpawnActor<AFighter>(FighterClass, Location, FRotator::ZeroRotator, Params);
		Squadron.Add(NewFighter);
	}
	SquadLeader = Squadron[0];

	BoidSystem = NewObject<UBoidsMovementSystem>(this);
	BoidSystem->InitializeMovementSystem(this, MovementComponent);
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

void ASquad::MoveTo(const FVector& Location)
{
	BoidSystem->RequestMoveTo(Location);
}
