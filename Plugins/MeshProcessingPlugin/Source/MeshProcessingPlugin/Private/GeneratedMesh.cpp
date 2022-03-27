#include "GeneratedMesh.h"
#include "DynamicMeshBaseActor.h"

#include "MeshTransforms.h"
#include "MeshNormals.h"
#include "MeshQueries.h"

#include "Generators/SphereGenerator.h"
#include "Generators/GridBoxMeshGenerator.h"
#include "Generators/BoxSphereGenerator.h"
#include "Generators/SweepGenerator.h"

#include "DynamicMeshEditor.h"
#include "MeshSimplification.h"
#include "MeshConstraintsUtil.h"
#include "Operations/MeshBoolean.h"
#include "Implicit/Solidify.h"
#include "Operations/MeshPlaneCut.h"
#include "Operations/MeshMirror.h"
#include "ConstrainedDelaunay2.h"


UGeneratedMesh::UGeneratedMesh()
{
	Mesh = MakeUnique<FDynamicMesh3>();
	ResetMesh();
	ClearAppendTransform();
}

UGeneratedMesh* UGeneratedMesh::ResetMesh()
{
	Mesh->Clear();
	Mesh->EnableTriangleGroups();
	Mesh->EnableAttributes();
	ClearAppendTransform();
	OnMeshUpdated();
	return this;
}

void UGeneratedMesh::OnMeshUpdated()
{
	MeshAABBTree = nullptr;
	FastWinding = nullptr;
}

TUniquePtr<FDynamicMeshAABBTree3>& UGeneratedMesh::GetAABBTree()
{
	if (!MeshAABBTree)
	{
		MeshAABBTree = MakeUnique<FDynamicMeshAABBTree3>(Mesh.Get(), true);
	}
	return MeshAABBTree;
}

const TUniquePtr<TFastWindingTree<FDynamicMesh3>>& UGeneratedMesh::GetFastWindingTree()
{
	if (!FastWinding)
	{
		const TUniquePtr<FDynamicMeshAABBTree3>& AABBTree = GetAABBTree();

		FastWinding = MakeUnique<TFastWindingTree<FDynamicMesh3>>(AABBTree.Get(), true);
	}
	return FastWinding;
}

UGeneratedMesh* UGeneratedMesh::InitializeFrom(ADynamicMeshBaseActor* MeshActor)
{
	*Mesh = MeshActor->GetMeshRef();
	ClearAppendTransform();
	OnMeshUpdated();
	return this;
}

UGeneratedMesh* UGeneratedMesh::MakeDuplicate(UGeneratedMesh* MeshObj)
{
	*Mesh = *(MeshObj->Mesh);
	OnMeshUpdated();
	return this;
}

UGeneratedMesh* UGeneratedMesh::SetAppendTransform(FTransform TransformIn)
{
	AppendTransform = FTransform3d(TransformIn);
	return this;
}

UGeneratedMesh* UGeneratedMesh::ClearAppendTransform()
{
	AppendTransform = FTransform3d::Identity();
	return this;
}

void UGeneratedMesh::AppendMeshWithAppendTransform(FDynamicMesh3&& ToAppend, bool bPostMeshUpdate)
{
	MeshTransforms::ApplyTransform(ToAppend, AppendTransform);

	FMeshIndexMappings Mappings;
	FDynamicMeshEditor Editor(Mesh.Get());
	Editor.AppendMesh(&ToAppend, Mappings);

	if (bPostMeshUpdate)
	{
		OnMeshUpdated();
	}
}

UGeneratedMesh* UGeneratedMesh::AppendAxisBox(FVector Min, FVector Max, int32 StepsX, int32 StepsY, int32 StepsZ)
{
	const FAxisAlignedBox3d AxisBox(static_cast<FVector3d>(Min), static_cast<FVector3d>(Max));
	FGridBoxMeshGenerator BoxGen;
	BoxGen.Box = FOrientedBox3d(AxisBox);
	BoxGen.EdgeVertices = { FMath::Max(0,StepsX), FMath::Max(0,StepsY), FMath::Max(0,StepsZ) };
	AppendMeshWithAppendTransform(FDynamicMesh3(&BoxGen.Generate()), true);
	return this;
}

