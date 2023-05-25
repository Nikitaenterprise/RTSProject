#pragma once

#include "AIController.h"
#include "Orders/OrdersProcessor.h"

#include "RTSAIController.generated.h"

struct RTSPROJECT_API FRTSBlackboardKeys
{
	static const FName EnemyTarget;
	static const FName MovementTargetLocation; // Location where we want to move
	static const FName bMovementTargetLocation; // Flag for above
	static const FName Order; 
};

UCLASS()
class RTSPROJECT_API ARTSAIController : public AAIController
{
	GENERATED_BODY()

public:
	void ChangeOrder(EOrderType InOrder) const;
};
