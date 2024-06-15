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
	virtual void Initialize(
		const TArray<AActor*>& InSelectedActors,
		AActor* InOrderCarrier,
		const FVector& InOrderLocation,
		const FHitResult& InHitResult);

	virtual bool Execute();
	virtual bool Undo();
	virtual void ProcessOrder(ARTSAIController* InRTSAIController, UBlackboardComponent* InBlackboard);

protected:
	UPROPERTY(Transient)
	TArray<AActor*> SelectedActors;

	FVector OrderLocation;
	FHitResult HitResultUnderCursor;

	UPROPERTY(Transient)
	AActor* OrderCarrier {nullptr};
};
