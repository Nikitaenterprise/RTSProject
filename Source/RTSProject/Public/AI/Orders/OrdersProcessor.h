#pragma once

#include "UObject/Object.h"

#include "OrdersProcessor.generated.h"

class UOrdersAcceptorComponent;

UENUM(BlueprintType)
enum class EOrderType : uint8
{
	NoOrder = 0,
	MoveOrder,
	AttackOrder,
	MoveAndAttackOrder,
	StopOrder,
	HoldPositionOrder,
	GatherResource,
	SetSpawnPoint,
	ActivateAbility
};

class UAbstractOrder;
class ARTSPlayerController;

UCLASS()
class RTSPROJECT_API UOrdersProcessor : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(ARTSPlayerController* InRTSPlayerController);

	bool ProcessOrder(const TArray<EOrderType>& PossibleOrders, const UOrdersAcceptorComponent* InOrderAcceptorComponent, const FVector& OrderLocation, const FHitResult& HitResult);
	bool Undo();
	bool Redo();

protected:
	UPROPERTY()
	ARTSPlayerController* RTSPlayerController = nullptr;

	UPROPERTY()
	TMap<EOrderType, UAbstractOrder*> Orders;

	TQueue<UAbstractOrder*> OrdersQueue;

	int CurrentOrderIndex = 0;
};
