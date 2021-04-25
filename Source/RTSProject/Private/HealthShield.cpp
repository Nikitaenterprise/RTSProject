// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthShield.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UHealthShield::UHealthShield()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
}


// Called when the game starts
void UHealthShield::BeginPlay()
{
	Super::BeginPlay();

	Health = StartingHealth;
	Shield = StartingShield;
	Owner = GetOwner();
	
}


// Called every frame
void UHealthShield::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UHealthShield::DamageToHealth(int damage)
{
	Health = FMath::Clamp<int>(Health - damage, 0, MaxHealth);
}

void UHealthShield::DamageToShield(int damage)
{
	Shield = FMath::Clamp<int>(Shield - damage, 0, MaxShield);
}

void UHealthShield::TakeDamage(int damage)
{
	if (Shield > 0) DamageToShield(damage);
	else DamageToHealth(damage);
}

void UHealthShield::HealHealth(int amount)
{
	Health = FMath::Clamp<int>(Health + amount, 0, MaxHealth);
}

void UHealthShield::HealShield(int amount)
{
	Shield = FMath::Clamp<int>(Shield + amount, 0, MaxShield);
}

void UHealthShield::Heal(int amount)
{
	if (Shield < Health) HealShield(amount);
	else HealHealth(amount);
}

float UHealthShield::GetHealthPercent()
{
	return (float)Health/(float)MaxHealth;
}

float UHealthShield::GetShieldPercent()
{
	return (float)Shield/(float)MaxShield;
}

bool UHealthShield::IsDead()
{
	return (Health <= 0);
}