UGeneratedMesh* UGeneratedMesh::AppendBox(FBox Box, int32 StepsX, int32 StepsY, int32 StepsZ)
{
	return AppendAxisBox(Box.Min, Box.Max, StepsX, StepsY, StepsZ);
}



UGeneratedMesh* UGeneratedMesh::AppendSphere(float Radius, int32 Slices, int32 Stacks)
{
	FSphereGenerator SphereGen;
	SphereGen.NumPhi = Stacks;
	SphereGen.NumTheta = Slices;
	SphereGen.Radius = Radius;
	AppendMeshWithAppendTransform( FDynamicMesh3(&SphereGen.Generate()), true);
	return this;
}


UGeneratedMesh* UGeneratedMesh::AppendSphereBox(float Radius, int32 Steps)
{
	FBoxSphereGenerator SphereGen;
	SphereGen.Box = FOrientedBox3d();
	SphereGen.Radius = Radius;
	SphereGen.EdgeVertices = { FMath::Max(0,Steps), FMath::Max(0,Steps), FMath::Max(0,Steps) };
	AppendMeshWithAppendTransform(FDynamicMesh3(&SphereGen.Generate()), true);
	return this;
}


UGeneratedMesh* UGeneratedMesh::AppendCylinder(float Radius, float Height, int32 Slices, int32 Stacks, bool bCapped)
{
	FCylinderGenerator CylGen;
	CylGen.Radius[0] = Radius;
	CylGen.Radius[1] = Radius;
	CylGen.Height = Height;
	CylGen.LengthSamples = Stacks;
	CylGen.AngleSamples = Slices;
	CylGen.bCapped = bCapped;
	AppendMeshWithAppendTransform(FDynamicMesh3(&CylGen.Generate()), true);
	return this;
}


UGeneratedMesh* UGeneratedMesh::AppendCone(float BaseRadius, float TopRadius, float Height, int32 Slices, int32 Stacks, bool bCapped)
{
	FCylinderGenerator CylGen;
	CylGen.Radius[0] = BaseRadius;
	CylGen.Radius[1] = TopRadius;
	CylGen.Height = Height;
	CylGen.LengthSamples = Stacks;
	CylGen.AngleSamples = Slices;
	CylGen.bCapped = bCapped;
	AppendMeshWithAppendTransform(FDynamicMesh3(&CylGen.Generate()), true);
	return this;
}



UGeneratedMesh* UGeneratedMesh::AppendTorus(float Radius, float SectionRadius, int32 CircleSlices, int32 SectionSlices)
{
	const FPolygon2d Circle = FPolygon2d::MakeCircle(SectionRadius, SectionSlices);
	TArray<FVector2D> Polygon;
	for (FVector2d v : Circle.GetVertices())
	{
		Polygon.Add(static_cast<FVector2D>(v));
	}
	return AppendRevolvePolygon(Polygon, Radius, CircleSlices);
}


UGeneratedMesh* UGeneratedMesh::AppendRevolvePolygon(TArray<FVector2D> Polygon, float Radius, int RevolveSteps)
{
	if (Polygon.Num() < 3) return this;
	FGeneralizedCylinderGenerator RevolveGen;
	for (FVector2D v : Polygon)
	{
		RevolveGen.CrossSection.AppendVertex(FVector2d(v));
	}
	const FPolygon2d PathPoly = FPolygon2d::MakeCircle(Radius, RevolveSteps);
	for (FVector2d v : PathPoly.GetVertices())
	{
		RevolveGen.Path.Add(v);
	}
	RevolveGen.bLoop = true;
	RevolveGen.bCapped = false;
	RevolveGen.bPolygroupPerQuad = true;
	RevolveGen.InitialFrame = FFrame3d(RevolveGen.Path[0]);
	AppendMeshWithAppendTransform(FDynamicMesh3(&RevolveGen.Generate()), true);
	return this;
}


