#include "UsefulFunctions.h"


bool UsefulFunctions::GetVertices(const UStaticMeshComponent* StaticMeshComponent, const FTransform& MeshTransform, TArray<FVector>& Vertices)
{
	if (!StaticMeshComponent) return false;
	if (!StaticMeshComponent->GetStaticMesh()) return false;
	if (!StaticMeshComponent->GetStaticMesh()->RenderData) return false;
	if (StaticMeshComponent->GetStaticMesh()->RenderData->LODResources.Num() > 0)
	{
		FPositionVertexBuffer* VertexBuffer = &StaticMeshComponent->GetStaticMesh()->RenderData->LODResources[0].VertexBuffers.PositionVertexBuffer;

		if (VertexBuffer)
		{
			const uint32 VertexCount = VertexBuffer->GetNumVertices();
			Vertices.Empty(VertexCount);
			for (uint32 i = 0; i < VertexCount; i++)
			{
				//Location and transform of Static Mesh
				const FVector WorldSpaceVertexLocation = MeshTransform.GetLocation() + MeshTransform.TransformVector(VertexBuffer->VertexPosition(i));
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
