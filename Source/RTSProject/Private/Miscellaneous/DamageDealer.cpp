#include "Miscellaneous/DamageDealer.h"

#include "Components/HealthShieldComponent.h"
#include "Actors/Ship.h"
#include "Actors/Turret.h"
#include "Actors/Rocket.h"

void DamageDealer::DealDamage(int DamageValue, AShip* Target)
{
	if (!Target) return;
	//Target->HealthShieldComponent->TakeDamage(DamageValue);
}

void DamageDealer::DealDamage(ARocket* Dealer, AShip* Target)
{
	if (Dealer && Target && Dealer->OwnerTurret->OwnerShip != Target)
	{
		Target->HealthShieldComponent->TakeDamage(int(Dealer->Damage));
	}
}

void DamageDealer::DealDamage(ARocket* Dealer, ATurret* Target)
{
	if (Dealer && Target && Dealer->OwnerTurret != Target)
	{
		Target->HealthShieldComponent->TakeDamage(int(Dealer->Damage));
	}
}
