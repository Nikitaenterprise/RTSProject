#pragma once

#include "Components/ActorComponent.h"
#include "HealthShieldComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTSPROJECT_API UHealthShieldComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Health Shield")
	AActor* Owner = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Shield")
	int MaxHealth = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Shield")
	int MaxShield = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Shield")
	int StartingHealth = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Shield")
	int StartingShield = 100;

private:

	int Health = 0;
	int Shield = 0;
	float HealthPercent = 0;
	float ShieldPercent = 0;

public:
	
	UHealthShieldComponent();
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	void DamageToHealth(int Damage);
	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	void DamageToShield(int Damage);
	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	void TakeDamage(int Damage);

	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	void HealHealth(int Amount);
	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	void HealShield(int Amount);
	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	void Heal(int Amount);

	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	float GetHealthPercent();
	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	float GetShieldPercent();

	const float* GetHealthPercentPtr() const;
	const float* GetShieldPercentPtr() const;

	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	bool IsDead();

private:

	inline void RecalculatePercents();

};