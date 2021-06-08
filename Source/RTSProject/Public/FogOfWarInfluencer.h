#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FogOfWarInfluencer.generated.h"

class AFogOfWar;
class ARTSPlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTSPROJECT_API UFogOfWarInfluencer : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY()
	AFogOfWar* FOW = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FogOfWar")
	int32 SightRadius = 1000;
	UPROPERTY(BlueprintReadOnly, Category="FogOfWar")
	bool bIsActorInTerraIncognita = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FogOfWar")
	bool bIsActorCanUnveilFogOfWar = true;
	
public:
	
	UFogOfWarInfluencer();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Initialize(ARTSPlayerController* PlayerController);

protected:

	virtual void BeginPlay() override;
			
};
