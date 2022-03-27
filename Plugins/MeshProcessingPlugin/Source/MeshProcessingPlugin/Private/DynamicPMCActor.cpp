#include "DynamicPMCActor.h"
#include "MeshComponentRuntimeUtils.h"
#include "DynamicMesh3.h"
#include "Operations/MeshConvexHull.h"


ADynamicPMCActor::ADynamicPMCActor()
{
	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"), false);
	SetRootComponent(MeshComponent);
}

void ADynamicPMCActor::OnMeshEditedInternal()
{
	UpdatePMCMesh();
	Super::OnMeshEditedInternal();
}

void ADynamicPMCActor::UpdatePMCMesh()
{
	if (MeshComponent)
	{
		const bool bUseFaceNormals = (this->NormalsMode == EDynamicMeshActorNormalsMode::FaceNormals);
		constexpr bool bUseUV0 = true;
		constexpr bool bUseVertexColors = false;

		bool bGenerateSectionCollision = false;
		if (this->CollisionMode == EDynamicMeshActorCollisionMode::ComplexAsSimple
			|| this->CollisionMode == EDynamicMeshActorCollisionMode::ComplexAsSimpleAsync)
		{
			bGenerateSectionCollision = true;
			MeshComponent->bUseAsyncCooking = (this->CollisionMode == EDynamicMeshActorCollisionMode::ComplexAsSimpleAsync);
			MeshComponent->bUseComplexAsSimpleCollision = true;
		}

		MPPUtils::UpdatePMCFromDynamicMesh_SplitTriangles(MeshComponent,
			&SourceMesh, bUseFaceNormals, bUseUV0, bUseVertexColors, bGenerateSectionCollision);

		// update material on new section
		UMaterialInterface* UseMaterial = (this->Material != nullptr) ?
			this->Material : UMaterial::GetDefaultMaterial(MD_Surface);
		MeshComponent->SetMaterial(0, UseMaterial);

		// generate convex collision
		if (this->CollisionMode == EDynamicMeshActorCollisionMode::SimpleConvexHull)
		{
			FMeshConvexHull HullCompute(&SourceMesh);
			const int32 NumTris = FMath::Clamp(this->MaxHullTriangles, 0, 1000);
			if (NumTris != 0)
			{
				HullCompute.bPostSimplify = true;
				HullCompute.MaxTargetFaceCount = NumTris;
			}
			if (HullCompute.Compute())
			{
				TArray<FVector> Points;
				for (FVector3d Pos : HullCompute.ConvexHull.VerticesItr())
				{
					Points.Add(static_cast<FVector>(Pos));
				}
				MeshComponent->bUseComplexAsSimpleCollision = false;
				MeshComponent->ClearCollisionConvexMeshes();
				MeshComponent->AddCollisionConvexMesh(Points);
			}
		}
	}
}