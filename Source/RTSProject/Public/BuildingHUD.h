#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildingHUD.generated.h"

class ARTSPlayerController;

UCLASS()
class RTSPROJECT_API UBuildingHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:

	ARTSPlayerController* PlayerController = nullptr;

public:

	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable)
	void CheckButton();
};
