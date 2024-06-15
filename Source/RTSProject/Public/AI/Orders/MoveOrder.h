#pragma once

#include "AbstractOrder.h"

#include "MoveOrder.generated.h"


UCLASS()
class RTSPROJECT_API UMoveOrder : public UAbstractOrder
{
	GENERATED_BODY()

public:
	virtual void ProcessOrder(ARTSAIController* InRTSAIController, UBlackboardComponent* InBlackboard) override;
};
