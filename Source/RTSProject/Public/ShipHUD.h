#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShipHUD.generated.h"

class ARTSPlayerController;

UCLASS()
class RTSPROJECT_API UShipHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Base")
	ARTSPlayerController* PlayerController = nullptr;

public:
	
	virtual void NativeConstruct() override;
};
