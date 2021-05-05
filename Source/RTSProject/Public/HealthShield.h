#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthShield.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTSPROJECT_API UHealthShield : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Health Shield")
	AActor* Owner = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health Shield")
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
	UHealthShield();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	void DamageToHealth(int damage);
	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	void DamageToShield(int damage);
	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	void TakeDamage(int damage);

	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	void HealHealth(int amount);
	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	void HealShield(int amount);
	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	void Heal(int amount);

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
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
