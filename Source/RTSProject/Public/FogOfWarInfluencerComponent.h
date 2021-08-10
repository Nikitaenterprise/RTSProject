#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FogOfWarInfluencerComponent.generated.h"

class AFogOfWar;
class ARTSPlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTSPROJECT_API UFogOfWarInfluencerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	AFogOfWar* FOW = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar")
	int32 SightRadius = 1000;
	UPROPERTY(BlueprintReadOnly, Category = "FogOfWar")
	bool bIsActorInTerraIncognita = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar")
	bool bIsActorCanUnveilFogOfWar = true;

public:

	UFogOfWarInfluencerComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Initialize(const ARTSPlayerController* PlayerController);

protected:

	virtual void BeginPlay() override;

};
