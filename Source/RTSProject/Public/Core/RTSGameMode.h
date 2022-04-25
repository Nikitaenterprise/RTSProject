#pragma once

#include "EngineUtils.h"
#include "GameFramework/GameModeBase.h"
#include "RTSGameMode.generated.h"

class AFogOfWar;
class AFogOfWarBoundsVolume;

UCLASS()
class RTSPROJECT_API ARTSGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, NoClear, BlueprintReadOnly, Category = Classes)
	TSubclassOf<AFogOfWar> FogOfWarClass;
	UPROPERTY(EditAnywhere, NoClear, BlueprintReadOnly, Category = Classes)
	TSubclassOf<AFogOfWarBoundsVolume> FogOfWarBoundsVolumeClass;

	ARTSGameMode();
	virtual void StartPlay() override;
protected:
	template<class T>
	void FindOnLevel(T*& PointerToWriteTo, UClass* Class) const;
};

template<class T>
void ARTSGameMode::FindOnLevel(T*& PointerToWriteTo, UClass* Class) const
{
	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogTemp, Error, TEXT("World is nullptr in ARTSGameMode::StartPlay()"))
	}
	for (TActorIterator<T> It(GetWorld(), Class); It; ++It)
	{
		if (!IsValid(*It))
		{
			UE_LOG(LogTemp, Error, TEXT("%s is nullptr in ARTSGameMode::StartPlay()"), *Class->GetName())
		}
		else if (IsValid(*It))
		{
			UE_LOG(LogTemp, Display, TEXT("%s is found on level in ARTSGameMode::StartPlay()"), *Class->GetName())
			PointerToWriteTo = *It;
			break;
		}
	}
}