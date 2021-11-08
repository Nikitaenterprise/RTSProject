#include "Components/HealthShieldComponent.h"


UHealthShieldComponent::UHealthShieldComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UHealthShieldComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Health = StartingHealth;
	Shield = StartingShield;

	AActor* TestOwner = Cast<AActor>(GetOwner());
	if (!IsValid(TestOwner))
	{
		UE_LOG(LogTemp, Error, TEXT("TestOwner is nullptr in UHealthShieldComponent::InitializeComponent()"));
		return;
	}
	Owner = TestOwner;
	RecalculatePercents();
}

void UHealthShieldComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHealthShieldComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHealthShieldComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UHealthShieldComponent::DamageToHealth(int Damage)
{
	Health = FMath::Clamp<int>(Health - Damage, 0, MaxHealth);
	RecalculatePercents();
}

void UHealthShieldComponent::DamageToShield(int Damage)
{
	Shield = FMath::Clamp<int>(Shield - Damage, 0, MaxShield);
	RecalculatePercents();
}

void UHealthShieldComponent::TakeDamage(int Damage)
{
	if (Shield > 0) DamageToShield(Damage);
	else DamageToHealth(Damage);
	RecalculatePercents();
}

void UHealthShieldComponent::HealHealth(int Amount)
{
	Health = FMath::Clamp<int>(Health + Amount, 0, MaxHealth);
	RecalculatePercents();
}

void UHealthShieldComponent::HealShield(int Amount)
{
	Shield = FMath::Clamp<int>(Shield + Amount, 0, MaxShield);
	RecalculatePercents();
}

void UHealthShieldComponent::Heal(int Amount)
{
	if (Shield < Health) HealShield(Amount);
	else HealHealth(Amount);
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

