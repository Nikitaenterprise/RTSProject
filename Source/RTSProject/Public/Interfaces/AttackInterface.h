#pragma once

#include "UObject/Interface.h"
#include "AttackInterface.generated.h"

class UAttackComponent;
class ATurret;

UINTERFACE()
class UAttackInterface : public UInterface
{
	GENERATED_BODY()
};

class RTSPROJECT_API IAttackInterface
{
	GENERATED_BODY()
public:
	virtual UAttackComponent* GetAttackComponent() const = 0;
	virtual TArray<ATurret*>& GetTurrets() = 0; 
};
