#pragma once

#include "Blueprint/UserWidget.h"
#include "BasicUIWidget.generated.h"

class ARTSPlayerController;

UCLASS()
class RTSPROJECT_API UBasicUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ARTSPlayerController* PlayerController = nullptr;


public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	bool IsShipsAreSelected() const;
	UFUNCTION(BlueprintCallable)
	bool IsBuildingsAreSelected() const;

};
