#pragma once

#include "EngineUtils.h"
#include "GameFramework/GameModeBase.h"
#include "RTSGameMode.generated.h"

class AResourceManager;
class AFogOfWar;
class AFogOfWarBoundsVolume;
class UFactoryAssets;

UCLASS()
class RTSPROJECT_API ARTSGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ARTSGameMode();
	virtual void StartPlay() override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category="Game", meta=(WorldContext="WorldContextObject"))
	static ARTSGameMode* GetRTSGameMode(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Getters")
	UFactoryAssets* GetFactoryAssets() const { return FactoryAssets; }

	UFUNCTION(BlueprintCallable, Category = "Getters")
	AResourceManager* GetResourceManager() const { return ResourceManager; }
	
protected:
	template<class T>
	bool FindOnLevel(T*& PointerToWriteTo, UClass* Class) const;

	UPROPERTY(EditAnywhere, NoClear, BlueprintReadOnly, Category = Classes)
	TSubclassOf<AFogOfWar> FogOfWarClass;
	
	UPROPERTY(EditAnywhere, NoClear, BlueprintReadOnly, Category = Classes)
	TSubclassOf<AFogOfWarBoundsVolume> FogOfWarBoundsVolumeClass;

	UPROPERTY(EditAnywhere, NoClear, BlueprintReadOnly, Category = Assets)
	UFactoryAssets* FactoryAssets {nullptr};

	UPROPERTY()
	AResourceManager* ResourceManager {nullptr};
};

template<class T>
bool ARTSGameMode::FindOnLevel(T*& PointerToWriteTo, UClass* Class) const
{
	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogTemp, Error, TEXT("World is nullptr in ARTSGameMode::FindOnLevel()"))
		return false;
	}
	for (TActorIterator<T> It(GetWorld(), Class); It; ++It)
	{
		if (!IsValid(*It))
		{
			UE_LOG(LogTemp, Error, TEXT("%s is nullptr in ARTSGameMode::FindOnLevel()"), *Class->GetName())
			return false;
		}
		UE_LOG(LogTemp, Display, TEXT("%s is found on level in ARTSGameMode::FindOnLevel()"), *Class->GetName())
		PointerToWriteTo = *It;
		return true;
	}
	return false;
}