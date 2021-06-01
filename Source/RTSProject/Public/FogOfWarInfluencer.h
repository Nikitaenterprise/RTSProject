#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FogOfWarInfluencer.generated.h"

class AFogOfWar;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTSPROJECT_API UFogOfWarInfluencer : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY()
	AFogOfWar* FOW = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FogOfWar")
	uint8 SightRadius = 100;
	UPROPERTY(BlueprintReadOnly, Category="FogOfWar")
	bool bIsActorInTerraIncognita = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FogOfWar")
	bool bIsActorCanUnveilFogOfWar = true;
	
public:
	
	UFogOfWarInfluencer();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:

	virtual void BeginPlay() override;
			
};
