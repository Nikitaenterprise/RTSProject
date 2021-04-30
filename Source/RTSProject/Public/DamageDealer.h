#pragma once

#include "CoreMinimal.h"

class AShip;
class ATurret;
class ARocket;

class RTSPROJECT_API DamageDealer
{
public:
	static void DealDamage(int DamageValue, AShip* Target);
	static void DealDamage(ARocket* Dealer, AShip* Target);
	static void DealDamage(ARocket* Dealer, ATurret* Target);
};
