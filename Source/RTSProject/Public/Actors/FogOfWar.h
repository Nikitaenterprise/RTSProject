#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FogOfWar.generated.h"

class UFogOfWarInfluencerComponent;
class ARTSPlayerController;
class APostProcessVolume;
class AFogOfWarBoundsVolume;
class UTexture2DDynamic;

UENUM(BlueprintType)
enum class EFOWState : uint8
{
	/** Area has never been visited before. */
	FOW_TERRA_INCOGNITA,
	/** Area has been visited before, but is currently not. */
	FOW_KNOWN,
	/** Area is revealed right now. */
	FOW_VISIBLE
};

UCLASS()
class RTSPROJECT_API AFogOfWar : public AActor
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly)
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
	
	TArray<TPair<AActor*, UFogOfWarInfluencerComponent*>> RegisteredActors;

	uint32 VolumeHeightInCells = 0;
	uint32 VolumeWidthInCells = 0;
	uint32 TextureBufferSize = 0;
	float VolumeHeight = 0;
	float VolumeWidth = 0;
public:	

	AFogOfWar();
	virtual void Tick(float DeltaTime) override;
	void Initialize(ARTSPlayerController* Controller);
	UFUNCTION(BlueprintCallable)
	void RegisterActor(AActor* ActorToRegister);
	UFUNCTION(BlueprintCallable)
	void UnRegisterActor(AActor* ActorToRegister);
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetRegisteredActors() const;
	
	EFOWState GetActorVisionData(uint32 X, uint32 Y);

	void UpdateTextureRegions(UTexture2D* Texture,	int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);

protected:

	virtual void BeginPlay() override;

private:

	
	
};