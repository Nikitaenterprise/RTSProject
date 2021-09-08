#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MiniMapIconComponent.generated.h"


class AFogOfWarBoundsVolume;
class ARTSPlayerController;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTSPROJECT_API UMiniMapIconComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSlateBrush IconBrush;

	UPROPERTY(BlueprintReadOnly)
	AFogOfWarBoundsVolume* FOWBoundsVolume = nullptr;

public:	
	
	UMiniMapIconComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Initialize(const ARTSPlayerController* PlayerController);

protected:

	virtual void BeginPlay() override;
};
