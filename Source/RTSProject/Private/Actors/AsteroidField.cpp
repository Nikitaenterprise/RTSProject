#include "Actors/AsteroidField.h"

//#include "KismetProceduralMeshLibrary.h"
#include "Core/RTSPlayerController.h"
#include "Actors/AsteroidResource.h"
#include "Core/FactoryAssets.h"
#include "Actors/ResourceManager.h"
#include "Algo/Copy.h"
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

	//AsteroidFieldMaker = UAsteroidFieldMaker::AsteroidFieldMaker(GetWorld(), this, Asteroids, 10);
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

// UAsteroidFieldMaker::UAsteroidFieldMaker()
// {
// }
//
// UAsteroidFieldMaker* UAsteroidFieldMaker::AsteroidFieldMaker(UWorld* ThisWorld, const AAsteroidField* AsteroidField,
// 	TArray<AAsteroidResource*>& AsteroidsInField, uint32 MeshSpawningAmount)
// {
// 	const auto Maker = NewObject<UAsteroidFieldMaker>();
// 	Maker->World = ThisWorld;
// 	Maker->Field = AsteroidField;
// 	Maker->AsteroidsInFieldToEdit = &AsteroidsInField;
// 	Maker->NumberOfAsteroidsToSpawn = MeshSpawningAmount;
// 	if (Maker->IsReady())
// 	{
// 		Maker->Start();
// 	}
// 	return Maker;
// }
//
// void UAsteroidFieldMaker::Start()
// {
// 	NumberOfThreads = AsteroidsInFieldToEdit->Num();
// 	ThreadWorkers.Reserve(NumberOfThreads);
// 	SpawnedAsteroids2DArray.Reserve(NumberOfThreads);
// 	ThreadsCompleted.Reserve(NumberOfThreads);
// 	
// 	for (uint32 Index = 0; Index < static_cast<uint32>((*AsteroidsInFieldToEdit).Num()); Index++)
// 	{
// 		auto Asteroid = (*AsteroidsInFieldToEdit)[Index];
// 		const auto Location = Asteroid->GetActorLocation();
// 		const auto Vertices = Asteroid->GetMeshComponent()->ExtractMesh(false)->GetVerticesBuffer();
// 		const auto Triangles = Asteroid->GetMeshComponent()->ExtractMesh(false)->GetTrianglesBuffer();
// 		FVector VertexPosition = FVector(Vertices[0][0], Vertices[0][1], Vertices[0][2]);
// 		const float Radius = VertexPosition.Size();
// 		Materials = Asteroid->GetMeshComponent()->GetMaterials();
// 		
// 		TArray<ADynamicSDMCActor*> SpawnedAsteroids;
// 		SpawnedAsteroids.Reserve(NumberOfAsteroidsToSpawn);
// 		for (uint32 i = 0; i < NumberOfAsteroidsToSpawn; i++)
// 		{
// 			FActorSpawnParameters Params;
// 			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
// 			FTransform Transform(FVector(Location + UKismetMathLibrary::RandomUnitVector()*Radius));
// 			auto AddingMesh = Asteroid->GetWorld()->SpawnActor<ADynamicSDMCActor>(
// 				ADynamicSDMCActor::StaticClass(), Transform, Params);
// 			if (!AddingMesh)
// 			{
// 				continue;
// 			}
// 			AddingMesh->MinimumRadius = Radius * UKismetMathLibrary::RandomFloat();
// 			AddingMesh->VariableRadius = Radius/10 * UKismetMathLibrary::RandomFloat();
// 			AddingMesh->TessellationLevel = 30;
// 			AddingMesh->PrimitiveType = EDynamicMeshActorPrimitiveType::Sphere;
// 			AddingMesh->GetMeshComponent()->SetMaterial(0, Materials[0]);
// 			
// 			SpawnedAsteroids.Add(AddingMesh);
// 		}
// 		SpawnedAsteroids2DArray.Add(SpawnedAsteroids);
// 		ThreadsCompleted.Add(false);
// 		
// 		FTimerHandle CheckThreadCompletionTimerHandle;
// 		CheckThreadCompletionTimerHandles.Add(CheckThreadCompletionTimerHandle);
// 		World->GetTimerManager().SetTimer(CheckThreadCompletionTimerHandle,
// 			FTimerDelegate::CreateUObject(this, &ThisClass::CheckIsThreadCompleted, Index),  0.1, true);
// 		auto ThreadWorker = FAsteroidMeshWorker::AsteroidMeshWorker(Asteroid, SpawnedAsteroids2DArray[Index]);
// 		ThreadWorkers.Add(ThreadWorker);
// 	}
//
// 	// Main timer to watch other threads
// 	CheckAllThreadsAreCompletedTimerDelegate.BindUObject(this, &ThisClass::CheckAreAllThreadsCompleted);
// 	World->GetTimerManager().SetTimer(CheckAllThreadsAreCompletedTimerHandle, CheckAllThreadsAreCompletedTimerDelegate, 0.5, true);
// }
//
// void UAsteroidFieldMaker::CheckAreAllThreadsCompleted()
// {
// 	uint32 Count = 0;
// 	for (const auto& IsThreadCompleted : ThreadsCompleted)
// 	{
// 		if (IsThreadCompleted == false)
// 		{
// 			return;
// 		}
// 		Count++;
// 	}
// 	if (Count == NumberOfThreads)
// 	{
// 		AreAllThreadsCompleted = true;
// 		OnAsteroidsModificationCompleted.ExecuteIfBound();
// 		FinishUp();
// 		ConditionalBeginDestroy();
// 		World->GetTimerManager().ClearTimer(CheckAllThreadsAreCompletedTimerHandle);
// 	}
// }
//
// void UAsteroidFieldMaker::CheckIsThreadCompleted(uint32 Index)
// {
// 	if (ThreadWorkers[Index] && ThreadWorkers[Index]->IsFinished())
// 	{
// 		// thread completed his work
// 		//delete[] ThreadWorkers[Index];
// 		ThreadWorkers[Index] = nullptr;	
// 		ThreadsCompleted[Index] = true;
// 		World->GetTimerManager().ClearTimer(CheckThreadCompletionTimerHandles[Index]);
// 	}
// }
//
// void UAsteroidFieldMaker::FinishUp()
// {
// 	for (uint32 i = 0; i < static_cast<uint32>(SpawnedAsteroids2DArray.Num()); i++)
// 	{
// 		(*AsteroidsInFieldToEdit)[i]->GetMeshComponent()->SetMaterial(0, UMaterial::GetDefaultMaterial(MD_Surface));
// 		for (uint32 j = 0; j < static_cast<uint32>(SpawnedAsteroids2DArray[i].Num()); j++)
// 		{
// 			if (SpawnedAsteroids2DArray[i][j])
// 			{
// 				// Update mesh after calculation
// 				SpawnedAsteroids2DArray[i][j]->RegenerateMesh();
// 				SpawnedAsteroids2DArray[i][j]->GetMeshComponent()->SetMaterial(0, Materials[0]);
// 				// Add or subtract meshes to initial mesh
// 				if(UKismetMathLibrary::RandomBool())
// 				{
// 					(*AsteroidsInFieldToEdit)[i]->SubtractMesh(SpawnedAsteroids2DArray[i][j]);
// 				}
// 				else
// 				{
// 					(*AsteroidsInFieldToEdit)[i]->UnionWithMesh(SpawnedAsteroids2DArray[i][j]);
// 				}
// 				(*AsteroidsInFieldToEdit)[i]->SimplifyMeshToTriCount(5000);
//
// 				//CopyMesh((*AsteroidsInFieldToEdit)[i], (*AsteroidsInFieldToEdit)[i]);
// 				// auto StatickMesh = NewObject<UStaticMeshComponent>();
// 				// StatickMesh->GetStaticMesh()->SetMaterial(0, Materials[0]);
// 				// UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(StatickMesh, 0, );
// 				(*AsteroidsInFieldToEdit)[i]->GetMeshComponent()->SetMaterial(0, Materials[0]);
// 				SpawnedAsteroids2DArray[i][j]->Destroy();
// 			}
// 		}
// 	}
// }
//
// void UAsteroidFieldMaker::CopyMesh(const AAsteroidResource* AsteroidResourceFrom, AAsteroidResource* AsteroidResourceTo)
// {
// 	auto DynamicMesh3 = AsteroidResourceFrom->GetMeshRef();
//
// 	//// MESH DATA
// 	//int32 NumSections = DynamicMesh3->GetNumSections(LODIndex);
// 	//for (int32 SectionIndex = 0; SectionIndex < NumSections; SectionIndex++)
// 	{
// 		// Buffers for copying geom data
// 		auto Vertices = DynamicMesh3.GetVerticesBuffer();
// 		auto Triangles = DynamicMesh3.GetTrianglesBuffer();
// 		auto Normals = DynamicMesh3.GetNormalsBuffer();
// 		auto UVs = DynamicMesh3.GetUVBuffer();
// 		TArray<FVector2D> UVs1;
// 		TArray<FVector2D> UVs2;
// 		TArray<FVector2D> UVs3;
// 		//TArray<FProcMeshTangent> Tangents;
//
// 		// Get geom data from static mesh
// 		//GetSectionFromStaticMesh(DynamicMesh3, LODIndex, SectionIndex, Vertices, Triangles, Normals, UVs, Tangents);
//
// 		// Create section using data
// 		TArray<FLinearColor> DummyColors;
// 		//ProcMeshComponent->CreateMeshSection_LinearColor(SectionIndex, Vertices, Triangles, Normals, UVs, UVs1, UVs2, UVs3, DummyColors, Tangents, bCreateCollision);
// 	}
// 	AsteroidResourceTo->GetMeshComponent()->SetMaterial(0, Materials[0]);
// 	//// SIMPLE COLLISION
//
// 	// Clear any existing collision hulls
// 	// ProcMeshComponent->ClearCollisionConvexMeshes();
// 	//
// 	// if (DynamicMesh3->GetBodySetup() != nullptr)
// 	// {
// 	// 	// Iterate over all convex hulls on static mesh..
// 	// 	const int32 NumConvex = DynamicMesh3->GetBodySetup()->AggGeom.ConvexElems.Num();
// 	// 	for (int ConvexIndex = 0; ConvexIndex < NumConvex; ConvexIndex++)
// 	// 	{
// 	// 		// Copy convex verts to ProcMesh
// 	// 		FKConvexElem& MeshConvex = DynamicMesh3->GetBodySetup()->AggGeom.ConvexElems[ConvexIndex];
// 	// 		ProcMeshComponent->AddCollisionConvexMesh(MeshConvex.VertexData);
// 	// 	}
// 	// }
//
// 	//// MATERIALS
//
// 	// for (int32 MatIndex = 0; MatIndex < StaticMeshComponent->GetNumMaterials(); MatIndex++)
// 	// {
// 	// 	ProcMeshComponent->SetMaterial(MatIndex, StaticMeshComponent->GetMaterial(MatIndex));
// 	// }
// }
