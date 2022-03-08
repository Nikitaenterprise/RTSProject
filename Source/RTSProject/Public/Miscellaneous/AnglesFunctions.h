#pragma once

class RTSPROJECT_API AnglesFunctions
{
public:
	/// <summary>
	/// Finds angle between two vectors on 2D (3D projection on XY plane)
	/// </summary>
	/// <param name="v1">First vector</param>
	/// <param name="v2">Second vector</param>
	/// <returns>Returns angle in degrees between two vectors 0..180</returns>
	static float FindAngleBetweenVectorsOn2D(const FVector& v1, const FVector& v2);
	/// <summary>
	/// Finds rotation direction from first vector to second vector on 2D (3D projection on XY plane)
	/// Rotation is found by calculating cross product
	/// </summary>
	/// <param name="v1">First vector</param>
	/// <param name="v2">Second vector</param>
	/// <returns>True if clockwise rotation, false if counter-clockwise rotation</returns>
	static bool FindRotationDirectionBetweenVectorsOn2D(const FVector& v1, const FVector& v2);
	/// <summary>
	/// Finds angle to rotate first vector to align with second vector on 2D (3D projection on XY plane)
	/// </summary>
	/// <param name="v1">First vector</param>
	/// <param name="v2">Second vector</param>
	/// <returns>Angle to rotate in degrees (signed) -180..180</returns>
	static float FindAngleToRotateVectorOn2D(const FVector& v1, const FVector& v2);
	/// <summary>
	/// Finds rotation over Z axis between two vectors (e.g. rotation angle between vectors projection on 2D-XY plane)
	/// </summary>
	/// <param name="ForwardVector">Forward vector</param>
	/// <param name="LookAt">Look at vector</param>
	/// <returns>Rotator with pitch=0, roll=0, yaw=rotation angle in degrees -180..180</returns>
	static FRotator FindYawRotatorOn2D(const FVector& ForwardVector, const FVector& LookAt);
};
