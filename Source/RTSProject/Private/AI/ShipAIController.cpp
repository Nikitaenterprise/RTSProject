#include "AI/ShipAIController.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Actors/Units/Ship.h"
#include "Components/Movement/ShipPathFollowingComponent.h"

AShipAIController::AShipAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UShipPathFollowingComponent>(TEXT("PathFollowingComponent")))
{
}

FPathFollowingRequestResult AShipAIController::MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath)
{
	return Super::MoveTo(MoveRequest, CurrentNavPath);
}

void AShipAIController::FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const
{	
	if (UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		// Project the ship's current location and destination onto the Navigation Mesh
		FNavLocation StartLocation;
		NavSystem->ProjectPointToNavigation(GetPawn()->GetActorLocation(), StartLocation);
		FNavLocation EndLocation;
		NavSystem->ProjectPointToNavigation(MoveRequest.GetDestination(), EndLocation);

		// Find the path from the ship's current location to its destination
		UNavigationPath* NavPath = NavSystem->FindPathToLocationSynchronously(GetWorld(), StartLocation.Location, EndLocation.Location, GetOwner());
	
		OutPath = NavPath->GetPath();	
		return;
	}
	
	Super::FindPathForMoveRequest(MoveRequest, Query, OutPath);
}

void AShipAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	Ship = Cast<AShip>(GetPawn());
	ShipMovementComponent = Cast<UShipMovementComponent>(Ship->GetMovementComponent());
}

void AShipAIController::ObtainPlayerState()
{
	Super::ObtainPlayerState();

	RTSPlayerState = Ship ? Ship->GetRTSPlayerState() : nullptr;
}
