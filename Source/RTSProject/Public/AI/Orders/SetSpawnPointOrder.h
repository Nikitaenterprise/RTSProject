// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbstractOrder.h"
#include "SetSpawnPointOrder.generated.h"

UCLASS()
class RTSPROJECT_API USetSpawnPointOrder : public UAbstractOrder
{
	GENERATED_BODY()

public:
	virtual void ProcessOrder(ARTSAIController* InRTSAIController, UBlackboardComponent* InBlackboard) override;
	virtual bool Execute() override;
};
