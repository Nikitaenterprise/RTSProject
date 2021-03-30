#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BaseBehavior.generated.h"


struct FHitResult;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBaseBehavior : public UInterface
{
	GENERATED_BODY()
};

class RTSPROJECT_API IBaseBehavior
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	bool Destroy(bool bNetForce = false, bool bShouldModifyLevel = false);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Selected(bool bIsSelected);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Highlighted(bool bIsHighlighted);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
								class UPrimitiveComponent* OtherComp,
								int32 OtherBodyIndex, bool bFromSweep,
								const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
								class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
