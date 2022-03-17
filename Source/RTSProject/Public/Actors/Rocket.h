#pragma once

#include "Interfaces/Shootable.h"
#include "GameFramework/Actor.h"
#include "Rocket.generated.h"

class UStaticMeshComponent;
class ATurret;

UCLASS()
class RTSPROJECT_API ARocket : public AActor, public IShootable
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	ATurret* OwnerTurret = nullptr;

	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxLifeTime = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 300;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 25;

public:
	
	ARocket();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void DealDamage(AActor* actor);
	virtual void DealDamage_Implementation(AActor* actor) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
								class UPrimitiveComponent* OtherComp,
								int32 OtherBodyIndex, bool bFromSweep,
								const FHitResult& SweepResult);
	virtual void OnOverlapBegin_Implementation(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
												class UPrimitiveComponent* OtherComp,
												int32 OtherBodyIndex, bool bFromSweep,
												const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
							class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	virtual void OnOverlapEnd_Implementation(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
												class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};