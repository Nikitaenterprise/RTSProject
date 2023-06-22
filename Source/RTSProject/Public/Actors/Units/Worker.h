#pragma once

#include "Ship.h"
#include "Worker.generated.h"

class UGatherResourceAbility;
class UResourceGathererAttributeSet;

UCLASS()
class RTSPROJECT_API AWorker : public AShip
{
	GENERATED_BODY()

public:
	AWorker(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGatherResourceAbility> GatherResourceAbility;
	FGameplayAbilitySpecHandle GatherResourceAbilityHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	UResourceGathererAttributeSet* ResourceGathererAttributeSet {nullptr};
};
