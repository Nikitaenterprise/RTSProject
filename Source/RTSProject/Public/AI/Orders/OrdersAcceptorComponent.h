// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OrdersProcessor.h"
#include "Components/ActorComponent.h"

#include "OrdersAcceptorComponent.generated.h"

class UAbstractOrder;
class UOrdersProcessor;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSPROJECT_API UOrdersAcceptorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void ProcessOrder(const FHitResult& HitResult);
	void AddPossibleOrder(EOrderType NewOrderType);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Transient)
	UOrdersProcessor* OrdersProcessor {nullptr};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<EOrderType> PossibleOrders;
};
