// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthShieldComponent.h"

// Sets default values for this component's properties
UHealthShieldComponent::UHealthShieldComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UHealthShieldComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = StartingHealth;
	Shield = StartingShield;
	Owner = GetOwner();
	RecalculatePercents();
}

void UHealthShieldComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}


void UHealthShieldComponent::DamageToHealth(int damage)
{
	Health = FMath::Clamp<int>(Health - damage, 0, MaxHealth);
	RecalculatePercents();
}

void UHealthShieldComponent::DamageToShield(int damage)
{
	Shield = FMath::Clamp<int>(Shield - damage, 0, MaxShield);
	RecalculatePercents();
}

void UHealthShieldComponent::TakeDamage(int damage)
{
	if (Shield > 0) DamageToShield(damage);
	else DamageToHealth(damage);
	RecalculatePercents();
}

void UHealthShieldComponent::HealHealth(int amount)
{
	Health = FMath::Clamp<int>(Health + amount, 0, MaxHealth);
	RecalculatePercents();
}

void UHealthShieldComponent::HealShield(int amount)
{
	Shield = FMath::Clamp<int>(Shield + amount, 0, MaxShield);
	RecalculatePercents();
}

void UHealthShieldComponent::Heal(int amount)
{
	if (Shield < Health) HealShield(amount);
	else HealHealth(amount);
}

float UHealthShieldComponent::GetHealthPercent()
{
	return HealthPercent;
}

float UHealthShieldComponent::GetShieldPercent()
{
	return ShieldPercent;
}

const float* UHealthShieldComponent::GetHealthPercentPtr() const
{
	return &HealthPercent;
}

const float* UHealthShieldComponent::GetShieldPercentPtr() const
{
	return &ShieldPercent;
}

bool UHealthShieldComponent::IsDead()
{
	return (Health <= 0);
}

void UHealthShieldComponent::RecalculatePercents()
{
	HealthPercent = static_cast<float>(Health) / static_cast<float>(MaxHealth);
	ShieldPercent = static_cast<float>(Shield) / static_cast<float>(MaxShield);
}

