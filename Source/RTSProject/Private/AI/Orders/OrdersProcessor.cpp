#include "AI/Orders/OrdersProcessor.h"

#include "AI/Orders/AbstractOrder.h"
#include "AI/Orders/MoveOrder.h"
#include "Core/RTSPlayerController.h"

void UOrdersProcessor::Initialize(ARTSPlayerController* InRTSPlayerController)
{
	RTSPlayerController = InRTSPlayerController;

	Orders.Add(EOrderType::MoveOrder, NewObject<UMoveOrder>());
}

bool UOrdersProcessor::ProcessOrder(EOrderType Order, const FVector& OrderLocation)
{
	if(!Orders.Contains(Order))
	{
		UE_LOG(LogTemp, Error, TEXT("UPZ_SquadOrdersProcessor::ProcessAnOrder() ERROR. No such order: %d"), Order);
		return false;
	}
	
	Orders[Order]->Initialize(RTSPlayerController->GetSelectedActors(), OrderLocation);
	return Orders[Order]->Execute();
}
