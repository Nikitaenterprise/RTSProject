#include "AnglesFunctions.h"

#include "Kismet/KismetMathLibrary.h"

AnglesFunctions::AnglesFunctions()
{
}

AnglesFunctions::~AnglesFunctions()
{
}

float AnglesFunctions::FindAngleBetweenVectorsOn2D(const FVector& v1, const FVector& v2)
{
	FVector2D v1XY = FVector2D(v1.X, v1.Y);
	FVector2D v2XY = FVector2D(v2.X, v2.Y);
	return UKismetMathLibrary::Acos(UKismetMathLibrary::DotProduct2D(v1XY, v2XY) / v1XY.Size() * v2XY.Size());
}

// If cross product > 0 then rotate clockwise
bool AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(const FVector& v1, const FVector& v2)
{
	FVector2D v1XY = FVector2D(v1.X, v1.Y);
	FVector2D v2XY = FVector2D(v2.X, v2.Y);
	if (UKismetMathLibrary::CrossProduct2D(v1XY, v2XY) > 0) return true;
	return false;
}
// If rotate clockwise then its angle * -1 
// if counter-clockwise then just angle
float AnglesFunctions::FindAngleToRotateVectorOn2D(const FVector& v1, const FVector& v2)
{
	float angle = FindAngleBetweenVectorsOn2D(v1, v2);
	bool clockwise = FindRotationDirectionBetweenVectorsOn2D(v1, v2);
	if (clockwise) return angle * -1;
	else return angle;
}

FRotator AnglesFunctions::FindYawRotatorOn2D(const FVector& ForwardVector, const FVector& LookAt)
{
	FVector2D Forward2D (ForwardVector.X, ForwardVector.Y);
	UKismetMathLibrary::Normalize2D(Forward2D);
	FVector2D Look2D(LookAt.X, LookAt.Y);
	UKismetMathLibrary::Normalize2D(Look2D);
	
	float cross = UKismetMathLibrary::CrossProduct2D(Forward2D, Look2D);
	float dot = UKismetMathLibrary::DotProduct2D(Forward2D, Look2D);
	float angle = UKismetMathLibrary::Acos(dot / (Forward2D.Size() * Look2D.Size()));
	
	if (cross < 0) angle *= -1;

	return FRotator(0, angle, 0);
}

