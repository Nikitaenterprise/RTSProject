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

	health = startingHealth;
	shield = startingShield;
	owner = GetOwner();
	
}


// Called every frame
void UHealthShield::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UHealthShield::DamageToHealth(int damage)
{
	health = FMath::Clamp<int>(health - damage, 0, maxHealth);
}

void UHealthShield::DamageToShield(int damage)
{
	shield = FMath::Clamp<int>(shield - damage, 0, maxShield);
}

void UHealthShield::TakeDamage(int damage)
{
	if (shield > 0) DamageToShield(damage);
	else DamageToHealth(damage);
}

void UHealthShield::HealHealth(int amount)
{
	health = FMath::Clamp<int>(health + amount, 0, maxHealth);
}

void UHealthShield::HealShield(int amount)
{
	shield = FMath::Clamp<int>(shield + amount, 0, maxShield);
}

void UHealthShield::Heal(int amount)
{
	if (shield < health) HealShield(amount);
	else HealHealth(amount);
}

float UHealthShield::getHealthPercent()
{
	return (float)health/(float)maxHealth;
}

float UHealthShield::getShieldPercent()
{
	return (float)shield/(float)maxShield;
}

bool UHealthShield::IsDead()
{
	return (health <= 0);
}

