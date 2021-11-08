#pragma once

#include "CoreMinimal.h"
#include "Resource.h"
#include "AsteroidResource.generated.h"

class AAsteroidField;

UCLASS()
class RTSPROJECT_API AAsteroidResource : public AResource
{
	GENERATED_BODY()

public:
	float Omega = 0.0;

	AAsteroidField* Owner = nullptr;

public:
	virtual void Tick(float MainDeltaTime) override;

protected:
	virtual void BeginPlay() override;
};
