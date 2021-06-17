#pragma once

#include "Shootable.h"
#include "BaseBehavior.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rocket.generated.h"

class UStaticMeshComponent;
class ARTSPlayerController;
class ATurret;

UCLASS()
class RTSPROJECT_API ARocket : public AActor, public IShootable, public IBaseBehavior
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	ARTSPlayerController* PlayerController = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	ATurret* OwnerTurret = nullptr;

	float DeltaTime = 0;
	float PastTime = 0;
	float MaxLifeTime = 0;
	float Speed = 300;
	float Damage = 25;

public:	
	ARocket();

	virtual void Tick(float mainDeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	bool Destroy(bool bNetForce = false, bool bShouldModifyLevel = false);

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
												const FHitResult& SweepResult) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
							class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	virtual void OnOverlapEnd_Implementation(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
												class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	

	void SetOwner(AActor* NewOwner) override;
	

protected:
	
	virtual void BeginPlay() override;
};
