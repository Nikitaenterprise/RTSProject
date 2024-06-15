// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbstractOrder.h"
#include "AttackOrder.generated.h"

UCLASS()
class RTSPROJECT_API UAttackOrder : public UAbstractOrder
{
	GENERATED_BODY()

public:
	virtual void ProcessOrder(ARTSAIController* InRTSAIController, UBlackboardComponent* InBlackboard) override;
	virtual bool Execute() override;
};
