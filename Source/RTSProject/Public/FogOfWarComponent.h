#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FogOfWarComponent.generated.h"

class AFogOfWar;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTSPROJECT_API UFogOfWarComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:

	/*Select the FOW Manager*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar")
	AFogOfWar* Manager = nullptr;
	/*Is the actor able to influence unfogged texels*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar")
	bool bCanWriteUnFog = true;
	/*Is the actor able to influence fog of war texels*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar")
	bool bCanWriteFow = true;
	/*Is the actor able to influence terra incognita texels*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar")
	bool bCanWriteTerraIncog = true;
	/*Check if the actor is in terra incognita*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar")
	bool bCheckActorTerraIncog = false;
	/**/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FogOfWar")
	bool isActorInTerraIncog = false;
	
public:

	UFogOfWarComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:

	virtual void BeginPlay() override;

};