UGeneratedMesh* UGeneratedMesh::AppendExtrusion(TArray<FVector2D> Polygon, float Height, bool bCapped)
{
	if (Polygon.Num() < 3) return this;
	FGeneralizedCylinderGenerator ExtrudeGen;
	for (const FVector2D& V : Polygon)
	{
		ExtrudeGen.CrossSection.AppendVertex((FVector2d)V);
	}
	ExtrudeGen.Path.Add(FVector3d::Zero());
	ExtrudeGen.Path.Add(FVector3d(0, 0, Height));
	ExtrudeGen.InitialFrame = FFrame3d();
	ExtrudeGen.bCapped = bCapped;
	ExtrudeGen.bPolygroupPerQuad = true;
	AppendMeshWithAppendTransform(FDynamicMesh3(&ExtrudeGen.Generate()), true);
	return this;
}


UGeneratedMesh* UGeneratedMesh::AppendTiled(UGeneratedMesh* OtherMeshObj, FTransform TransformIn, int RepeatCount, bool bApplyBefore)
{
	if (!OtherMeshObj) return this;
	// make a copy of other mesh so we can apply repeat transforms. This is necessary only for nonuniform scaling.
	FDynamicMesh3 AppendMesh = *OtherMeshObj->GetMesh();
	const FTransform3d Transform3d(TransformIn);

	FMeshIndexMappings Mappings;
	for (int32 k = 0; k < RepeatCount; ++k)
	{
		if (bApplyBefore)
		{
			MeshTransforms::ApplyTransform(AppendMesh, Transform3d);
		}

		Mappings.Reset();
		FDynamicMeshEditor Editor(Mesh.Get());
		Editor.AppendMesh(&AppendMesh, Mappings);

		if (!bApplyBefore)
		{
			MeshTransforms::ApplyTransform(AppendMesh, Transform3d);
		}
	}

	OnMeshUpdated();
	return this;
}




UGeneratedMesh* UGeneratedMesh::BooleanWith(UGeneratedMesh* OtherMesh, EGeneratedMeshBooleanOperation Operation)
{
	if (!OtherMesh) return this;

	FDynamicMesh3 ResultMesh;
	const FMeshBoolean::EBooleanOp ApplyOp = static_cast<FMeshBoolean::EBooleanOp>(static_cast<int>(Operation));
	FMeshBoolean Boolean( Mesh.Get(), FTransform3d::Identity(),
						  OtherMesh->Mesh.Get(), FTransform3d::Identity(),
						  &ResultMesh, ApplyOp);
	Boolean.bPutResultInInputSpace = true;
	const bool bOK = Boolean.Compute();
	if (!bOK)
	{
		// fill holes
	}
	*Mesh = MoveTemp(ResultMesh);

	OnMeshUpdated();
	return this;
}




UGeneratedMesh* UGeneratedMesh::BooleanWithTransformed(UGeneratedMesh* OtherMesh, FTransform TransformIn, EGeneratedMeshBooleanOperation Operation)
{
	if (!OtherMesh) return this;

	FDynamicMesh3 ResultMesh;

	FMeshBoolean::EBooleanOp ApplyOp = static_cast<FMeshBoolean::EBooleanOp>(static_cast<int>(Operation));
	FMeshBoolean Boolean(Mesh.Get(), FTransform3d::Identity(),
		OtherMesh->Mesh.Get(), FTransform3d(TransformIn),
		&ResultMesh, ApplyOp);
	Boolean.bPutResultInInputSpace = true;
	bool bOK = Boolean.Compute();
	if (!bOK)
	{
		// fill holes
	}
	*Mesh = MoveTemp(ResultMesh);
	OnMeshUpdated();
	return this;
}



UGeneratedMesh* UGeneratedMesh::CutWithPlane(FVector Origin, FVector Normal, bool bFillHole, bool bFlipSide)
{
	if (bFlipSide)
	{
		Normal = -Normal;
	}

	FMeshPlaneCut Cut(Mesh.Get(), FVector3d(Origin), FVector3d(Normal).Normalized());
	//Cut.UVScaleFactor = UVScaleFactor;
	Cut.Cut();

	const bool bFillSpans = true;
	if (bFillHole)
	{
		Cut.HoleFill(ConstrainedDelaunayTriangulate<double>, bFillSpans);
	}

	OnMeshUpdated();
	return this;
}



