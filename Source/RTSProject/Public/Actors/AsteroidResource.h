#pragma once

#include "Resource.h"
#include "AsteroidResource.generated.h"

class AAsteroidField;

UCLASS()
class RTSPROJECT_API AAsteroidResource : public AResource
{
	GENERATED_BODY()
public:
	
	float Omega = 0.0;

private:
	
	bool bIsBeingConsumed = false;
	
public:
	
	AAsteroidResource();
	virtual void Tick(float DeltaSeconds) override;
	void IsEmpty();
	void SetIsBeingConsumed(bool bNewIsBeingConsumed) { bIsBeingConsumed = bNewIsBeingConsumed; }
	bool GetIsBeingConsumed() const { return bIsBeingConsumed; }

protected:
	
	virtual void BeginPlay() override;

private:
};
