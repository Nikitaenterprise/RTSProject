// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PawnMovementComponent.h"
#include "SimpleMovementComponent.generated.h"


UCLASS(ClassGroup=(Movement), meta=(BlueprintSpawnableComponent))
class RTSPROJECT_API USimpleMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	USimpleMovementComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

};
