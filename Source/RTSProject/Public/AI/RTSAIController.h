#pragma once

#include "AIController.h"
#include "Orders/OrdersProcessor.h"

#include "RTSAIController.generated.h"

class ARTSPlayerState;

struct RTSPROJECT_API FRTSBlackboardKeys
{
	static const FName SelfActor;
	static const FName EnemyTarget;
	static const FName MovementTargetLocation; // Location where we want to move
	static const FName bMovementTargetLocation; // Flag for above
	static const FName Order;
	static const FName ResourceSource;
};

UCLASS()
class RTSPROJECT_API ARTSAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override; 
	
	UFUNCTION(BlueprintCallable)
	void ChangeOrder(EOrderType InOrder) const;

	UFUNCTION(BlueprintCallable)
	void ClearAllBlackboardKeys() const;
	
	UFUNCTION(BlueprintCallable)
	void RequestMoveToLocation(const FVector& InLocation);

	// Getters for Blackboard keys - Begin
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FName& BlackboardKey_SelfActor() { return FRTSBlackboardKeys::SelfActor; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FName& BlackboardKey_EnemyTarget() { return FRTSBlackboardKeys::EnemyTarget; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FName& BlackboardKey_MovementTargetLocation() { return FRTSBlackboardKeys::MovementTargetLocation; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FName& BlackboardKey_bMovementTargetLocation() { return FRTSBlackboardKeys::bMovementTargetLocation; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FName& BlackboardKey_Order() { return FRTSBlackboardKeys::Order; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FName& BlackboardKey_ResourceSource() { return FRTSBlackboardKeys::ResourceSource; }
	// Getters for Blackboard keys - End

protected:
	virtual void ObtainPlayerState() {};
	
	UPROPERTY()
	ARTSPlayerState* RTSPlayerState { nullptr };
};
