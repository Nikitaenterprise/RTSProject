// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Orders/OrdersAcceptorComponent.h"

#include "AI/Orders/OrdersProcessor.h"
#include "Core/RTSPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UOrdersAcceptorComponent::ProcessOrder(const FHitResult& HitResult)
{
	if (IsValid(OrdersProcessor) == false)
	{
		return;
	}

	const FVector& OrderLocation = HitResult.Location;
	const AActor* HitActor = HitResult.GetActor();
	OrdersProcessor->ProcessOrder(PossibleOrders, this, OrderLocation, HitResult);
}

void UOrdersAcceptorComponent::AddPossibleOrder(EOrderType NewOrderType)
{
	PossibleOrders.Add(NewOrderType);
}

void UOrdersAcceptorComponent::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0);
	if (IsValid(PlayerController) == false)
	{
		return;
	}

	ARTSPlayerController* RTSController = Cast<ARTSPlayerController>(PlayerController);
	if (IsValid(RTSController) == false)
	{
		return;
	}

	OrdersProcessor = NewObject<UOrdersProcessor>();
	OrdersProcessor->Initialize(RTSController);
}
