#pragma once

#include "UObject/Object.h"

#include "AbstractOrder.generated.h"

class ARTSAIController;
class UBlackboardComponent;

UCLASS()
class RTSPROJECT_API UAbstractOrder : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(const TArray<AActor*>& InSelectedActors, FVector InOrderLocation = FVector::ZeroVector);

	virtual bool Execute();
	virtual void ProcessOrder(ARTSAIController* InRTSAIController, UBlackboardComponent* InBlackboard);

protected:
	UPROPERTY()
	TArray<AActor*> SelectedActors;

	FVector OrderLocation;
};
