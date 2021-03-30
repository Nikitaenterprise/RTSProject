#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthShieldBarHUD.generated.h"


UCLASS()
class RTSPROJECT_API UHealthShieldBarHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "HUD")
		void ShowUI();
	virtual void ShowUI_Implementation();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "HUD")
		void HideUI();
	virtual void HideUI_Implementation();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
		float ShieldPercent = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
		float HealthPercent = 0;
};