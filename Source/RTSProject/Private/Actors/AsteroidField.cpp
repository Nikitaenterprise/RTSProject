#include "Actors/AsteroidField.h"

#include "Core/RTSPlayerController.h"
#include "Actors/AsteroidResource.h"
#include "Core/FactoryAssets.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

AAsteroidField::AAsteroidField()
{
 	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(GetRootComponent());
}

void AAsteroidField::BeginPlay()
{
	Super::BeginPlay();

	ARTSPlayerController* TestController = Cast<ARTSPlayerController>(GetOwner());
	if (!IsValid(TestController))
	{
		UE_LOG(LogTemp, Error, TEXT("ARTSPlayerController is nullptr in AAsteroidField::BeginPlay()"));
		return;
	}
	PlayerController = TestController;
}

void AAsteroidField::Tick(float MainDeltaTime)
{
	Super::Tick(MainDeltaTime);
}

void AAsteroidField::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AAsteroidField::RemoveRandomAsteroidFromField()
{
	int Index = UKismetMathLibrary::RandomIntegerInRange(0, RecalculateNumberOfAsteroidsInField());
	RemoveAsteroidFromField(Asteroids[Index]);
}


void AAsteroidField::RemoveAsteroidFromField(AAsteroidResource* Asteroid)
{
	Asteroids.Remove(Asteroid);
	NumberOfAsteroids--;
	Asteroid->Destroy(false, true);

	// Possible slow speed operation
	Asteroids.Shrink();
}

int AAsteroidField::RecalculateNumberOfAsteroidsInField()
{
	int Size = Asteroids.Num();
	if (Size == NumberOfAsteroids) return NumberOfAsteroids;
	NumberOfAsteroids = Size;
	return NumberOfAsteroids;
}

void AAsteroidField::AddRandomNumberOfAsteroidsToField(int MinValue, int MaxValue)
{
	const int NumberOfAsteroidsToAdd = UKismetMathLibrary::RandomIntegerInRange(MinValue, MaxValue);
	for (int i = 0; i < NumberOfAsteroidsToAdd; ++i)
	{
		AddAsteroidToField();
	}
}

void AAsteroidField::AddAsteroidToField()
{
	FVector BoxExtent = BoxCollision->GetScaledBoxExtent();
	// Displace asteroid inside box on random dx, dy, dz
	FVector Location(
		UKismetMathLibrary::RandomFloatInRange(BoxExtent.X * -1, BoxExtent.X),
		UKismetMathLibrary::RandomFloatInRange(BoxExtent.Y * -1, BoxExtent.Y),
		UKismetMathLibrary::RandomFloatInRange(BoxExtent.Z * -1, BoxExtent.Z)
	);
	Location += GetActorLocation();
	// Set random rotation to this asteroid
	FRotator Rotation(
		UKismetMathLibrary::RandomFloatInRange(0, 360),
		UKismetMathLibrary::RandomFloatInRange(0, 360),
		UKismetMathLibrary::RandomFloatInRange(0, 360)
	);
	// Set random scale
	FVector Scale(UKismetMathLibrary::RandomFloatInRange(0.5, 1.5));

	FTransform Transform(Rotation, Location, Scale);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	Params.Instigator = nullptr;
	Params.Owner = this;
	
	const TSubclassOf<AAsteroidResource> AsteroidResourceClass = PlayerController->GetFactoryAssets()->GetAsteroidResourceClass(0);
	AAsteroidResource* SpawnedAsteroid = GetWorld()->SpawnActor<AAsteroidResource>(AsteroidResourceClass.Get(), Transform, Params);
	if (SpawnedAsteroid)
	{
		Asteroids.AddUnique(SpawnedAsteroid);
		NumberOfAsteroids++;
	}
	else GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn asteroid"));
}
