#pragma once

#include "Components/ActorComponent.h"
#include "AttackComponent.generated.h"

class ARTSPlayerController;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTSPROJECT_API UAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	AActor* Owner = nullptr;
	UPROPERTY(BlueprintReadOnly)
	ARTSPlayerController* PlayerController = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanAttack = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeAttacked = false;

public:

private:	
	
	UAttackComponent();
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
};
