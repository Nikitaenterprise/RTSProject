#include "Actors/AsteroidResource.h"
#include "Actors/AsteroidField.h"
#include "Components/StaticMeshComponent.h"
#include "GAS/ResourceSourceAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"


AAsteroidResource::AAsteroidResource()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	
	ResourceType = EResourceType::Asteroid;
}

void AAsteroidResource::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (ResourceSourceAttributeSet)
	{
		if (ResourceSourceAttributeSet->GetResourceCapacity() <= 0)
		{
			IsEmpty();
		}
	}
	// Rotate asteroid
	const FRotator Rotation = StaticMeshComponent->GetComponentRotation();
	SetActorRotation(FRotator(Rotation.Pitch, Rotation.Yaw + (Omega * DeltaSeconds), Rotation.Roll), ETeleportType::None);
}

void AAsteroidResource::IsEmpty()
{
	const auto AsteroidField = Cast<AAsteroidField>(GetOwner());
	if (AsteroidField)
	{
		AsteroidField->RemoveAsteroidFromField(this);
	}
	Destroy();
}

void AAsteroidResource::BeginPlay()
{
	Super::BeginPlay();

	// Set rotation speed
	Omega = UKismetMathLibrary::RandomFloatInRange(10, 50);
	Omega = UKismetMathLibrary::RandomBool() ? -Omega : Omega;
}

float AAsteroidResource::InitialCapacity()
{
	const int SquaredSizeOfMesh = UKismetMathLibrary::VSizeSquared(GetActorScale3D());
	if (UKismetMathLibrary::Round(SquaredSizeOfMesh) != 0)
	{
		const auto AttributeSet = AbilitySystemComponent->GetSet<UResourceSourceAttributeSet>();
		if (AttributeSet)
		{
			return UKismetMathLibrary::FTrunc(SquaredSizeOfMesh * AttributeSet->GetInitResourceCapacity());	
		}
	}
	return 0;
}