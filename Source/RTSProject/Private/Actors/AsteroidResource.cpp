#include "Actors/AsteroidResource.h"
#include "Actors/AsteroidField.h"
#include "Components/StaticMeshComponent.h"
#include "GAS/ResourceSourceAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"


AAsteroidResource::AAsteroidResource()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	PrimaryActorTick.bCanEverTick = true;
	ResourceType = EResourceType::Asteroid;
}

void AAsteroidResource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Rotate asteroid
	const FRotator Rotation = StaticMeshComponent->GetComponentRotation();
	SetActorRotation(FRotator(Rotation.Pitch, Rotation.Yaw + (RotationSpeed * DeltaTime), Rotation.Roll), ETeleportType::None);
}

void AAsteroidResource::BeginPlay()
{
	Super::BeginPlay();

	// Set rotation speed
	RotationSpeed = UKismetMathLibrary::RandomFloatInRange(10, 50);
	RotationSpeed = UKismetMathLibrary::RandomBool() ? -RotationSpeed : RotationSpeed;
}

void AAsteroidResource::CheckCapacity(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0)
	{
		const auto AsteroidField = Cast<AAsteroidField>(GetOwner());
		if (AsteroidField)
		{
			AsteroidField->RemoveAsteroidFromField(this);
		}
		Destroy();
	}
}

float AAsteroidResource::SetupInitialCapacity()
{
	const int SquaredSizeOfMesh = UKismetMathLibrary::VSizeSquared(GetActorScale3D());
	if (UKismetMathLibrary::Round(SquaredSizeOfMesh) != 0)
	{
		const auto AttributeSet = AbilitySystemComponent->GetSet<UResourceSourceAttributeSet>();
		if (AttributeSet)
		{
			InitialCapacity = UKismetMathLibrary::FTrunc(SquaredSizeOfMesh * SizeModifier);
			return InitialCapacity;	
		}
	}
	return 0;
}
