#include "Actors/Units/Squad.h"

#include "Actors/Units/Fighter.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/Movement/SimpleMovementComponent.h"
#include "Interfaces/Squadable.h"
#include "Kismet/KismetMathLibrary.h"
#include "Systems/UnitMovement/BoidsMovementSystem.h"

ASquad::ASquad(const FObjectInitializer& OI) : Super(OI)
{
	MovementComponent = CreateDefaultSubobject<USimpleMovementComponent>(TEXT("SimpleMovementComponent"));
	HISMComponent = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HISMComponent"));
}

void ASquad::BeginPlay()
{
	Super::BeginPlay();

	Squadron.Reserve(InitialSquadronSize);
	TArray<FVector> Positions;
	GeneratePositions(Positions);

	for (int32 Index = 0; Index < InitialSquadronSize; Index++)
	{
		/*FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* NewFighter = GetWorld()->SpawnActor<AFighter>(FighterClass, Positions[Index], FRotator::ZeroRotator, Params);*/
		FTransform Transform;
		Transform.SetLocation(Positions[Index]);
		HISMComponent->AddInstance(Transform);
		Squadron.Add(NewFighter);
	}
	SquadLeader = Squadron[0];

	BoidSystem = NewObject<UBoidsMovementSystem>(this);
	BoidSystem->InitializeMovementSystem(this, MovementComponent);

	FTimerHandle Temp;
	GetWorld()->GetTimerManager().SetTimer(Temp, [this]()
	{
		MoveTo(FVector(0, 0, 0));
	}, 4, false);
}

void ASquad::GeneratePositions(TArray<FVector>& OutPositions)
{
	OutPositions.Empty(InitialSquadronSize);
	const FVector& InitialPosition = GetActorLocation();
	const FVector& Backward = GetActorForwardVector() * -1;
	OutPositions.Add(InitialPosition);

	int32 Rows = 0;
	int32 TotalSize = 0;
	while (TotalSize <= InitialSquadronSize)
	{
		Rows++;
		TotalSize = Rows * (Rows + 1) / 2;
	}
	Rows -= 1;

	for (int32 I = 1; I < Rows; I++)
	{
		//const int32 NumFightersInRow = I + 2;
		for (int32 J = -I; J <= I; J++)
		{
			int32 X = I;
			int32 Y = J;
			FVector Position = InitialPosition + Backward * Spacing * FVector(X, Y, 0);
			OutPositions.Add(Position);
		}
	}
}

FVector ASquad::CalculateCohesion(AActor* Fighter)
{
	FVector CenterOfMass(0.f);
    int32 NeighborCount = 0;

    for (AActor* Other : Squadron)
    {
        if (Other != Fighter && FVector::Dist(Fighter->GetActorLocation(), Other->GetActorLocation()) < NeighborRadius)
        {
            CenterOfMass += Other->GetActorLocation();
            NeighborCount++;
        }
    }

    if (NeighborCount == 0) return FVector(0.f);

    CenterOfMass /= NeighborCount;
    return (CenterOfMass - Fighter->GetActorLocation()).GetSafeNormal() * CohesionWeight;
}

FVector ASquad::CalculateAlignment(AActor* Fighter)
{
	FVector AverageVelocity(0.f);
    int32 NeighborCount = 0;

    for (AActor* Other : Squadron)
    {
        if (Other != Fighter && FVector::Dist(Fighter->GetActorLocation(), Other->GetActorLocation()) < NeighborRadius)
        {
            AverageVelocity += Other->GetVelocity();
            NeighborCount++;
        }
    }

    if (NeighborCount == 0) return FVector(0.f);

    AverageVelocity /= NeighborCount;
    return AverageVelocity.GetSafeNormal() * AlignmentWeight;

}

FVector ASquad::CalculateSeparation(AActor* Fighter)
{
	FVector SeparationForce(0.f);

    for (AActor* Other : Squadron)
    {
        if (Other != Fighter && FVector::Dist(Fighter->GetActorLocation(), Other->GetActorLocation()) < NeighborRadius)
        {
            SeparationForce += (Fighter->GetActorLocation() - Other->GetActorLocation()).GetSafeNormal();
        }
    }

    return SeparationForce.GetSafeNormal() * SeparationWeight;
}

void ASquad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Squadron.Num() == 0)
	{
		return;
	}

	for (AActor* Fighter : Squadron)
    {
        FVector Cohesion = CalculateCohesion(Fighter);
        FVector Alignment = CalculateAlignment(Fighter);
        FVector Separation = CalculateSeparation(Fighter);

        FVector MoveDirection = Cohesion + Alignment + Separation;

        // Направление к новой локации
        FVector ToNewLocation = (NewLocation - Fighter->GetActorLocation()).GetSafeNormal() * MaxSpeed;

        // Итоговое направление
        FVector FinalDirection = MoveDirection + ToNewLocation;

        // Ограничение скорости
        FinalDirection = FinalDirection.GetClampedToMaxSize(MaxSpeed);

        // Обновление позиции
        FVector NewPosition = Fighter->GetActorLocation() + FinalDirection * DeltaTime;
        Fighter->SetActorLocation(NewPosition);
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
	NewLocation = Location;
	//BoidSystem->RequestMoveTo(Location);
}
