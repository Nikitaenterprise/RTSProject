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

bool UOrdersProcessor::ProcessOrder(EOrderType OrderType, const FVector& OrderLocation, const FHitResult& HitResult)
{
	if(!Orders.Contains(OrderType))
	{
		UE_LOG(LogTemp, Error, TEXT("UOrdersProcessor::ProcessAnOrder() No such order: %d"), OrderType);
		return false;
	}

	UAbstractOrder* Order = Orders[OrderType];
	OrdersQueue.Enqueue(Order);
	Order->Initialize(RTSPlayerController->GetSelectedActors(), OrderLocation, HitResult);
	return Order->Execute();
}

bool UOrdersProcessor::Undo()
{
	UAbstractOrder* Order {nullptr};
	if (OrdersQueue.Dequeue(Order))
	{
		return Order->Undo();
	}
	return false;
}

bool UOrdersProcessor::Redo()
{
	UAbstractOrder* Order {nullptr};
	if (OrdersQueue.Dequeue(Order))
	{
		return Order->Execute();
	}
	return false;
}
