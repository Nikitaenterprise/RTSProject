#include "Actors/AsteroidResource.h"

#include <functional>
#include "Actors/AsteroidField.h"
#include "GAS/ResourceSourceAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "GeneratedMeshDeformersLibrary.h"
#include "Generators/SphereGenerator.h"

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
	GetMeshComponent()->AddLocalRotation(RandomRotator * DeltaTime);
}

void AAsteroidResource::BeginPlay()
{
	Super::BeginPlay();
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
	
	if (!bManualSetUpRotation)
	{
		RotationSpeed = UKismetMathLibrary::RandomFloat();
	}
	RotationSpeed *= UKismetMathLibrary::RandomBool() ? 1 : -1;
	RandomRotator = UKismetMathLibrary::RandomRotator(false) * RotationSpeed;

	const auto AttributeSet = GetAbilitySystemComponent()->GetSet<UResourceSourceAttributeSet>();
	if (AttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("No UResourceSourceAttributeSet in AAsteroidResource::BeginPlay()"));
		return;
	}
	ResourceComponent->InitializeCapacity([&]()->float
	{
		const int SquaredSizeOfMesh = UKismetMathLibrary::VSizeSquared(GetActorScale3D());
		if (UKismetMathLibrary::Round(SquaredSizeOfMesh) != 0)
		{
			return UKismetMathLibrary::FTrunc(SquaredSizeOfMesh * InitialCapacityModifier);	
		}
		return 0;
	});
	// Bind resource capacity check
	const auto ResourceCapacityDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AttributeSet->GetResourceCapacityAttribute()).AddUObject(this, &ThisClass::CheckCapacity);
	ResourceComponent->SetResourceCapacityDelegateHandle(ResourceCapacityDelegateHandle);
	
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

void AAsteroidResource::ProcessMesh()
{
	if (FModuleManager::Get().IsModuleLoaded("MeshProcessingPlugin"))
	{
		const auto Location = GetActorLocation();
		const auto Size = UKismetMathLibrary::VSizeSquared(GetActorScale3D());
		const auto Vertices = MeshComponent->ExtractMesh(false)->GetVerticesBuffer();
		const auto Triangles = MeshComponent->ExtractMesh(false)->GetTrianglesBuffer();
		FVector VertexPosition = FVector(Vertices[0][0], Vertices[0][1], Vertices[0][2]);
		const float Radius = VertexPosition.Size();

		for (int32 i = 0; i<1; i++)
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FTransform Transform(FVector(Location + UKismetMathLibrary::RandomUnitVector()*Radius));
			auto AddingMesh = GetWorld()->SpawnActor<ADynamicSDMCActor>(ADynamicSDMCActor::StaticClass(), Transform, Params);
			AddingMesh->MinimumRadius = Radius * UKismetMathLibrary::RandomFloat();
			AddingMesh->VariableRadius = Radius/2 * Radius * UKismetMathLibrary::RandomFloat();
			AddingMesh->PrimitiveType = EDynamicMeshActorPrimitiveType::Sphere;
			AddingMesh->TessellationLevel = 30;
			TFunctionRef<void(FDynamicMesh3&)> EditFunc = [&](FDynamicMesh3& Mesh3)
			{
				double UseRadius = MinimumRadius + VariableRadius * FMathd::Sin(UKismetMathLibrary::RandomFloat());
				// generate new mesh
				FSphereGenerator SphereGen;
				SphereGen.NumPhi = SphereGen.NumTheta = FMath::Clamp(this->TessellationLevel, 3, 50);
				SphereGen.Radius = UseRadius;
				Mesh3.Copy(&SphereGen.Generate());
			};
			AddingMesh->EditMesh(EditFunc);
				
			// if(UKismetMathLibrary::RandomBool())
			// {
			// 	UnionWithMesh(AddingMesh);
			// }
			// else
			// {
			// 	SubtractMesh(AddingMesh);
			// }
			SubtractMesh(AddingMesh);
			AddingMesh->Destroy();
			
		}
		//SimplifyMeshToTriCount(Triangles.Num()*0.8);
	}
}
