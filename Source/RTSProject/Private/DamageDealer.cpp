// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageDealer.h"

#include "HealthShield.h"
#include "Ship.h"
#include "Turret.h"
#include "Rocket.h"

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
		//Target->HealthShieldComponent->TakeDamage(int(Dealer->Damage));
	}
}
