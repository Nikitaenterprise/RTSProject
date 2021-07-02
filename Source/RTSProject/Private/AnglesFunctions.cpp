#include "AnglesFunctions.h"

#include "Kismet/KismetMathLibrary.h"

float AnglesFunctions::FindAngleBetweenVectorsOn2D(const FVector& v1, const FVector& v2)
{
	const FVector2D V1XY = FVector2D(v1);
	const FVector2D V2XY = FVector2D(v2);
	return UKismetMathLibrary::Acos(UKismetMathLibrary::DotProduct2D(V1XY, V2XY) / V1XY.Size() * V2XY.Size());
}

// If cross product > 0 then rotate clockwise
bool AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(const FVector& v1, const FVector& v2)
{
	const FVector2D V1XY = FVector2D(v1);
	const FVector2D V2XY = FVector2D(v2);
	if (UKismetMathLibrary::CrossProduct2D(V1XY, V2XY) > 0) return true;
	return false;
}
// If rotate clockwise then its angle * -1 
// if counter-clockwise then just angle
float AnglesFunctions::FindAngleToRotateVectorOn2D(const FVector& v1, const FVector& v2)
{
	const float Angle = FindAngleBetweenVectorsOn2D(v1, v2);
	const bool bClockwise = FindRotationDirectionBetweenVectorsOn2D(v1, v2);
	if (bClockwise) return Angle * -1;
	else return Angle;
}

FRotator AnglesFunctions::FindYawRotatorOn2D(const FVector& ForwardVector, const FVector& LookAt)
{
	FVector2D Forward2D (ForwardVector.X, ForwardVector.Y);
	UKismetMathLibrary::Normalize2D(Forward2D);
	FVector2D Look2D(LookAt.X, LookAt.Y);
	UKismetMathLibrary::Normalize2D(Look2D);

	const float Cross = UKismetMathLibrary::CrossProduct2D(Forward2D, Look2D);
	const float Dot = UKismetMathLibrary::DotProduct2D(Forward2D, Look2D);
	float Angle = UKismetMathLibrary::Acos(Dot / (Forward2D.Size() * Look2D.Size()));
	
	if (Cross < 0) Angle *= -1;

	return FRotator(0, Angle, 0);
}

