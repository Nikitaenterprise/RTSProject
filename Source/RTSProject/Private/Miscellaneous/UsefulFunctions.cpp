#include "Miscellaneous/UsefulFunctions.h"

#include "Math/UnitConversion.h"


bool UsefulFunctions::GetVertices(const UStaticMeshComponent* StaticMeshComponent, const FTransform& MeshTransform, TArray<FVector>& Vertices)
{
	if (!StaticMeshComponent) return false;
	if (!StaticMeshComponent->GetStaticMesh()) return false;
	if (!StaticMeshComponent->GetStaticMesh()->GetRenderData()) return false;
	if (StaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources.Num() > 0)
	{
		FPositionVertexBuffer* VertexBuffer = &StaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[0].VertexBuffers.PositionVertexBuffer;

		if (VertexBuffer)
		{
			const uint32 VertexCount = VertexBuffer->GetNumVertices();
			Vertices.Empty(VertexCount);
			for (uint32 i = 0; i < VertexCount; i++)
			{
				//Location and transform of Static Mesh
				const auto& VertexPosition = VertexBuffer->VertexPosition(i);
				const FVector WorldSpaceVertexLocation = MeshTransform.GetLocation() + MeshTransform.TransformVector({VertexPosition.X, VertexPosition.Y, VertexPosition.Z});
				Vertices.Add(WorldSpaceVertexLocation);
			}
		}
	}
	return true;
}

TArray<FVector> UsefulFunctions::GetSortedVectorsByDistanceFromPoint(const TArray<FVector>& Vectors, const FVector& Point)
{
	TArray<TPair<uint32, float>> DistancesSquared;
	DistancesSquared.Empty(Vectors.Num());
	TArray<FVector> Output;
	Output.Empty(Vectors.Num());
	// Fill DistancesSquared with all distances from Socket to every vertices
	for (uint32 i = 0; i < static_cast<uint32>(Vectors.Num()); i++)
	{
		DistancesSquared.Add(TPair<uint32, float>(i, FVector::Dist(Point, Vectors[i])));
	}
	// Sort by increasing distance
	DistancesSquared.Sort([](const TPair<uint32, float>& LHS, const TPair<uint32, float>& RHS) {return LHS.Value < RHS.Value; });

	for (uint32 i = 0; i < static_cast<uint32>(Vectors.Num()); i++)
	{
		Output.Add(Vectors[DistancesSquared[i].Key]);
	}
	return Output;
}

float UsefulFunctions::GetScaleValueFromSettings()
{
	float DistanceUnitScale = 0;
	FString ValueReceived;
	if (GConfig->GetString(
		TEXT("/Script/UnrealEd.EditorProjectAppearanceSettings"),
		TEXT("DistanceUnits"),
		ValueReceived,
		GEditorIni))
	{
		TOptional<EUnit> CurrentUnit = FUnitConversion::UnitFromString(*ValueReceived);
		if (!CurrentUnit.IsSet())
			CurrentUnit = EUnit::Centimeters;

		switch (CurrentUnit.GetValue())
		{
		case EUnit::Micrometers:
			DistanceUnitScale = 1000000.0;
			break;
		case EUnit::Millimeters:
			DistanceUnitScale = 1000.0;
			break;
		case EUnit::Centimeters:
			DistanceUnitScale = 100.0;
			break;
		case EUnit::Meters:
			DistanceUnitScale = 1.0;
			break;
		case EUnit::Kilometers:
			DistanceUnitScale = 0.001;
			break;
		default:
			DistanceUnitScale = 100.0;
		}
	}
	return DistanceUnitScale;
}
