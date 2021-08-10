#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniMapHUD.generated.h"


class AFogOfWarBoundsVolume;
class ARTSPlayerController;

UCLASS()
class RTSPROJECT_API UMiniMapHUD : public UUserWidget
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AFogOfWarBoundsVolume* FOWBoundsVolume = nullptr;
	UPROPERTY()
	TArray<AActor*> MiniMapInfluencers;

public:
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void Initialize(ARTSPlayerController* Controller);
	void RegisterActor(AActor* ActorToRegister);
};
