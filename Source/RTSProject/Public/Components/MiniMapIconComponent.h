#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MiniMapIconComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTSPROJECT_API UMiniMapIconComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	UTexture2D* MiniMapIcon = nullptr;
public:	
	
	UMiniMapIconComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	virtual void BeginPlay() override;
};
