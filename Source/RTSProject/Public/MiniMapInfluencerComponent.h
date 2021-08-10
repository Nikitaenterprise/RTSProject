#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MiniMapInfluencerComponent.generated.h"

class ARTSPlayerController;
class UMiniMapHUD;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTSPROJECT_API UMiniMapInfluencerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	UMiniMapHUD* MiniMapHUD = nullptr;

public:

	UMiniMapInfluencerComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Initialize(const ARTSPlayerController* PlayerController);

protected:

	virtual void BeginPlay() override;

};