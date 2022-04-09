#pragma once

#include "EngineUtils.h"
#include "Actors/Ship.h"
#include "GameFramework/HUD.h"
#include "Kismet/KismetSystemLibrary.h"

#include "GameHUD.generated.h"

class USelectionRectangleWidget;
class ARTSPlayerController;
class UMiniMapWidget;

UCLASS()
class RTSPROJECT_API AGameHUD : public AHUD
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ARTSPlayerController* PlayerController = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UMiniMapWidget> MiniMapWidgetClass;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UMiniMapWidget* MiniMapWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<USelectionRectangleWidget> SelectionRectangleWidgetClass;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USelectionRectangleWidget* SelectionRectangleWidget = nullptr;

public:
	virtual void BeginPlay() override;

	void LockSelectionRectangle() const;
	void UnlockSelectionRectangle() const;

	template<class T>
	bool SetupWidget(T*& WidgetReference, TSubclassOf<T>& WidgetTemplate);
};

template<class T>
bool AGameHUD::SetupWidget(T*& WidgetReference, TSubclassOf<T>& WidgetTemplate)
{
	if (WidgetTemplate)	{
		WidgetReference = CreateWidget<T>(GetWorld(), WidgetTemplate);
		if (WidgetReference) return true;
		return false;
	}
	return false;
}