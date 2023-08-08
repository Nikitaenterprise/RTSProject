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
	virtual void Initialize(const TArray<AActor*>& InSelectedActors,
		const FVector& InOrderLocation,
		const FHitResult& InHitResult);

	virtual bool Execute();
	virtual bool Undo();
	virtual void ProcessOrder(ARTSAIController* InRTSAIController, UBlackboardComponent* InBlackboard);

protected:
	UPROPERTY()
	TArray<AActor*> SelectedActors;

	FVector OrderLocation;
	FHitResult HitResultUnderCursor;
};
