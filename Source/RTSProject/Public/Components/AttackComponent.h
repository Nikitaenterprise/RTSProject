#pragma once

#include "Components/ActorComponent.h"
#include "AttackComponent.generated.h"

class ARTSPlayerController;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSPROJECT_API UAttackComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly)
	ARTSPlayerController* PlayerController = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanAttack = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeAttacked = true;
public:
	UAttackComponent();
	virtual void InitializeComponent() override;

	bool GetCanAttack() const { return bCanAttack; }
	bool GetCanBeAttacked() const { return bCanBeAttacked; }
};
