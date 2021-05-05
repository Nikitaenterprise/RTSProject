#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BasicButtonsHUD.generated.h"

class ARTSPlayerController;

UCLASS()
class RTSPROJECT_API UBasicButtonsHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Base")
	ARTSPlayerController* PlayerController = nullptr;

public:
	
	//UBasicButtonsHUD(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SpawnShip();
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SpawnAsteroidField();
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SpawnBuilding();
};
