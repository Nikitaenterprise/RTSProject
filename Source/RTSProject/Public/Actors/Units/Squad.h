#pragma once

#include "Actors/Units/BaseUnitWithAbility.h"

#include "Squad.generated.h"

class USimpleMovementComponent;
class UBoidsMovementSystem;
class AFighter;

UCLASS()
class RTSPROJECT_API ASquad : public ABaseUnitWithAbility
{
	GENERATED_BODY()

public:
	ASquad(const FObjectInitializer& OI);
	virtual void Tick(float DeltaTime) override;
	void AddToSquad(AActor* InActor);
	void RemoveFromSquad(AActor* InActor);
	void SetSquadLeader(AActor* InSquadLeader);
	const TArray<AActor*>& GetSquad() const { return Squadron; }

	void MoveTo(const FVector& Location);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Squadron")
	USimpleMovementComponent* MovementComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Squadron")
	UBoidsMovementSystem* BoidSystem = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category = "Squadron")
	TArray<AActor*> Squadron;

	UPROPERTY(BlueprintReadWrite, Category = "Squadron")
	AActor* SquadLeader = nullptr;

	UPROPERTY(EditAnywhere, Category = "Squadron")
	TSubclassOf<AFighter> FighterClass;

	UPROPERTY(EditAnywhere, Category = "Squadron")
	int32 InitialSquadronSize = 10;
};
