#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildingHUD.generated.h"


UCLASS()
class RTSPROJECT_API UBuildingHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "HUD")
	void ShowUI();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "HUD")
	void HideUI();
};
