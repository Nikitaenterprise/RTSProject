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
	UStaticMeshComponent* StaticMesh = nullptr;

	int ResourceAmount = 100;
	
public:
	AResource();
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginDestroy() override;
protected:
	virtual void BeginPlay() override;

};
