#pragma once

#include "Actors/Units/BaseUnitWithAbility.h"

#include "Squad.generated.h"

class UHierarchicalInstancedStaticMeshComponent;
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

	void MoveTo(const FVector& Location);

protected:
	virtual void BeginPlay() override;
	void GeneratePositions(TArray<FVector>& OutPositions);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Squadron")
	USimpleMovementComponent* MovementComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Squadron")
	UBoidsMovementSystem* BoidsSystem = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Squadron")
	UHierarchicalInstancedStaticMeshComponent* HISMComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Squadron")
	UStaticMesh* StaticMeshToUseInHISM = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Squadron")
	int32 InitialSquadronSize = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Squadron")
	float HISMScaleFactor = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Squadron")
	float Spacing = 20.f;

	bool bUsingWorldTransformForHISM = true;

	// BEGIN EXPERIMENTAL
protected:
	UPROPERTY(EditAnywhere, Category = "Boids")
	float CohesionWeight;

	UPROPERTY(EditAnywhere, Category = "Boids")
	float AlignmentWeight;

	UPROPERTY(EditAnywhere, Category = "Boids")
	float SeparationWeight;

	UPROPERTY(EditAnywhere, Category = "Boids")
	float MaxSpeed;

	UPROPERTY(EditAnywhere, Category = "Boids")
	float NeighborRadius;

	FVector NewLocation;

	bool CalculateBoidsParameters(int32 InstanceIndex, const FVector& InstanceLocation, FVector& Cohesion, FVector& Alignment, FVector& Separation);
	// END EXPERIMENTAL

};
