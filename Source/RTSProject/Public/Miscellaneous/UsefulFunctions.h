#pragma once

/**
 * Class with useful static functions
 */
class RTSPROJECT_API UsefulFunctions
{
public:
	/// <summary>
	/// Get vertices data (positions) from StaticMeshComponent with respect to MeshTransform
	/// Example:
	///			AActor* Actor;
	///			TArray<FVector> Vertices;
	///			bool bCompleted = GetVertices(Actor->StaticMesh, Actor->GetActorTransform(), Vertices);
	/// </summary>
	/// <param name="StaticMeshComponent">Static Mesh Component to get data from</param>
	/// <param name="MeshTransform">Transform of static mesh</param>
	/// <param name="Vertices">TArray where data will be written</param>
	/// <returns>Returns true if succeeded, false otherwise</returns>
	static bool GetVertices(const UStaticMeshComponent* StaticMeshComponent, const FTransform& MeshTransform, TArray<FVector>& Vertices);

	/// <summary>
	/// Get sorted array of vectors. First element is closest vector from TArray<FVector>Vectors to Point
	/// </summary>
	/// <param name="Vectors">TArray of vectors</param>
	/// <param name="Point">FVector from which distance will be calculated</param>
	/// <returns>Sorted array of vectors</returns>
	static TArray<FVector> GetSortedVectorsByDistanceFromPoint(const TArray<FVector>& Vectors, const FVector& Point);

	/// <summary>
	/// Returns scale units from project settings:  centimeters, meters, ...
	/// By default DistanceUnits is meters
	/// </summary>
	/// <returns>Float DistanceUnits: Millimeters=1000.0, Meters=1.0, Kilometers=0.001</returns>
	static float GetScaleValueFromSettings();
};
