#include "AnglesFunctions.h"

#include "Kismet/KismetMathLibrary.h"

float AnglesFunctions::FindAngleBetweenVectorsOn2D(const FVector& v1, const FVector& v2)
{
	FVector2D V1XY = FVector2D(v1);
	UKismetMathLibrary::Normalize2D(V1XY);
	FVector2D V2XY = FVector2D(v2);
	UKismetMathLibrary::Normalize2D(V2XY);
	
	return UKismetMathLibrary::DegAcos(UKismetMathLibrary::DotProduct2D(V1XY, V2XY));
}

bool AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(const FVector& v1, const FVector& v2)
{
	FVector2D V1XY = FVector2D(v1);
	UKismetMathLibrary::Normalize2D(V1XY);
	FVector2D V2XY = FVector2D(v2);
	UKismetMathLibrary::Normalize2D(V2XY);
	
	if (UKismetMathLibrary::CrossProduct2D(V1XY, V2XY) > 0) return true;
	return false;
}

float AnglesFunctions::FindAngleToRotateVectorOn2D(const FVector& v1, const FVector& v2)
{
	const float Angle = FindAngleBetweenVectorsOn2D(v1, v2);
	const bool bClockwise = FindRotationDirectionBetweenVectorsOn2D(v1, v2);
	// If rotation is clockwise then its angle * -1 
	if (bClockwise) return Angle * -1;
	// if rotation is counter-clockwise then it's just angle
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
	float Angle = UKismetMathLibrary::DegAcos(Dot);
	
	if (Cross < 0) Angle *= -1;

	return FRotator(0, Angle, 0);
}

