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

#include "Miscellaneous/AsteroidMeshWorker.h"
#include "Generators/SphereGenerator.h"

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
	
	ARTSPlayerController* TestController = Cast<ARTSPlayerController>(GetOwner());
	if (!IsValid(TestController))
	{
		UE_LOG(LogTemp, Error, TEXT("ARTSPlayerController is nullptr in AAsteroidField::BeginPlay()"));
		return;
	}
	PlayerController = TestController;
	
	WidgetComponent->InitWidget();
	WidgetComponent->SetVisibility(false);

	ResourceComponent->InitializeCapacity([&]()->float
	{
		return 0;
	});
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
			const auto BasicValueViewerWidget = Cast<UBasicValueViewer>(WidgetComponent->GetWidget());
			if (BasicValueViewerWidget)
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
	ResourceComponent->ModifyResource([&]()->float
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
	AsteroidFieldMaker = UAsteroidFieldMaker::AsteroidFieldMaker(GetWorld(), this, Asteroids);
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
	
	const TSubclassOf<AAsteroidResource> AsteroidResourceClass = PlayerController->GetFactoryAssets()->GetAsteroidResourceClass(0);
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
	ResourceComponent->ModifyResource([&]()->float
	{
		return FieldCapacity + AsteroidCapacity;
	});
}

UAsteroidFieldMaker::UAsteroidFieldMaker()
{
}

UAsteroidFieldMaker* UAsteroidFieldMaker::AsteroidFieldMaker(UWorld* ThisWorld, const AAsteroidField* AsteroidField,
                                                             TArray<AAsteroidResource*>& AsteroidsInField)
{
	auto Maker = NewObject<UAsteroidFieldMaker>();
	Maker->World = ThisWorld;
	Maker->Field = AsteroidField;
	Maker->AsteroidsToEdit = &AsteroidsInField;
	if (Maker->IsReady())
	{
		Maker->Start();
	}
	return Maker;
}

void UAsteroidFieldMaker::Start()
{
	NumberOfThreads = AsteroidsToEdit->Num();
	ThreadWorkers.Reserve(NumberOfThreads);
	SpawnedAsteroids2DArray.Reserve(NumberOfThreads);
	ThreadsCompleted.Reserve(NumberOfThreads);
	uint32 Index = 0;
	for(AAsteroidResource* Asteroid : *AsteroidsToEdit)
	{
		//Asteroid->SetActorTickEnabled(false);
		const auto Location = Asteroid->GetActorLocation();
		const auto Vertices = Asteroid->MeshComponent->ExtractMesh(false)->GetVerticesBuffer();
		const auto Triangles = Asteroid->MeshComponent->ExtractMesh(false)->GetTrianglesBuffer();
		FVector VertexPosition = FVector(Vertices[0][0], Vertices[0][1], Vertices[0][2]);
		const float Radius = VertexPosition.Size();

		NumberOfAsteroidsToSpawn = 10;
		TArray<ADynamicSDMCActor*> SpawnedAsteroids;
		SpawnedAsteroids.Reserve(NumberOfAsteroidsToSpawn);
		for (uint32 i = 0; i < NumberOfAsteroidsToSpawn; i++)
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FTransform Transform(FVector(Location + UKismetMathLibrary::RandomUnitVector()*Radius));
			auto AddingMesh = Asteroid->GetWorld()->SpawnActor<ADynamicSDMCActor>(
				ADynamicSDMCActor::StaticClass(), Transform, Params);
			if (!AddingMesh)
			{
				continue;
			}
			//AddingMesh->SetActorTickEnabled(false);
			AddingMesh->MinimumRadius = Radius * UKismetMathLibrary::RandomFloat();
			AddingMesh->VariableRadius = Radius/2 * UKismetMathLibrary::RandomFloat();
			AddingMesh->PrimitiveType = EDynamicMeshActorPrimitiveType::Sphere;
			// Update mesh after calculation
			AddingMesh->RegenerateMesh();
			
			AddingMesh->TessellationLevel = 30;
			SpawnedAsteroids.Add(AddingMesh);
		}
		SpawnedAsteroids2DArray.Add(SpawnedAsteroids);
		ThreadsCompleted.Add(false);
		
		FTimerHandle CheckThreadCompletionTimerHandle;
		CheckThreadCompletionTimerHandles.Add(CheckThreadCompletionTimerHandle);
		World->GetTimerManager().SetTimer(CheckThreadCompletionTimerHandle,
			FTimerDelegate::CreateUObject(this, &ThisClass::CheckIsThreadCompleted, Index),  0.1, true);
		auto ThreadWorker = FAsteroidMeshWorker::AsteroidMeshWorker(Asteroid, SpawnedAsteroids2DArray[Index]);
		ThreadWorkers.Add(ThreadWorker);
		Index++;
	}

	// Main timer to watch other threads
	CheckAllThreadsAreCompletedTimerDelegate.BindUObject(this, &ThisClass::CheckAreAllThreadsCompleted);
	World->GetTimerManager().SetTimer(CheckAllThreadsAreCompletedTimerHandle, CheckAllThreadsAreCompletedTimerDelegate, 0.5, true);
}

void UAsteroidFieldMaker::CheckAreAllThreadsCompleted()
{
	uint32 Count = 0;
	for (const auto& IsThreadCompleted : ThreadsCompleted)
	{
		if (IsThreadCompleted == false)
		{
			return;
		}
		Count++;
	}
	if (Count == NumberOfThreads)
	{
		AreAllThreadsCompleted = true;
		OnAsteroidsModificationCompleted.ExecuteIfBound();
		FinishUp();
		ConditionalBeginDestroy();
		World->GetTimerManager().ClearTimer(CheckAllThreadsAreCompletedTimerHandle);
	}
}

void UAsteroidFieldMaker::CheckIsThreadCompleted(uint32 Index)
{
	if (ThreadWorkers[Index] && ThreadWorkers[Index]->IsFinished())
	{
		// thread completed his work
		//delete[] ThreadWorkers[Index];
		ThreadWorkers[Index] = nullptr;	
		ThreadsCompleted[Index] = true;
		World->GetTimerManager().ClearTimer(CheckThreadCompletionTimerHandles[Index]);
	}
}

void UAsteroidFieldMaker::FinishUp()
{
	for (uint32 i = 0; i < static_cast<uint32>(SpawnedAsteroids2DArray.Num()); i++)
	{
		for (uint32 j = 0; j < static_cast<uint32>(SpawnedAsteroids2DArray[i].Num()); j++)
		{
			if (SpawnedAsteroids2DArray[i][j])
			{
				// Update mesh after calculation
				// SpawnedAsteroids2DArray[i][j]->EditMesh([](FDynamicMesh3& Mesh3)
				// {
				// 	return;
				// });
				
				if(UKismetMathLibrary::RandomBool())
				{
					(*AsteroidsToEdit)[i]->SubtractMesh(SpawnedAsteroids2DArray[i][j]);
				}
				else
				{
					(*AsteroidsToEdit)[i]->UnionWithMesh(SpawnedAsteroids2DArray[i][j]);
				}
				//(*AsteroidsToEdit)[i]->SolidifyMesh((*AsteroidsToEdit)[i]->MeshComponent->GetMesh()->VertexCount() * 0.5);
				SpawnedAsteroids2DArray[i][j]->Destroy();
			}
		}
	}
}