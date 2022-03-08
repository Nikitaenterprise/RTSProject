#pragma once

#include "Components/ActorComponent.h"
#include "MiniMapInfluencerComponent.generated.h"

class ARTSPlayerController;
class UMiniMapWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTSPROJECT_API UMiniMapInfluencerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	UMiniMapWidget* MiniMapWidget = nullptr;

public:

	UMiniMapInfluencerComponent();
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};