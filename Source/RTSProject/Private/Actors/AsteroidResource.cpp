#include "Actors/AsteroidResource.h"

#include <functional>
#include "Actors/AsteroidField.h"
#include "GAS/ResourceSourceAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "MeshProcessingPlugin/Public/MeshProcessingPlugin.h"
#include "Generators/SphereGenerator.h"

AAsteroidResource::AAsteroidResource()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ResourceComponent = CreateDefaultSubobject<UResourceComponent>(TEXT("ResourceComponent"));
	PrimaryActorTick.bCanEverTick = true;
	ResourceComponent->SetResourceType(EResourceType::Asteroid);

	PrimitiveType = EDynamicMeshActorPrimitiveType::Sphere;
	TessellationLevel = 500;
	MinimumRadius = 50;
	CollisionMode = EDynamicMeshActorCollisionMode::ComplexAsSimple;
}

void AAsteroidResource::PostLoad()
{
	Super::PostLoad();
	EditMesh([&](FDynamicMesh3& MeshToUpdate) {
		if (SourceType == EDynamicMeshActorSourceType::Primitive)
		{
			double UseRadius = (this->MinimumRadius + this->VariableRadius)
				+ (this->VariableRadius) * FMathd::Sin(PulseSpeed * AccumulatedTime);
	
			// generate new mesh
			if (this->PrimitiveType == EDynamicMeshActorPrimitiveType::Sphere)
			{
				FSphereGenerator SphereGen;
				SphereGen.NumPhi = SphereGen.NumTheta = FMath::Clamp(this->TessellationLevel, 3, 50);
				SphereGen.Radius = UseRadius;
				MeshToUpdate.Copy(&SphereGen.Generate());
			}
		}
	});
}

void AAsteroidResource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Rotate asteroid
	const FRotator Rotation = MeshComponent->GetComponentRotation();
	SetActorRotation(FRotator(Rotation.Pitch, Rotation.Yaw + (RotationSpeed * DeltaTime), Rotation.Roll), ETeleportType::None);
}

void AAsteroidResource::BeginPlay()
{
	Super::BeginPlay();
	
	//ProcessMesh();

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
