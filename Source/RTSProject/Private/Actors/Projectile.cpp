#include "Actors/Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Actors/Units/Ship.h"
#include "Actors/Units/Turret.h"
#include "AbilitySystemComponent.h"
#include "GAS/ProjectileAttributeSet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GAS/HealthShieldAttributeSet.h"

AProjectile::AProjectile(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->ProjectileGravityScale = 0;
	ProjectileMovementComponent->Bounciness = 0;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());
	StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ProjectileAttributeSet = CreateDefaultSubobject<UProjectileAttributeSet>(TEXT("ProjectileAttributeSet"));
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	ATurret* TestOwner = Cast<ATurret>(GetOwner());
	if (!IsValid(TestOwner))
	{
		UE_LOG(LogTemp, Error, TEXT("TestOwner is nullptr in ARocket::BeginPlay()"));
		return;
	}
	OwnerTurret = TestOwner;

	AbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(ProjectileAttributeSet);
	
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([This = TWeakObjectPtr<ThisClass>(this)]()
	{
		This->Destroy();
	});
	GetWorld()->GetTimerManager().SetTimer(MaxLifeTimeTimerHandle,
		TimerDelegate, ProjectileAttributeSet->GetMaxLifeTime(), false);
	StartMoving();
}

void AProjectile::StartMoving()
{
	ProjectileMovementComponent->MaxSpeed = ProjectileAttributeSet->GetMaxSpeed();
	const FVector HeadingVector = OwnerTurret->GetActorForwardVector();
	ProjectileMovementComponent->AddForce(HeadingVector*ProjectileAttributeSet->GetMaxSpeed());
}

void AProjectile::OnOverlapBegin_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const auto Ship = Cast<AShip>(OtherActor);
	if (!Ship)
	{
		return;
	}
	if (OwnerTurret->GetOwnerShip() == Ship)
	{
		return;
	}
	OnProjectileHit.ExecuteIfBound(Ship);
	const auto ShipAbilityComponent = Ship->GetAbilitySystemComponent();
	const auto ShipHealthShieldAttributeSet = Ship->GetHealthShieldAttributeSet();
	if (ShipAbilityComponent && ShipHealthShieldAttributeSet)
	{
		auto DamageEffect = Cast<UGameplayEffect>(DamageEffectClass.Get());
		if (DamageEffect == nullptr)
		{
			DamageEffect = NewObject<UGameplayEffect>();
			DamageEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
			FGameplayModifierInfo DamageModifierInfo;
			DamageModifierInfo.Attribute = ShipHealthShieldAttributeSet->GetCurrentShieldAttribute();
			DamageModifierInfo.ModifierOp = EGameplayModOp::Additive;
			DamageModifierInfo.ModifierMagnitude = FScalableFloat(ProjectileAttributeSet->GetDamage() * -1);
			DamageEffect->Modifiers.Add(DamageModifierInfo);
		}
		ShipAbilityComponent->ApplyGameplayEffectToSelf(DamageEffect, 0, ShipAbilityComponent->MakeEffectContext());
	}
	Destroy();
}
