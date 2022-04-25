#pragma once

#include "Components/ActorComponent.h"
#include "AttackComponent.generated.h"

class ARTSPlayerController;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSPROJECT_API UAttackComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanAttack {true};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeAttacked {true};
public:
	UAttackComponent();

	UFUNCTION(BlueprintCallable)
	bool GetCanAttack() const { return bCanAttack; }
	UFUNCTION(BlueprintCallable)
	bool GetCanBeAttacked() const { return bCanBeAttacked; }
	UFUNCTION(BlueprintCallable)
	void RequestAttack(const AActor* ActorToAttack);
};
