#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FogOfWar.generated.h"

class ARTSPlayerController;
class APostProcessVolume;
class AFogOfWarBoundsVolume;
class UTexture2DDynamic;

UCLASS()
class RTSPROJECT_API AFogOfWar : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ARTSPlayerController* PlayerController = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	APostProcessVolume* FOWPostProcessVolume = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMaterialInterface* FOWMaterial = nullptr;
	UPROPERTY(BlueprintReadOnly)
	UTexture2D* FOWTexture = nullptr;
	uint8* FOWTextureBuffer = nullptr;
	FUpdateTextureRegion2D* FOWUpdateTextureRegion = nullptr;
	UPROPERTY(BlueprintReadOnly)
	UMaterialInstanceDynamic* FOWMaterialInstance = nullptr;
	UPROPERTY(BlueprintReadOnly)
	UTexture2DDynamic* FOWTextureDynamic = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AFogOfWarBoundsVolume* FOWBoundsVolume = nullptr;
	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> RegistredActors;
	
public:	

	AFogOfWar();
	virtual void Tick(float DeltaTime) override;
	void Initialize(ARTSPlayerController* Controller);
	UFUNCTION(BlueprintCallable)
	void RegisterActor(AActor* ActorToRegister);

	void UpdateTextureRegions(UTexture2D* Texture,	int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);
	
protected:

	virtual void BeginPlay() override;
};
