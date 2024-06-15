#pragma once

#include "Components/Movement/SimpleMovementComponent.h"
#include "Interfaces/Squadable.h"
#include "Components/Movement/BoidMovementComponent.h"

#include "Fighter.generated.h"

UCLASS()
class RTSPROJECT_API AFighter : public APawn,
	public ISquadable
{
	GENERATED_BODY()

public:
	AFighter(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	virtual UPawnMovementComponent* GetMovementComponent() const override { return MovementComponent; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fighter")
	UBoidMovementComponent* MovementComponent = nullptr;
};
