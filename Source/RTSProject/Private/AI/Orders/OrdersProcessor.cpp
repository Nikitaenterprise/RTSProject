#include "AI/Orders/OrdersProcessor.h"

#include "AI/Orders/AbstractOrder.h"
#include "AI/Orders/AttackOrder.h"
#include "AI/Orders/MoveOrder.h"
#include "AI/Orders/GatherResourceOrder.h"
#include "AI/Orders/OrdersAcceptorComponent.h"
#include "AI/Orders/SetSpawnPointOrder.h"
#include "Core/RTSPlayerController.h"

void UOrdersProcessor::Initialize(ARTSPlayerController* InRTSPlayerController)
{
	RTSPlayerController = InRTSPlayerController;

	Orders.Add(EOrderType::MoveOrder, NewObject<UMoveOrder>());
	Orders.Add(EOrderType::GatherResource, NewObject<UGatherResourceOrder>());
	Orders.Add(EOrderType::AttackOrder, NewObject<UAttackOrder>());
	Orders.Add(EOrderType::SetSpawnPoint, NewObject<USetSpawnPointOrder>());
}

bool UOrdersProcessor::ProcessOrder(const TArray<EOrderType>& PossibleOrders, const UOrdersAcceptorComponent* InOrderAcceptorComponent, const FVector& OrderLocation, const FHitResult& HitResult)
{
	bool bSuccess = false;
	if (IsValid(InOrderAcceptorComponent) == false)
	{
		return false;
	}

	for (const auto& PossibleOrder : PossibleOrders)
	{
		if(Orders.Contains(PossibleOrder) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("UOrdersProcessor::ProcessAnOrder() No such order: %d"), PossibleOrder);
			continue;
		}

		UAbstractOrder* Order = Orders[PossibleOrder];
		OrdersQueue.Enqueue(Order);
		Order->Initialize(RTSPlayerController->GetSelectedActors(), InOrderAcceptorComponent->GetOwner(), OrderLocation, HitResult);
		bSuccess |= Order->Execute();
	}
	
	return bSuccess;
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
