#pragma once

#include "AbstractOrder.h"
#include "GatherResourceOrder.generated.h"


UCLASS()
class RTSPROJECT_API UGatherResourceOrder : public UAbstractOrder
{
	GENERATED_BODY()

public:
	virtual void ProcessOrder(ARTSAIController* InRTSAIController, UBlackboardComponent* InBlackboard) override;
	virtual bool Execute() override;
};
