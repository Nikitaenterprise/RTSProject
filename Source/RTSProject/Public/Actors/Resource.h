#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Resource.generated.h"

class UStaticMeshComponent;

UCLASS()
class RTSPROJECT_API AResource : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* StaticMesh = NULL;

	int ResourceAmount = 100;

	float DeltaTime = 0;
	float PastTime = 0;

public:
	AResource();
	virtual void Tick(float mainDeltaTime) override;
	
protected:
	virtual void BeginPlay() override;

};
