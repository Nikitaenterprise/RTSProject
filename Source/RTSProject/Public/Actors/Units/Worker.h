﻿#pragma once

#include "Ship.h"
#include "Worker.generated.h"

class ABuilding;
class UGatherResourceAbility;
class UResourceGathererAttributeSet;

UCLASS()
class RTSPROJECT_API AWorker : public AShip
{
	GENERATED_BODY()

public:
	AWorker(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void SetResourceContainerBuilding(ABuilding* InResourceContainerBuilding) { ResourceContainerBuilding = InResourceContainerBuilding; }

	UFUNCTION(BlueprintCallable)
	ABuilding* GetResourceContainerBuilding() const { return ResourceContainerBuilding; } 
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	ABuilding* ResourceContainerBuilding {nullptr};
};