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
	AActor* owner = NULL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health Shield")
	int maxHealth = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Shield")
	int maxShield = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Shield")
	int startingHealth = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Shield")
	int startingShield = 100;
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Health Shield")
	int health = 0;
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Health Shield")
	int shield = 0;

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
	float getHealthPercent();
	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	float getShieldPercent();
	UFUNCTION(BlueprintCallable, Category = "Health Shield")
	bool IsDead();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
