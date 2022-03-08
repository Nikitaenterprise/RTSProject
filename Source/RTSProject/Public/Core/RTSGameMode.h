#pragma once

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

public:

	ARTSGameMode();
	virtual void StartPlay() override;

};
