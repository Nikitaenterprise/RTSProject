#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildingHUD.generated.h"

class ABuilding;
class ARTSPlayerController;

UCLASS()
class RTSPROJECT_API UBuildingHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category="Base")
	ARTSPlayerController* PlayerController = nullptr;

public:

	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable)
	TArray<ABuilding*> GetSelectedBuildings();
	UFUNCTION(BlueprintCallable)
	void AddToBuildingQueue(ABuilding* BuildingToAddTo, const TSubclassOf<AActor>& ActorTypeToSpawn);
	UFUNCTION(BlueprintCallable)
	int GetNumberOfQueueUnitsInBuilding(const ABuilding* BuildingToGetFrom, TSubclassOf<AActor> ActorClass);
};