UGeneratedMesh* UGeneratedMesh::Mirror(FVector Origin, FVector Normal, bool bApplyPlaneCut)
{
	const FVector3d PlaneOrigin(Origin);
	FVector3d PlaneNormal(Normal);
	PlaneNormal.Normalize();

	constexpr double PlaneTolerance = FMathf::ZeroTolerance * 10.0;

	if (bApplyPlaneCut)
	{
		FMeshPlaneCut Cutter(Mesh.Get(), PlaneOrigin, PlaneNormal);
		Cutter.PlaneTolerance = PlaneTolerance;
		Cutter.Cut();
	}

	FMeshMirror Mirrorer(Mesh.Get(), PlaneOrigin, PlaneNormal);
	Mirrorer.bWeldAlongPlane = true;
	Mirrorer.bAllowBowtieVertexCreation = false;
	Mirrorer.PlaneTolerance = PlaneTolerance;

	Mirrorer.MirrorAndAppend(nullptr);

	OnMeshUpdated();
	return this;
}



UGeneratedMesh* UGeneratedMesh::SolidifyMesh(int VoxelResolution, float WindingThreshold)
{
	const FDynamicMesh3& SolidifyMesh = *Mesh;
	FDynamicMeshAABBTree3& SolidifyAABBTree = *GetAABBTree();
	TFastWindingTree<FDynamicMesh3>& SolidifyFastWinding = *GetFastWindingTree();

	constexpr double ExtendBounds = 2.0;
	TImplicitSolidify<FDynamicMesh3> SolidifyCalc(&SolidifyMesh, &SolidifyAABBTree, &SolidifyFastWinding);
	SolidifyCalc.SetCellSizeAndExtendBounds(SolidifyAABBTree.GetBoundingBox(), ExtendBounds, VoxelResolution);
	SolidifyCalc.WindingThreshold = WindingThreshold;
	SolidifyCalc.SurfaceSearchSteps = 5;
	SolidifyCalc.bSolidAtBoundaries = true;
	SolidifyCalc.ExtendBounds = ExtendBounds;
	FDynamicMesh3 SolidMesh(&SolidifyCalc.Generate());

	SolidMesh.EnableAttributes();
	FMeshNormals::InitializeOverlayToPerVertexNormals(SolidMesh.Attributes()->PrimaryNormals(), false);

	*Mesh = MoveTemp(SolidMesh);
	OnMeshUpdated();
	return this;
}


UGeneratedMesh* UGeneratedMesh::SimplifyMeshToTriCount(int32 TargetTriangleCount, bool bDiscardAttributes)
{
	TargetTriangleCount = FMath::Max(1, TargetTriangleCount);

	if (bDiscardAttributes)
	{
		Mesh->DiscardAttributes();
	}
	Mesh->EnableTriangleGroups();		// workaround?

	if (TargetTriangleCount < Mesh->TriangleCount())
	{
		FAttrMeshSimplification Reducer(Mesh.Get());

		if (!bDiscardAttributes)
		{
			// eliminate any bowties that might have formed on UV seams.
			Reducer.SetEdgeFlipTolerance(1.e-5);
			if (FDynamicMeshAttributeSet* Attributes = Mesh->Attributes())
			{
				for (int i = 0; i < Attributes->NumUVLayers(); ++i)
				{
					Attributes->GetUVLayer(i)->SplitBowties();
				}
				Attributes->PrimaryNormals()->SplitBowties();
			}

			constexpr bool bAllowSeamSplits = true;
			constexpr bool bAllowSeamSmoothing = true;
			constexpr bool bAllowSeamCollapse = true;
			FMeshConstraints constraints;
			FMeshConstraintsUtil::ConstrainAllBoundariesAndSeams(constraints, *Mesh,
				EEdgeRefineFlags::NoConstraint, EEdgeRefineFlags::NoConstraint, EEdgeRefineFlags::NoConstraint,
				bAllowSeamSplits, bAllowSeamSmoothing, bAllowSeamCollapse);
			Reducer.SetExternalConstraints(MoveTemp(constraints));
		}

		Reducer.SimplifyToTriangleCount(TargetTriangleCount);
		Mesh->CompactInPlace();
	}


	if (bDiscardAttributes)
	{
		Mesh->EnableAttributes();
		FMeshNormals::InitializeOverlayToPerVertexNormals(Mesh->Attributes()->PrimaryNormals(), false);
	}

	OnMeshUpdated();
	return this;
}





