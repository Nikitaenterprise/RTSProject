#include "Resource.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

AResource::AResource()
{
 	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());
}

void AResource::BeginPlay()
{
	Super::BeginPlay();
	
	// Set resource amount
	int SquaredSizeOfMesh = UKismetMathLibrary::VSizeSquared(GetActorScale3D());
	if (UKismetMathLibrary::Round(SquaredSizeOfMesh) != 0)
	{
		ResourceAmount = UKismetMathLibrary::FTrunc(SquaredSizeOfMesh * ResourceAmount);
	}
}

void AResource::Tick(float mainDeltaTime)
{
	Super::Tick(mainDeltaTime);
	DeltaTime = mainDeltaTime;
	PastTime += mainDeltaTime;


}

