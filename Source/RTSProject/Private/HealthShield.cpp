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
	RecalculatePercents();
}


// Called every frame
void UHealthShield::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}


void UHealthShield::DamageToHealth(int damage)
{
	Health = FMath::Clamp<int>(Health - damage, 0, MaxHealth);
	RecalculatePercents();
}

void UHealthShield::DamageToShield(int damage)
{
	Shield = FMath::Clamp<int>(Shield - damage, 0, MaxShield);
	RecalculatePercents();
}

void UHealthShield::TakeDamage(int damage)
{
	if (Shield > 0) DamageToShield(damage);
	else DamageToHealth(damage);
	RecalculatePercents();
}

void UHealthShield::HealHealth(int amount)
{
	Health = FMath::Clamp<int>(Health + amount, 0, MaxHealth);
	RecalculatePercents();
}

void UHealthShield::HealShield(int amount)
{
	Shield = FMath::Clamp<int>(Shield + amount, 0, MaxShield);
	RecalculatePercents();
}

void UHealthShield::Heal(int amount)
{
	if (Shield < Health) HealShield(amount);
	else HealHealth(amount);
}

float UHealthShield::GetHealthPercent()
{
	return HealthPercent;
}

float UHealthShield::GetShieldPercent()
{
	return ShieldPercent;
}

const float* UHealthShield::GetHealthPercentPtr() const
{
	return &HealthPercent;
}

const float* UHealthShield::GetShieldPercentPtr() const
{
	return &ShieldPercent;
}

bool UHealthShield::IsDead()
{
	return (Health <= 0);
}

void UHealthShield::RecalculatePercents()
{
	HealthPercent = static_cast<float>(Health) / static_cast<float>(MaxHealth);
	ShieldPercent = static_cast<float>(Shield) / static_cast<float>(MaxShield);
}

