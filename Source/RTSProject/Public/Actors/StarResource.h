#pragma once

#include "Actors/Resource.h"
#include "Interfaces/Selectable.h"
#include "StarResource.generated.h"

class USphereComponent;
class UWidgetComponent;

UCLASS()
class RTSPROJECT_API AStarResource : public AResource, public ISelectable
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent = nullptr;
	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComponent = nullptr;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* SelectionComponent = nullptr;
	UPROPERTY(EditAnywhere)
	float RotationSpeed = 50.0;
	
public:	
	
	AStarResource();
	virtual void Tick(float DeltaTime) override;
	float GetRotationSpeed() const { return RotationSpeed; }
	void SetRotationSpeed(float NewRotationSpeed) { RotationSpeed = NewRotationSpeed; }
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Selected(bool bIsSelected);
	virtual void Selected_Implementation(bool bIsSelected) override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Highlighted(bool bIsHighlighted);
	virtual void Highlighted_Implementation(bool bIsHighlighted) override;
	
protected:
	
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnOverlapBegan(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult);
	
};
