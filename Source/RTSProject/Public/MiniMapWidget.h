#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY()
	TArray<UTexture2D*> IconsArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* DefaultIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AFogOfWarBoundsVolume* FOWBoundsVolume = nullptr;
	UPROPERTY()
	TArray<AActor*> MiniMapInfluencers;

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void Initialize(ARTSPlayerController* Controller);
	UFUNCTION(BlueprintCallable)
	void RegisterActor(AActor* ActorToRegister);
};
