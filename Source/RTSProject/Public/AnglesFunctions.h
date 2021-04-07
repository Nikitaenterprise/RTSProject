#pragma once

#include "CoreMinimal.h"

class RTSPROJECT_API AnglesFunctions
{
public:
	static float FindAngleBetweenVectorsOn2D(const FVector& v1, const FVector& v2);
	static bool FindRotationDirectionBetweenVectorsOn2D(const FVector& v1, const FVector& v2);
	static float FindAngleToRotateVectorOn2D(const FVector& v1, const FVector& v2);
	static FRotator FindYawRotatorOn2D(const FVector& ForwardVector, const FVector& LookAt);
};
