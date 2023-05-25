#pragma once

#include "UObject/Interface.h"
#include "Squadable.generated.h"

class UPawnMovementComponent;

UINTERFACE()
class USquadable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Use this interface to tell that actor can be added to squad
 */
class RTSPROJECT_API ISquadable
{
	GENERATED_BODY()

public:
	virtual UPawnMovementComponent* GetMovementComponent() const = 0;
};
