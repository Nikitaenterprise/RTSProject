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
	ABuilding* GetSelectedBuilding();
	UFUNCTION(BlueprintCallable)
	void AddToBuildingQueue(const TSubclassOf<AActor>& ActorTypeToSpawn);
};
