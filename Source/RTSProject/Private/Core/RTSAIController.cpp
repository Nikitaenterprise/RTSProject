#include "Core/RTSAIController.h"

#include "NavigationSystem.h"

//TArray<FVector> ARTSAIController::SearchPath(const FVector& _Location)
//{
//	FPathFindingQuery Query;
//	FAIMoveRequest Request(_Location);
//	
//	const bool bk = FindPathForMoveRequest(Request,Query, )
//	//PreparePathfinding deprecated
//	const bool bValidQuery = PreparePathfinding(Query, _Location, nullptr);
//	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
//	TArray<FVector> Result;
//
//	if (NavSys)
//	{
//		FPathFindingResult PathResult = NavSys->FindPathSync(Query);
//
//		if (PathResult.Result != ENavigationQueryResult::Error)
//		{
//			if (PathResult.IsSuccessful() && PathResult.Path.IsValid())
//			{
//				for (FNavPathPoint point : PathResult.Path->GetPathPoints())
//				{
//					Result.Add(point.Location);
//				}
//			}
//		}
//		else
//		{
//			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Cyan, TEXT("Pathfinding failed."));
//		}
//	}
//	else
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Cyan, TEXT("Can't find navigation system."));
//	}
//
//	return Result;
//}

