#pragma once

#include "UObject/Interface.h"
#include "Shootable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UShootable : public UInterface
{
	GENERATED_BODY()
};

class RTSPROJECT_API IShootable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void DealDamage(AActor* actor);
};
