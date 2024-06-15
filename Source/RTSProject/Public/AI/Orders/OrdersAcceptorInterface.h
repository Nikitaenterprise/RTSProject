// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"

#include "OrdersAcceptorInterface.generated.h"

class UOrdersAcceptorComponent;

UINTERFACE()
class UOrdersAcceptorInterface : public UInterface
{
	GENERATED_BODY()
};

class RTSPROJECT_API IOrdersAcceptorInterface
{
	GENERATED_BODY()

public:
	virtual UOrdersAcceptorComponent* GetOrdersAcceptorComponent() const = 0;

};
