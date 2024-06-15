#include "Systems/UnitMovement/BoidsMovementSystem.h"

#include "Actors/Units/Squad.h"

void UBoidsMovementSystem::InitializeMovementSystem(AActor* InOwnerActor, UPawnMovementComponent* InMovementComponent)
{
	if (ASquad* Squadron = Cast<ASquad>(InOwnerActor))
	{
		FBoidSquadron BoidSquadron;
		BoidSquadron.BoidSquadron = Squadron->GetSquad();
		UnitsRegister.Add(Squadron, BoidSquadron);	
	}
}

bool UBoidsMovementSystem::RequestMoveTo(const FVector& Location)
{
	return true;
}

void UBoidsMovementSystem::CalculatePosition(float DeltaTime)
{
}

void UBoidsMovementSystem::MakePath()
{
}

void UBoidsMovementSystem::Separation(const ASquad* Squadron)
{
	// For every boid in the system, check if it's too close
    for (int32 i = 0; i < UnitsRegister[Squadron].BoidSquadron.Num() - 1; i++)
    {
    	FVector Steer = FVector::ZeroVector;
		int32 Count = 0;
    	for (int32 j = i + 1; j < UnitsRegister[Squadron].BoidSquadron.Num(); j++)
    	{
    		// Calculate distance from current boid to other one
            const float Distance = FVector::Distance(UnitsRegister[Squadron].BoidSquadron[i]->GetActorLocation(), UnitsRegister[Squadron].BoidSquadron[j]->GetActorLocation());
    		// If this is a fellow boid and it's too close, move away from it
    		if (Distance < DesiredSeparation)
    		{
    			FVector Diff = FVector::ZeroVector;
    			Diff = UnitsRegister[Squadron].BoidSquadron[i]->GetActorLocation() - UnitsRegister[Squadron].BoidSquadron[j]->GetActorLocation();
    			Diff.Normalize();
    			Diff /= Distance;      // Weight by distance
    			Steer += Diff;
    			Count++;
    		}
    	}
    	// Adds average difference of location to acceleration
    	if (Count > 0)
    	{
    		Steer /= static_cast<float>(Count);
    	}
    	if (Steer.Size() > 0)
    	{
    		// Steering = Desired - Velocity
    		Steer.Normalize();
    		Steer *= MaxSpeed;
    		Steer -= UnitsRegister[Squadron].BoidSquadron[i]->GetVelocity();
    		if (Steer.Size() > MaxForce)
    		{
    			Steer /= Steer.Size();
    		}
    	}
    }
}

void UBoidsMovementSystem::Alignment(const ASquad* Squadron)
{
	for (int32 i = 0; i < UnitsRegister[Squadron].BoidSquadron.Num() - 1; i++)
	{
		FVector Sum = FVector::ZeroVector;
		int32 Count = 0;
		for (int32 j = i + 1; j < UnitsRegister[Squadron].BoidSquadron.Num(); j++)
		{
			const float Distance = FVector::Distance(UnitsRegister[Squadron].BoidSquadron[i]->GetActorLocation(), UnitsRegister[Squadron].BoidSquadron[j]->GetActorLocation());
			if (Distance < NeighborDist)
			{
				Sum += UnitsRegister[Squadron].BoidSquadron[j]->GetVelocity();
				Count++;
			}
		}
		// If there are boids close enough for alignment...
		if (Count > 0)
		{
			Sum /= static_cast<float>(Count);// Divide sum by the number of close boids (average of velocity)
			Sum.Normalize();            // Turn sum into a unit vector, and
			Sum *= MaxSpeed;    // Multiply by maxSpeed
			// Steer = Desired - Velocity
			FVector Steer;
			Steer = Sum - UnitsRegister[Squadron].BoidSquadron[i]->GetVelocity(); //sum = desired(average)
			if (Steer.Size() > MaxForce)
			{
				Steer /= Steer.Size();
			}
		}
	}
	
}

void UBoidsMovementSystem::Cohesion(const ASquad* Squadron)
{
	for (int32 i = 0; i < UnitsRegister[Squadron].BoidSquadron.Num() - 1; i++)
	{
		FVector Sum = FVector::ZeroVector;
		int32 Count = 0;
		for (int32 j = i + 1; j < UnitsRegister[Squadron].BoidSquadron.Num(); j++)
		{
			const float Distance = FVector::Distance(UnitsRegister[Squadron].BoidSquadron[i]->GetActorLocation(), UnitsRegister[Squadron].BoidSquadron[j]->GetActorLocation());
			if (Distance < NeighborDist)
			{
				Sum += UnitsRegister[Squadron].BoidSquadron[j]->GetActorLocation();
				Count++;
			}
		}
		if (Count > 0)
		{
			Sum /= static_cast<float>(Count);

			FVector Desired;
			Desired -= Sum;  // A vector pointing from the location to the target
			// Normalize desired and scale to maximum speed
			Desired.Normalize();
			Desired *= MaxSpeed;
			// Steering = Desired - Velocity
			FVector Acceleration = FVector::ZeroVector;
			Acceleration = Desired - UnitsRegister[Squadron].BoidSquadron[i]->GetVelocity();
			if (Acceleration.Size() > MaxForce)
			{
				Acceleration /= Acceleration.Size();
			}
		}
	}
}

void UBoidsMovementSystem::Apply()
{
	Separation(nullptr);
	Alignment(nullptr);
	Cohesion(nullptr);
	// Arbitrarily weight these forces
	//sep.mulScalar(1.5);
	//ali.mulScalar(1.0); // Might need to alter weights for different characteristics
	//coh.mulScalar(1.0);
	// Add the force vectors to acceleration
	//applyForce(sep);
	//applyForce(ali);
	//applyForce(coh);
}
