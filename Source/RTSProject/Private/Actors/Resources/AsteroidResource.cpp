#include "Actors/Resources/AsteroidResource.h"

#include "Actors/Resources/AsteroidField.h"
#include "GAS/ResourceSourceAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "GeneratedMeshDeformersLibrary.h"

AAsteroidResource::AAsteroidResource()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ResourceComponent = CreateDefaultSubobject<UResourceComponent>(TEXT("ResourceComponent"));
	PrimaryActorTick.bCanEverTick = true;
	ResourceComponent->SetResourceType(EResourceType::Asteroid);

	PrimitiveType = EDynamicMeshActorPrimitiveType::Sphere;
	TessellationLevel = 50;
	MinimumRadius = 100;
	VariableRadius = 20;
	CollisionMode = EDynamicMeshActorCollisionMode::ComplexAsSimple;
}

void AAsteroidResource::PostLoad()
{
	Super::PostLoad();
	
	// EditMesh([&](FDynamicMesh3& MeshToUpdate) {
	// 	if (SourceType == EDynamicMeshActorSourceType::Primitive)
	// 	{
	// 		FSphereGenerator SphereGen;
	// 		SphereGen.NumPhi = SphereGen.NumTheta = FMath::Clamp(this->TessellationLevel, 3, 50);
	// 		SphereGen.Radius = (this->MinimumRadius + this->VariableRadius)
	// 		+ (this->VariableRadius) * FMathd::Sin(PulseSpeed * AccumulatedTime);
	// 		//MeshToUpdate.Copy(&SphereGen.Generate());
	// 		
	// 		
	// 	}
	//});
	
}

void AAsteroidResource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Rotate asteroid
	GetSDMComponent()->AddLocalRotation(RandomRotator * DeltaTime);
}

void AAsteroidResource::BeginPlay()
{
	Super::BeginPlay();

	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [This = TWeakObjectPtr<ThisClass>(this)]()
	{
		if (This.IsValid())
		{
			//TODO: there is a crash when deforming asteroid
			//This->Deform();
		}
	});
	
	if (!bManualSetUpRotation)
	{
		RotationSpeed = UKismetMathLibrary::RandomFloat();
	}
	RotationSpeed *= UKismetMathLibrary::RandomBool() ? 1 : -1;
	RandomRotator = UKismetMathLibrary::RandomRotator(false) * RotationSpeed;

	// const auto AttributeSet = AbilitySystemComponent->GetSet<UResourceSourceAttributeSet>();
	// if (AttributeSet)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("No UResourceSourceAttributeSet in AAsteroidResource::BeginPlay()"));
	// 	return;
	// }
	ResourceComponent->InitializeCapacity([&]()->float
	{
		const int SquaredSizeOfMesh = UKismetMathLibrary::RandomFloatInRange(1000, 5000);//UKismetMathLibrary::VSizeSquared(GetActorScale3D());
		if (UKismetMathLibrary::Round(SquaredSizeOfMesh) != 0)
		{
			return UKismetMathLibrary::FTrunc(SquaredSizeOfMesh * InitialCapacityModifier);	
		}
		return 0;
	});
	// Bind resource capacity check
	ResourceComponent->GetResourceCapacityDelegate().AddUObject(this, &ThisClass::CheckCapacity);
	
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

void AAsteroidResource::Deform()
{
	if (FModuleManager::Get().IsModuleLoaded("MeshProcessingPlugin"))
	{
		UGeneratedMesh* AllocatedMesh = AllocateComputeMesh();
		AllocatedMesh->AppendSphere(MinimumRadius + VariableRadius * FMathd::Sin(PulseSpeed * AccumulatedTime), 500, 500);
		UGeneratedMeshDeformersLibrary::DeformMeshAxisSinWaveRadial(AllocatedMesh, 6, 0.1, 0.1, FVector(0,1,0));
		UGeneratedMeshDeformersLibrary::DeformMeshAxisSinWave1D(AllocatedMesh, 12, 0.05, 0.1, FVector(0,1,1), FVector(1,1,0));
		CopyFromMesh(AllocatedMesh);
		if (!bManualSetUpNumberOfCavities)
		{
			NumberOfCavities = UKismetMathLibrary::RandomInteger(10);
		}
		for (int32 Index = 0; Index < NumberOfCavities; Index++)
		{
			UGeneratedMesh* SubtractAllocatedMesh = AllocateComputeMesh();
			SubtractAllocatedMesh->AppendSphere(
				MinimumRadius/20 + VariableRadius * FMathd::Sin(UKismetMathLibrary::RandomInteger(360)), 100, 100);
			if (bShouldDistortCavityWithSinWave)
			{
				UGeneratedMeshDeformersLibrary::DeformMeshAxisSinWaveRadial(
					SubtractAllocatedMesh, 6, 0.1, 0.1, FVector(0,1,0));
				UGeneratedMeshDeformersLibrary::DeformMeshAxisSinWave1D(
					SubtractAllocatedMesh, 12, 0.05, 0.1, FVector(0,1,1), FVector(1,1,0));
			}
			auto NearestPoint = AllocatedMesh->NearestPoint(UKismetMathLibrary::RandomUnitVector() * MinimumRadius);
			const auto Scale = FVector(
				UKismetMathLibrary::RandomFloatInRange(0.5, 2),
				UKismetMathLibrary::RandomFloatInRange(0.5, 2),
				UKismetMathLibrary::RandomFloatInRange(0.5, 2));
			const FTransform Transform (FRotator(0, 0, 0), NearestPoint, Scale);
			const auto GeneratedMesh = AllocatedMesh->BooleanWithTransformed(
				SubtractAllocatedMesh, Transform, EGeneratedMeshBooleanOperation::Subtraction);
			CopyFromMesh(GeneratedMesh);
			AllocatedMesh->SolidifyMesh();
		}
	}
}