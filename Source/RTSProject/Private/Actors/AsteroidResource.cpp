#include "Actors/AsteroidResource.h"

#include "Actors/AsteroidField.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"


void AAsteroidResource::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ResourceAmount <= 0)
	{
		IsEmpty();
	}
	// Rotate asteroid
	const FRotator Rotation = StaticMesh->GetComponentRotation();
	SetActorRotation(FRotator(Rotation.Pitch, Rotation.Yaw + (Omega * DeltaSeconds), Rotation.Roll), ETeleportType::None);

}

void AAsteroidResource::IsEmpty()
{
	const auto AsteroidField = Cast<AAsteroidField>(GetOwner());
	if (AsteroidField)
	{
		AsteroidField->RemoveAsteroidFromField(this);
	}
}

void AAsteroidResource::BeginPlay()
{
	Super::BeginPlay();

	// Set rotation speed
	Omega = UKismetMathLibrary::RandomFloatInRange(10, 50);
	Omega = UKismetMathLibrary::RandomBool() ? -Omega : Omega;
}
