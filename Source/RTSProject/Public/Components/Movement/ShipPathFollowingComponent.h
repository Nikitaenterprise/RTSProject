// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SplineComponent.h"
#include "Navigation/PathFollowingComponent.h"

#include "ShipPathFollowingComponent.generated.h"

class UShipMovementComponent;
class USplineComponent;

UCLASS()
class RTSPROJECT_API UShipPathFollowingComponent : public UPathFollowingComponent
{
	GENERATED_BODY()

public:
	virtual void OnComponentCreated() override;

	virtual FAIRequestID RequestMove(const FAIMoveRequest& RequestData, FNavPathSharedPtr InPath) override;
	virtual bool UpdateMovementComponent(bool bForce) override;
	virtual void SetMoveSegment(int32 SegmentStartIndex) override;
	virtual void FollowPathSegment(float DeltaTime) override;
	virtual void UpdatePathSegment() override;
	virtual void OnPathUpdated() override;
	virtual void OnPathfindingQuery(FPathFindingQuery& Query) override;

	FVector GetNextDestinationPoint(const float DeltaDistance) const;
	FRotator GetNextRotationPoint(const float DeltaDistance) const;

protected:
	void GenerateSpline();
	void DrawDebugSpline(const bool bDraw);
	
	TArray<FNavPathPoint> PathPoints;
	TArray<FSplinePoint> SplinePoints;

	UPROPERTY()
	USplineComponent* SplinePath {nullptr};

	UPROPERTY()
	UShipMovementComponent* ShipMovementComponent {nullptr};
	
	float InitialDistanceToDestination = 0;
	float CurrentDistanceAlongSpline = 0;
};
