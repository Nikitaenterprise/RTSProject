#include "Actors/Resources/AsteroidField.h"
#include "Core/RTSPlayerController.h"
#include "Actors/Resources/AsteroidResource.h"
#include "Core/FactoryAssets.h"
#include "Actors/Resources/ResourceManager.h"
#include "Algo/Copy.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Core/RTSGameMode.h"
#include "UI/BasicValueViewer.h"
#include "GAS/ResourceSourceAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "Miscellaneous/FactoriesFunctionLibrary.h"

AAsteroidField::AAsteroidField()
{
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
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ResourceComponent = CreateDefaultSubobject<UResourceComponent>(TEXT("ResourceComponent"));
	ResourceComponent->SetResourceType(EResourceType::AsteroidField);
	PrimaryActorTick.bCanEverTick = false;
}

void AAsteroidField::BeginPlay()
{
	Super::BeginPlay();
	
	WidgetComponent->InitWidget();
	WidgetComponent->SetVisibility(false);

	ResourceComponent->InitializeCapacity([]()->float
	{
		return 0;
	});

	AddRandomNumberOfAsteroidsToField();

	if (auto* ResourceManager = ARTSGameMode::GetRTSGameMode(GetWorld())->GetResourceManager())
	{
		ResourceManager->AddAsteroidField(this);
	}
}

void AAsteroidField::Selected_Implementation(bool bIsSelected)
{
	// We can't select field
}

void AAsteroidField::Highlighted_Implementation(bool bIsHighlighted)
{
	if (WidgetComponent)
	{
		if (bIsHighlighted)
		{
			if (auto* BasicValueViewerWidget = Cast<UBasicValueViewer>(WidgetComponent->GetWidget()))
			{
				BasicValueViewerWidget->SetText(ResourceComponent->GetResourceSourceAttributeSet()->GetResourceCapacity());
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
	RemoveAsteroidFromField(Asteroids[Index]);
	if (GetNumberOfAsteroidsInField() == 0)
	{
		Destroy();
	}
}

void AAsteroidField::RemoveAsteroidFromField(AAsteroidResource* Asteroid)
{
	Asteroids.Remove(Asteroid);
	// Possible slow speed operation
	Asteroids.Shrink();
	
	const auto AsteroidResourceComponent = Asteroid->GetResourceComponent();
	if (!AsteroidResourceComponent)
	{
		return;
	}
	const auto AsteroidCapacity = AsteroidResourceComponent->GetResourceSourceAttributeSet()->GetResourceCapacity();
	const auto FieldCapacity = ResourceComponent->GetResourceSourceAttributeSet()->GetResourceCapacity();
	ResourceComponent->ModifyResource([FieldCapacity, AsteroidCapacity]()->float
	{
		return FieldCapacity - AsteroidCapacity;
	});
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
	const FVector BoxExtent = BoxCollision->GetScaledBoxExtent();
	// Displace asteroid inside box on random dx, dy, dz
	FVector Location(
		UKismetMathLibrary::RandomFloatInRange(BoxExtent.X * -1, BoxExtent.X),
		UKismetMathLibrary::RandomFloatInRange(BoxExtent.Y * -1, BoxExtent.Y),
		UKismetMathLibrary::RandomFloatInRange(BoxExtent.Z * -1, BoxExtent.Z)
	);
	Location += GetActorLocation();
	// Set random rotation to this asteroid
	const FRotator Rotation(
		UKismetMathLibrary::RandomFloatInRange(0, 360),
		UKismetMathLibrary::RandomFloatInRange(0, 360),
		UKismetMathLibrary::RandomFloatInRange(0, 360)
	);
	// Set random scale
	const FVector Scale(UKismetMathLibrary::RandomFloatInRange(0.5, 1.5));
	const FTransform Transform(Rotation, Location, Scale);

	const auto* GameMode = Cast<ARTSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("AAsteroidField::CreateAsteroidAndAddItToField ARTSGameMode is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("AAsteroidField::CreateAsteroidAndAddItToField ARTSGameMode is nullptr"));
		return;
	}
	
	const TSubclassOf<AAsteroidResource> AsteroidResourceClass = GameMode->GetFactoryAssets()->GetAsteroidResourceClass(0);
	AAsteroidResource* SpawnedAsteroid = UFactoriesFunctionLibrary::NewAsteroid(GetWorld(), AsteroidResourceClass, this, Transform);
	if (!SpawnedAsteroid)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn asteroid"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn asteroid"));
		return;
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

	const auto AsteroidResourceComponent = AsteroidToAdd->GetResourceComponent();
	if (!AsteroidResourceComponent)
	{
		return;
	}
	const auto AsteroidCapacity = AsteroidResourceComponent->GetResourceSourceAttributeSet()->GetResourceCapacity();
	const auto FieldCapacity = ResourceComponent->GetResourceSourceAttributeSet()->GetResourceCapacity();
	ResourceComponent->ModifyResource([FieldCapacity, AsteroidCapacity]()->float
	{
		return FieldCapacity + AsteroidCapacity;
	});
}