float UGeneratedMesh::DistanceToPoint(FVector Point, FVector& NearestPoint, int& NearestTriangle, FVector& TriBaryCoords)
{
	NearestPoint = Point;
	NearestTriangle = -1;

	double NearDistSqr;
	NearestTriangle = GetAABBTree()->FindNearestTriangle(Point, NearDistSqr);
	if (NearestTriangle < 0)
	{
		return TNumericLimits<float>::Max();
	}

	const FDistPoint3Triangle3d DistQuery = TMeshQueries<FDynamicMesh3>::TriangleDistance(*GetMesh(), NearestTriangle, Point);
	NearestPoint = static_cast<FVector>(DistQuery.ClosestTrianglePoint);
	TriBaryCoords = static_cast<FVector>(DistQuery.TriangleBaryCoords);
	return static_cast<float>(FMathd::Sqrt(NearDistSqr));
}


FVector UGeneratedMesh::NearestPoint(FVector Point)
{
	return static_cast<FVector>(GetAABBTree()->FindNearestPoint(Point));
}

bool UGeneratedMesh::ContainsPoint(FVector Point, float WindingThreshold)
{
	return GetFastWindingTree()->IsInside(Point, WindingThreshold);
}


bool UGeneratedMesh::IntersectRay(FVector RayOrigin, FVector RayDirection,
	FVector& HitPoint, float& HitDistance, int& NearestTriangle, FVector& TriBaryCoords,
	float MaxDistance)
{
	const FRay3d LocalRay(FVector3d(RayOrigin), FVector3d(RayDirection).Normalized());
	IMeshSpatial::FQueryOptions QueryOptions;
	if (MaxDistance > 0)
	{
		QueryOptions.MaxDistance = MaxDistance;
	}
	NearestTriangle = GetAABBTree()->FindNearestHitTriangle(LocalRay, QueryOptions);
	if (GetMesh()->IsTriangle(NearestTriangle))
	{
		const FIntrRay3Triangle3d IntrQuery = TMeshQueries<FDynamicMesh3>::TriangleIntersection(*GetMesh(), NearestTriangle, LocalRay);
		if (IntrQuery.IntersectionType == EIntersectionType::Point)
		{
			HitDistance = IntrQuery.RayParameter;
			HitPoint = static_cast<FVector>(LocalRay.PointAt(IntrQuery.RayParameter));
			TriBaryCoords = static_cast<FVector>(IntrQuery.TriangleBaryCoords);
			return true;
		}
	}
	return false;
}




UGeneratedMesh* UGeneratedMesh::SetToFaceNormals()
{
	FMeshNormals::InitializeMeshToPerTriangleNormals(Mesh.Get());
	// OnMeshUpdated();		// skip for now as we're just doing normals
	return this;
}

UGeneratedMesh* UGeneratedMesh::SetToVertexNormals()
{
	Mesh->EnableAttributes();
	FMeshNormals::InitializeOverlayToPerVertexNormals(Mesh->Attributes()->PrimaryNormals(), false);
	// OnMeshUpdated();		// skip for now as we're just doing normals
	return this;

}

UGeneratedMesh* UGeneratedMesh::SetToAngleThresholdNormals(float AngleThresholdDeg)
{
	Mesh->EnableAttributes();

	float NormalDotProdThreshold = FMathf::Cos(AngleThresholdDeg * FMathf::DegToRad);
	FMeshNormals FaceNormals(Mesh.Get());
	FaceNormals.ComputeTriangleNormals();
	const TArray<FVector3d>& Normals = FaceNormals.GetNormals();
	Mesh->Attributes()->PrimaryNormals()->CreateFromPredicate([&Normals, &NormalDotProdThreshold](int VID, int TA, int TB)
	{
		return Normals[TA].Dot(Normals[TB]) > NormalDotProdThreshold;
	}, 0);
	FMeshNormals::QuickRecomputeOverlayNormals(*Mesh);
	// OnMeshUpdated();		// skip for now as we're just doing normals
	return this;
}



