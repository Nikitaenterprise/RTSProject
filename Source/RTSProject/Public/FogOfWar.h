#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FogOfWar.generated.h"

class FFogOfWarThread;

//https://forums.unrealengine.com/t/tutorial-fog-of-war/18222

UCLASS()
class RTSPROJECT_API AFogOfWar : public AActor
{
	GENERATED_BODY()

public:
	
	//How far will an actor be able to see
	//CONSIDER: Place it on the actors to allow for individual sight-radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
	float SightRange = 9.0f;

	//The number of samples per 100 unreal units
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
	float SamplesPerMeter = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
	//FColor ColorOne = FColor((uint8)255, (uint8)255, (uint8)255, 255);
	uint8 UnfogColor = static_cast<uint8>(255);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
	uint8 FowMaskColor = static_cast<uint8>(100);

	UPROPERTY(EditAnywhere, Category = FogOfWar)
	bool bUseTextureFile = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
	UTexture2D* TextureInFile = nullptr;
	// Set this variable to floor in level so map size can be known
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AActor* MapFloor = nullptr;
	//If the last texture blending is done
	UPROPERTY(BlueprintReadWrite)
	bool bIsDoneBlending;

	//Should we blur? It takes up quite a lot of CPU time...
	UPROPERTY(EditAnywhere)
	bool bIsBlurEnabled = true;

	//The size of our textures
	uint32 TextureSize = 1024;
	//The size of our textures (parameter for UE4 editor)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int TextureSizeForPropertyWindow = TextureSize;

	//Array containing what parts of the map we've unveiled.
	UPROPERTY()
	TArray<bool> UnfoggedData;

	//Temp array for horizontal blur pass
	UPROPERTY()
	TArray<uint8> HorizontalBlurData;

	//Our texture data (result of vertical blur pass)
	UPROPERTY()
	TArray<FColor> TextureData;

	//Our texture data from the last frame
	UPROPERTY()
	TArray<FColor> LastFrameTextureData;

	UPROPERTY()
	TArray<FColor> TextureInFileData;

	//Time Array
	UPROPERTY()
	TArray<float> FOWTimeArray;

	UPROPERTY()
	TArray<bool> FOWArray;

	UPROPERTY(EditAnywhere)
	bool bIsFowTimerEnabled = false;

	UPROPERTY(EditAnywhere)
	float FowTimeLimit = 10.0f;

	//Check to see if we have a new FOW-texture.
	bool bHasFOWTextureUpdate = false;

	//Blur size
	uint8 blurKernelSize = 15;

	//Blur kernel
	UPROPERTY()
	TArray<float> blurKernel;

	//Store the actors that will be unveiling the FOW-texture.
	UPROPERTY()
	TArray<AActor*> FowActors;

	//DEBUG: Time it took to update the fow texture
	float fowUpdateTime = 0;

private:

	//Our dynamically updated texture
	UPROPERTY()
	UTexture2D* FOWTexture;

	//Texture from last update. We blend between the two to do a smooth unveiling of newly discovered areas.
	UPROPERTY()
	UTexture2D* LastFOWTexture;

	//Texture regions
	FUpdateTextureRegion2D* textureRegions;

	//Our fowupdatethread
	FFogOfWarThread* FowThread;

	//This is for accessing the actor component "RegisterToFow_BP"
	UActorComponent* ActorComp = nullptr;

public:	
	
	AFogOfWar();
	~AFogOfWar();
	virtual void Tick(float DeltaTime) override;
	//Triggers a update in the blueprint
	UFUNCTION(BlueprintNativeEvent)
	void OnFowTextureUpdated(UTexture2D* currentTexture, UTexture2D* lastTexture);
	UFUNCTION(BlueprintCallable, Category = FogOfWar)
	void debugTextureAccess();
	//Register an actor to influence the FOW-texture
	UFUNCTION(BlueprintCallable, Category = FogOfWar)
	void RegisterFowActor(AActor* Actor);

	//Stolen from https://wiki.unrealengine.com/Dynamic_Textures
	void UpdateTextureRegions(
		UTexture2D* Texture,
		int32 MipIndex,
		uint32 NumRegions,
		FUpdateTextureRegion2D* Regions,
		uint32 SrcPitch,
		uint32 SrcBpp,
		uint8* SrcData,
		bool bFreeData);

	//Getter for the working thread
	bool GetIsBlurEnabled() const;
	//Getter for the working thread
	bool GetIsTextureFileEnabled() const;
	//Temp method for logging an actor components names
	UFUNCTION(BlueprintCallable, Category = FogOfWar)
	void LogNames();


protected:
	
	virtual void BeginPlay() override;


private:

	//void UpdateFowTexture();

	//Triggers the start of a new FOW-texture-update
	void StartFOWTextureUpdate();
};
