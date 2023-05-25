#pragma once

#include "AbstractOrder.h"

#include "MoveOrder.generated.h"


UCLASS()
class RTSPROJECT_API UMoveOrder : public UAbstractOrder
{
	GENERATED_BODY()

public:
	virtual void Initialize(const TArray<AActor*>& InSelectedActors, FVector InOrderLocation = FVector::ZeroVector) override;

	virtual void ProcessOrder(ARTSAIController* InRTSAIController, UBlackboardComponent* InBlackboard) override;

protected:
	FVector MovementVector;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebug = false;
};
