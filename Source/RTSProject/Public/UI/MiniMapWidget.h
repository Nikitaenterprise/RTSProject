#pragma once

#include "Blueprint/UserWidget.h"
#include "MiniMapWidget.generated.h"


class AFogOfWarBoundsVolume;
class ARTSPlayerController;

UCLASS()
class RTSPROJECT_API UMiniMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	ARTSPlayerController* PlayerController = nullptr;
	UPROPERTY(EditAnywhere)
	UMaterialInterface* MiniMapMaterial = nullptr;
	UPROPERTY(BlueprintReadOnly)
	UMaterialInstanceDynamic* MiniMapMaterialInstance = nullptr;
	UPROPERTY(BlueprintReadOnly)
	UTexture2D* FOWTexture = nullptr;
	uint8* MiniMapTextureBuffer = nullptr;
	FUpdateTextureRegion2D* MiniMapUpdateTextureRegion = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush DefaultIconBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AFogOfWarBoundsVolume* FOWBoundsVolume = nullptr;
	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> MiniMapInfluencers;

	TArray<TPair<AActor*, FSlateBrush&>> Icons;

private:

	bool bLMBPressed;

public:

	UFUNCTION(BlueprintCallable)
	void RegisterActor(AActor* ActorToRegister);
	UFUNCTION(BlueprintCallable)
	void UnRegisterActor(AActor* ActorToUnregister);

private:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual int32 NativePaint(const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void DrawIcons(const FPaintContext& Context) const;
	void DrawCameraTrapezoid(const FPaintContext& Context) const;

	FVector2D WorldToMiniMap(const FVector& Location) const;
	FVector ViewportToWorld(const FVector2D& ViewportPosition) const;

	/// <summary>
	/// Cleans TArray<TPair<AActor*, FSlateBrush&>> Icons and
	/// TArray<AActor*> MiniMapInfluencers arrays from not valid actors
	/// </summary>
	void CleanUpArrays();

};