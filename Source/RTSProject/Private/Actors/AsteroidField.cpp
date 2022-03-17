#include "Actors/AsteroidField.h"

#include "Core/RTSPlayerController.h"
#include "Actors/AsteroidResource.h"
#include "Core/FactoryAssets.h"
#include "Actors/ResourceManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/BasicValueViewer.h"
#include "GAS/ResourceSourceAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "Miscellaneous/FactoriesFunctionLibrary.h"

AAsteroidField::AAsteroidField()
{
 	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(GetRootComponent());
	BoxCollision->SetSimulatePhysics(false);
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(GetRootComponent());
	WidgetComponent->SetGeometryMode(EWidgetGeometryMode::Plane);
	WidgetComponent->SetTwoSided(false);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
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
	
	WidgetComponent->InitWidget();
	WidgetComponent->SetVisibility(false);
}

void AAsteroidField::Selected_Implementation(bool bIsSelected)
{
}

void AAsteroidField::Highlighted_Implementation(bool bIsHighlighted)
{
	if (WidgetComponent)
	{
		if (bIsHighlighted)
		{
			const auto BasicValueViewerWidget = Cast<UBasicValueViewer>(WidgetComponent->GetWidget());
			if (BasicValueViewerWidget)
			{
				BasicValueViewerWidget->SetValue(ResourceSourceAttributeSet->GetResourceCapacity());
			}
			WidgetComponent->SetVisibility(true);
		}
		else
		{
			WidgetComponent->SetVisibility(false);
		}
	}
}

void AAsteroidField::RemoveRandomAsteroidFromField()
{
	const int32 Index = UKismetMathLibrary::RandomIntegerInRange(0, GetNumberOfAsteroidsInField());
	const auto Asteroid = Asteroids[Index];
	const auto AsteroidCapacity = Asteroid->GetResourceSourceAttributeSet()->GetResourceCapacity();
	const auto FieldCapacity = GetResourceSourceAttributeSet()->GetResourceCapacity();
	CalculateResource([&]()->float
	{
		return FieldCapacity - AsteroidCapacity;
	});
	RemoveAsteroidFromField(Asteroids[Index]);
}

void AAsteroidField::RemoveAsteroidFromField(AAsteroidResource* Asteroid)
{
	Asteroids.Remove(Asteroid);

	// Possible slow speed operation
	Asteroids.Shrink();
	const auto AsteroidCapacity = Asteroid->GetResourceSourceAttributeSet()->GetResourceCapacity();
	const auto FieldCapacity = GetResourceSourceAttributeSet()->GetResourceCapacity();
	CalculateResource([&]()->float
	{
		return FieldCapacity - AsteroidCapacity;
	});
}

float AAsteroidField::InitialCapacity()
{
	return 1;
}

int32 AAsteroidField::GetNumberOfAsteroidsInField()
{
	return Asteroids.Num();
}

void AAsteroidField::AddRandomNumberOfAsteroidsToField(int MinValue, int MaxValue)
{
	const int NumberOfAsteroidsToAdd = UKismetMathLibrary::RandomIntegerInRange(MinValue, MaxValue);
	for (int i = 0; i < NumberOfAsteroidsToAdd; ++i)
	{
		CreateAsteroidAndAddItToField();
	}
}

void AAsteroidField::CreateAsteroidAndAddItToField()
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
	AAsteroidResource* SpawnedAsteroid = UFactoriesFunctionLibrary::NewAsteroid(GetWorld(), AsteroidResourceClass, this, Transform);
	if (!SpawnedAsteroid)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn asteroid"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn asteroid"));
		return;
	}
	if (ResourceManager)
	{
		ResourceManager->AddResource(SpawnedAsteroid);
	}
	AddAsteroidToField(SpawnedAsteroid);
}

void AAsteroidField::AddAsteroidToField(AAsteroidResource* AsteroidToAdd)
{
	if (!AsteroidToAdd)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to add asteroid to field"));
		UE_LOG(LogTemp, Error, TEXT("Failed to add asteroid to field"));
		return;
	}
	Asteroids.AddUnique(AsteroidToAdd);
	AsteroidToAdd->SetIsInAsteroidField(true);
	
	const auto AsteroidCapacity = AsteroidToAdd->GetResourceSourceAttributeSet()->GetResourceCapacity();
	const auto FieldCapacity = GetResourceSourceAttributeSet()->GetResourceCapacity();
	CalculateResource([&]()->float
	{
		return FieldCapacity + AsteroidCapacity;
	});
}
