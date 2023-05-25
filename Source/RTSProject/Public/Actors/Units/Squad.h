#pragma once

#include "Squad.generated.h"

class USimpleMovementComponent;

UCLASS()
class RTSPROJECT_API ASquad : public APawn
{
	GENERATED_BODY()

public:
	ASquad();
	virtual void Tick(float DeltaTime) override;
	void AddToSquad(AActor* InActor);
	void RemoveFromSquad(AActor* InActor);
	void SetSquadLeader(AActor* InSquadLeader);
	const TArray<AActor*>& GetSquad() const { return Squadron; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Squadron")
	USimpleMovementComponent* MovementComponent = nullptr;
	
	UPROPERTY()
	TArray<AActor*> Squadron;

	UPROPERTY()
	AActor* SquadLeader = nullptr;
};
