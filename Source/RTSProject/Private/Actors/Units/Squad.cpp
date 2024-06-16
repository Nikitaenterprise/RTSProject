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

	TArray<FVector> Positions;
	GeneratePositions(Positions);

	HISMComponent->SetStaticMesh(StaticMeshToUseInHISM);

	for (int32 Index = 0; Index < InitialSquadronSize; Index++)
	{
		FTransform Transform;
		Transform.SetLocation(Positions[Index]);
		Transform.SetScale3D(FVector(HISMScaleFactor));
		HISMComponent->AddInstance(Transform, bUsingWorldTransformForHISM);
	}

	BoidsSystem = NewObject<UBoidsMovementSystem>(this);
	BoidsSystem->InitializeMovementSystem(this, MovementComponent);

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

bool ASquad::CalculateBoidsParameters(int32 InstanceIndex, const FVector& InstanceLocation, FVector& Cohesion, FVector& Alignment, FVector& Separation)
{
	Cohesion = FVector::ZeroVector;
	Alignment = FVector::ZeroVector;
	Separation = FVector::ZeroVector;

	int32 NeighborCount = 0;
	// Cohesion parameter
	FVector CenterOfMass = FVector::ZeroVector;
	// Alignment parameter
	FVector AverageVelocity = FVector::ZeroVector;
	// Separation parameter
	FVector SeparationForce = FVector::ZeroVector;

	for (int32 Index = 0; Index < HISMComponent->GetNumInstances(); Index++)
	{
		if (InstanceIndex == Index)
		{
			continue;
		}

		FTransform Transform;
		HISMComponent->GetInstanceTransform(Index, Transform, bUsingWorldTransformForHISM);
		const FVector& Location = Transform.GetLocation();
		
        if (FVector::Dist(InstanceLocation, Location) < NeighborRadius)
        {
	        NeighborCount++;
	        CenterOfMass += Location;
	        AverageVelocity += Location;
			SeparationForce += (InstanceLocation - Location).GetSafeNormal();
        }
    }

    if (NeighborCount == 0) return false;

    CenterOfMass /= NeighborCount;
	AverageVelocity /= NeighborCount;

	Cohesion = (CenterOfMass - InstanceLocation).GetSafeNormal() * CohesionWeight;
	Alignment = AverageVelocity.GetSafeNormal() * AlignmentWeight;
	Separation = SeparationForce.GetSafeNormal() * SeparationWeight;
	return true;
}

void ASquad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int32 Index = 0; Index < HISMComponent->GetNumInstances(); Index++)
	{
		FTransform Transform;
		HISMComponent->GetInstanceTransform(Index, Transform, bUsingWorldTransformForHISM);
		const FVector& Location = Transform.GetLocation();

		FVector Cohesion = FVector::ZeroVector;
        FVector Alignment = FVector::ZeroVector;
		FVector Separation = FVector::ZeroVector;

		CalculateBoidsParameters(Index, Location, Cohesion, Alignment, Separation);

        const FVector& MoveDirection = Cohesion + Alignment + Separation;

        const FVector& ToNewLocation = (NewLocation - Location).GetSafeNormal() * MaxSpeed;

        // Final direction with speed clamping
        FVector FinalDirection = MoveDirection + ToNewLocation;
        FinalDirection = FinalDirection.GetClampedToMaxSize(MaxSpeed);

        const FVector& NewPosition = Location + FinalDirection * DeltaTime;
		FTransform NewTransform = Transform;
		NewTransform.SetLocation(NewPosition);
		HISMComponent->UpdateInstanceTransform(Index, NewTransform, bUsingWorldTransformForHISM);
	}
}

void ASquad::MoveTo(const FVector& Location)
{
	NewLocation = Location;
	//BoidSystem->RequestMoveTo(Location);
}
