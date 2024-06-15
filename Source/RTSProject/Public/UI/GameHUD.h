#pragma once

#include "EngineUtils.h"
#include "Actors/Units/Ship.h"
#include "GameFramework/HUD.h"
#include "Kismet/KismetSystemLibrary.h"

#include "GameHUD.generated.h"

class UMouseCursorWidget;
class USelectionRectangleWidget;
class ARTSPlayerController;
class UMiniMapWidget;

UCLASS()
class RTSPROJECT_API AGameHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetMinimapWidget(UMiniMapWidget* InMinimapWidget) { MiniMapWidget = InMinimapWidget; }
	UMiniMapWidget* GetMiniMapWidget() const { return MiniMapWidget; }

	UFUNCTION(BlueprintCallable)
	void SetSelectionRectangleWidget(USelectionRectangleWidget* InSelectionRectangleWidget) { SelectionRectangleWidget = InSelectionRectangleWidget; }
	USelectionRectangleWidget* GetSelectionRectangleWidget() const { return SelectionRectangleWidget; }

	UFUNCTION(BlueprintCallable)
	UMouseCursorWidget* GetMouseCursorWidget() const { return MouseCursorWidget; }

protected:
	virtual void BeginPlay() override;

	template<class T>
	bool SetupWidget(T*& WidgetReference, TSubclassOf<T>& WidgetTemplate);
	
	UPROPERTY(BlueprintReadOnly)
	ARTSPlayerController* PlayerController {nullptr};
	
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UMiniMapWidget> MiniMapWidgetClass;

	UPROPERTY(BlueprintReadOnly)
	UMiniMapWidget* MiniMapWidget {nullptr};
	
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<USelectionRectangleWidget> SelectionRectangleWidgetClass;
	
	UPROPERTY(BlueprintReadOnly)
	USelectionRectangleWidget* SelectionRectangleWidget {nullptr};

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	UMouseCursorWidget* MouseCursorWidget {nullptr};

public:
	void LockSelectionRectangle() const;
	void UnlockSelectionRectangle() const;
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