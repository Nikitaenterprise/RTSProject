#pragma once

#include "GameFramework/Actor.h"
#include "Planet.generated.h"

UCLASS()
class RTSPROJECT_API APlanet : public AActor
{
	GENERATED_BODY()
public:	
	
	APlanet();

protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void Tick(float DeltaTime) override;
};
