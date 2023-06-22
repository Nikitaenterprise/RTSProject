#include "AI/Orders/OrdersProcessor.h"

#include "AI/Orders/AbstractOrder.h"
#include "AI/Orders/MoveOrder.h"
#include "AI/Orders/GatherResourceOrder.h"
#include "Core/RTSPlayerController.h"

void UOrdersProcessor::Initialize(ARTSPlayerController* InRTSPlayerController)
{
	RTSPlayerController = InRTSPlayerController;

	Orders.Add(EOrderType::MoveOrder, NewObject<UMoveOrder>());
	Orders.Add(EOrderType::GatherResource, NewObject<UGatherResourceOrder>());
}

bool UOrdersProcessor::ProcessOrder(EOrderType Order, const FVector& OrderLocation, const FHitResult& HitResult)
{
	if(!Orders.Contains(Order))
	{
		UE_LOG(LogTemp, Error, TEXT("UOrdersProcessor::ProcessAnOrder() No such order: %d"), Order);
		return false;
	}
	
	Orders[Order]->Initialize(RTSPlayerController->GetSelectedActors(), OrderLocation, HitResult);
	return Orders[Order]->Execute();
}
