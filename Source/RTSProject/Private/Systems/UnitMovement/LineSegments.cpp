#include "Systems/UnitMovement/LineSegments.h"

#include "Kismet/KismetMathLibrary.h"
#include "Miscellaneous/AnglesFunctions.h"

void LineBuilder::BuildLineSegments(
	TArray<FLineSegment*>& OutLineSegments,
	const TArray<FVector>& Locations,
	const FVector& StartingLocation,
	const FRotator& StartingRotation,
	const FVector& ForwardVector,
	float MinTurnRadius)
{
	if (Locations.Num() == 0)
	{
		return;
	}
	
	OutLineSegments.Reserve(Locations.Num());
	for (size_t i = 0; i < Locations.Num() - 1; i++)
	{
		FVector Start = Locations[i];
		FVector End = Locations[i + 1];

		FVector StartToEnd = End - Start;

		const float ForwardAndDirToDestinationAngle = AnglesFunctions::FindAngleBetweenVectorsOn2D(
				ForwardVector.GetSafeNormal(),
				StartToEnd.GetSafeNormal());
		const bool bClockwiseRotation = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(
			ForwardVector.GetSafeNormal(),
			StartToEnd.GetSafeNormal());

		if (ForwardAndDirToDestinationAngle < 15)
		{
			OutLineSegments.Add(new FStraightLine(Start, End, StartToEnd.Size(), bClockwiseRotation, ForwardAndDirToDestinationAngle));
		}
		else
		{
			//DrawDebugLine(GetWorld(), FVector(Owner->GetActorLocation().X, Owner->GetActorLocation().Y, 150), FVector(ForwardVector.X * 20, ForwardVector.Y * 20, 150), FColor::Red, false, 50, 0, 5);
			//DrawDebugLine(GetWorld(), FVector(Start.X, Start.Y, 150), FVector(End.X, End.Y, 150), FColor::White, false, 50, 0, 5);
			
			/*GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, bClockwiseRotation ? TEXT("bClockwiseRotation=true") : TEXT("bClockwiseRotation=false"));
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, FString::Printf(TEXT("ForwardVector= %s"), *ForwardVector.ToString()));
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, FString::Printf(TEXT("ForwardAndDirToDestinationAngle= %s"), *FString::SanitizeFloat(ForwardAndDirToDestinationAngle)));*/
						
			// First we calculate the location of point P, which is the center of our turning circle,
			// and is always radius r away from the starting point.
			// If we are turning right from our initial direction, that means P is at an angle
			// of (initial_direction - 90) from the origin, so
			const float AngleToCenterOfTurningCircle = ForwardVector.Rotation().Yaw + (bClockwiseRotation ? 90 : -90);
			FVector2D CircleCenter;
			CircleCenter.X = StartingLocation.X + MinTurnRadius * UKismetMathLibrary::DegCos(AngleToCenterOfTurningCircle);
			CircleCenter.Y = StartingLocation.Y + MinTurnRadius * UKismetMathLibrary::DegSin(AngleToCenterOfTurningCircle);
			
			// Now that we know the location of the center point P,
			// we can calculate the distance from P to the destination
			FVector2D h = FVector2D(End) - CircleCenter;
			const float DistanceFromCircleCenterToDestination = h.Size();
			// If End point is inside Circle
			if (DistanceFromCircleCenterToDestination < MinTurnRadius)
			{
				OutLineSegments.Add(new FStraightLine(Start, End, StartToEnd.Size(), bClockwiseRotation, ForwardAndDirToDestinationAngle));
				continue;
			}

			// Now we can calculate the length of straight part, since we already know the lengths
			// of the other two sides of the right triangle, namely h and radius.
			// We can also determine angle theta from the right-triangle relationship:
			const float LengthOfStraightPart = sqrt(DistanceFromCircleCenterToDestination * DistanceFromCircleCenterToDestination - MinTurnRadius * MinTurnRadius);
			float Theta = UKismetMathLibrary::DegAcos(MinTurnRadius / DistanceFromCircleCenterToDestination); 
			// if rotating left then -theta, else +theta
			Theta = bClockwiseRotation ? -Theta : Theta;
			
			// Finally, to figure out the point Q at which to leave the circle and start
			// on the straight line, we need to know the total angle phi + theta,
			// and  is easily determined as the angle from P to the destination:
			const float Phi = UKismetMathLibrary::DegAtan2(h.Y, h.X);
			
			const FVector LeavingCirclePoint = FVector(
				CircleCenter.X + MinTurnRadius * UKismetMathLibrary::DegCos(Phi + Theta),
				CircleCenter.Y + MinTurnRadius * UKismetMathLibrary::DegSin(Phi + Theta),
				StartingLocation.Z
			);
			// Total angle to cover while moving on circle
			const float AngleOnCircle = 270 - Phi - Theta;
			
			OutLineSegments.Add(new FArcLine(Start, LeavingCirclePoint, AngleOnCircle * MinTurnRadius, bClockwiseRotation, CircleCenter, StartingRotation.Yaw, UKismetMathLibrary::DegreesToRadians(Phi + Theta)));
			OutLineSegments.Add(new FStraightLine(LeavingCirclePoint, End, LengthOfStraightPart, bClockwiseRotation, ForwardAndDirToDestinationAngle));
			i++;
		}
	}
}
