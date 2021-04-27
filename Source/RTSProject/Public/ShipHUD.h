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

	ARTSPlayerController* PlayerController = nullptr;

public:
	
	virtual void NativeConstruct() override;
};