UGeneratedMesh* UGeneratedMesh::RecomputeNormals()
{
	FMeshNormals::QuickRecomputeOverlayNormals(*Mesh);
	// OnMeshUpdated();		// skip for now as we're just doing normals
	return this;
}




UGeneratedMesh* UGeneratedMesh::Translate(FVector Translation)
{
	MeshTransforms::Translate(*Mesh, FVector3d(Translation));
	OnMeshUpdated();
	return this;
}

UGeneratedMesh* UGeneratedMesh::RotateQuat(FQuat Rotation, FVector OriginIn)
{
	FMatrix3d RotMatrix = FQuaterniond(Rotation).ToRotationMatrix();
	FVector3d Origin(OriginIn);
	MeshTransforms::ApplyTransform(*Mesh,
		[&RotMatrix, &Origin](const FVector3d& Pos) { return RotMatrix * (Pos - Origin) + Origin; },
		[&RotMatrix](const FVector3f& Normal) { return static_cast<FVector3f>(RotMatrix * static_cast<FVector3d>(Normal)); } );
	OnMeshUpdated();
	return this;
}
UGeneratedMesh* UGeneratedMesh::Rotate(FRotator RotationIn, FVector OriginIn)
{
	return RotateQuat(RotationIn.Quaternion());
}


UGeneratedMesh* UGeneratedMesh::Scale(FVector Scale, FVector Origin)
{
	MeshTransforms::Scale(*Mesh, FVector3d(Scale), FVector3d(Origin));
	OnMeshUpdated();
	return this;
}
UGeneratedMesh* UGeneratedMesh::ScaleUniform(float Scale, FVector Origin)
{
	return this->Scale(FVector(Scale, Scale, Scale), Origin);
}


UGeneratedMesh* UGeneratedMesh::Transform(FTransform Transform)
{
	MeshTransforms::ApplyTransform(*Mesh, FTransform3d(Transform));
	OnMeshUpdated();
	return this;
}




UGeneratedMesh* UGeneratedMeshPool::RequestMesh()
{
	if (CachedMeshes.Num() > 0)
	{
		return CachedMeshes.Pop(false);
	}
	UGeneratedMesh* NewMesh = NewObject<UGeneratedMesh>();

	// If we have allocated more meshes than our safety threshold, drop our holds on the existing meshes.
	// This will allow them to be garbage-collected (eventually)
	if (!ensure(AllCreatedMeshes.Num() < MeshCountSafetyThreshold))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGeneratedMeshPool Safety Threshold of %d Allocated Meshes exceeded! Releasing references to all current meshes and forcing a garbage collection."), MeshCountSafetyThreshold);
		AllCreatedMeshes.Reset();
		GEngine->ForceGarbageCollection(true);
	}

	AllCreatedMeshes.Add(NewMesh);
	return NewMesh;
}



void UGeneratedMeshPool::ReturnMesh(UGeneratedMesh* Mesh)
{
	if (ensure(Mesh))
	{
		Mesh->ResetMesh();
		if (ensure(CachedMeshes.Contains(Mesh) == false))
		{
			CachedMeshes.Add(Mesh);
		}
	}
}


void UGeneratedMeshPool::ReturnAllMeshes()
{
	CachedMeshes = AllCreatedMeshes;
	for (UGeneratedMesh* Mesh : CachedMeshes)
	{
		if (Mesh)
		{
			Mesh->ResetMesh();
		}
	}
	// work around inexplicable bug?
	CachedMeshes.RemoveAll([](UGeneratedMesh* Mesh) { return Mesh == nullptr; });
}

void UGeneratedMeshPool::FreeAllMeshes()
{
	CachedMeshes.Reset();
	AllCreatedMeshes.Reset();
}
