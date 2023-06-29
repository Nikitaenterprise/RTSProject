#pragma once

#include "UObject/Object.h"

#include "OrdersProcessor.generated.h"

UENUM(BlueprintType)
enum class EOrderType : uint8
{
	NoOrder = 0,
	MoveOrder,
	AttackOrder,
	MoveAndAttackOrder,
	StopOrder,
	HoldPositionOrder,
	GatherResource
};

class UAbstractOrder;
class ARTSPlayerController;

UCLASS()
class RTSPROJECT_API UOrdersProcessor : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(ARTSPlayerController* InRTSPlayerController);

	bool ProcessOrder(EOrderType Order, const FVector& OrderLocation, const FHitResult& HitResult);
	
protected:
	UPROPERTY()
	ARTSPlayerController* RTSPlayerController = nullptr;

	UPROPERTY()
	TMap<EOrderType, UAbstractOrder*> Orders;
};
