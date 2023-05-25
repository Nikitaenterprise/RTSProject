#pragma once

#include "BehaviorTree/BTTaskNode.h"

#include "BTTask_ShipMoveTo.generated.h"

UCLASS()
class RTSPROJECT_API UBTTask_ShipMoveTo : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ShipMoveTo(const FObjectInitializer& ObjectInitializer);	
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
	virtual FString GetStaticDescription() const override;	
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	//EBTNodeResult::Type HandleTaskFailure(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, UBlackboardComponent* Blackboard);
	//void HandleTaskFailureAndExit(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	EBlackboardNotificationResult OnBlackboardValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID);

	// Behavior Tree Input:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShipNavigation")
	FBlackboardKeySelector FlightLocationKey;	

	// Optional: Useful in somecases where you want failure or success of a task to automatically update a particular blackboard key
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShipNavigation")
	FBlackboardKeySelector FlightResultKey;

	// Optional: This boolean will be flip-flopped at the end of this task (regardless of success or failure). This can be useful for certain types of behavior tree setups
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShipNavigation")
	FBlackboardKeySelector KeyToFlipFlopWhenTaskExits;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShipNavigation")
	float MinimumProximityRequired = 15.f;
	
	// Recalculate path enable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShipNavigation", meta = (InlineEditConditionToggle))
	uint32 bRecalculatePathOnDestinationChanged : 1;

	// Recalculate path if FlightLocation value changed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShipNavigation", meta = (EditCondition = "bRecalcPathOnDestinationChanged"))
	float RecalculatePathTolerance;

	// UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShipNavigation")
	// FDoNNavigationQueryParams QueryParams;
	
	//UFUNCTION(BlueprintCallable, Category="ShipNavigation")
	//void Pathfinding_OnFinish(const FDoNNavigationQueryData& Data);

	//UFUNCTION(BlueprintCallable, Category="ShipNavigation")
	//void Pathfinding_OnDynamicCollisionAlert(const FDonNavigationDynamicCollisionPayload& Data);	

	//UPROPERTY(BlueprintReadOnly, Category = "ShipNavigation")
	//ADonNavigationManager* NavigationManager;

	/** In some scenarios, it may be desirable to allow the A.I. to teleport to its intended destination  if pathfinding failed for some reason
	*    Set this flag to true to if you want this behavior*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShipNavigation")
	bool bTeleportToDestinationUponFailure = false;

	// Makeshift arrangement until the Task Owner / Task List discrepancy bug is comprehensively conquered
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShipNavigation")
	float MaxTimeBeforeTeleport = 10.f;

protected:

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	//FBT_FlyToTarget* TaskMemoryFromGenericPayload(void* GenericPayload);

	//EBTNodeResult::Type SchedulePathfindingRequest(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);	

	//void AbortPathfindingRequest(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	//void TickPathNavigation(UBehaviorTreeComponent& OwnerComp, FBT_FlyToTarget* MyMemory, float DeltaSeconds);

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	//virtual bool TeleportAndExit(UBehaviorTreeComponent& OwnerComp, bool bWrapUpLatentTask = true);
	
	TMap<TWeakObjectPtr<AActor>, float> LastRequestTimestamps;

	constexpr static float RequestThrottleInterval = 0.25;
};
