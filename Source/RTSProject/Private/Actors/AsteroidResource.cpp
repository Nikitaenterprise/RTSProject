#include "Actors/AsteroidResource.h"

#include "Actors/AsteroidField.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"


void AAsteroidResource::Tick(float MainDeltaTime)
{
	Super::Tick(MainDeltaTime);

	if (ResourceAmount <= 0)
	{
		Owner->Asteroids.Remove(this);
		Owner->NumberOfAsteroids--;
		Destroy(false, true);
	}
	// Rotate asteroid
	FRotator Rotation = StaticMesh->GetComponentRotation();
	SetActorRotation(FRotator(Rotation.Pitch, Rotation.Yaw + (Omega * DeltaTime), Rotation.Roll), ETeleportType::None);

}

void AAsteroidResource::BeginPlay()
{
	Super::BeginPlay();

	// Set rotation speed
	Omega = UKismetMathLibrary::RandomFloatInRange(10, 50);
	if (UKismetMathLibrary::RandomBool()) Omega *= -1;
}
