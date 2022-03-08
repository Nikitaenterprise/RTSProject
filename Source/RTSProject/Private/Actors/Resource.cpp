#include "Actors/Resource.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GAS/ResourceSourceAttributeSet.h"

AResource::AResource()
{
 	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());
	ResourceSourceAttributeSet = CreateDefaultSubobject<UResourceSourceAttributeSet>(TEXT("ResourceSourceAttributeSet"));

	ResourceType = EResourceType::None;
}

float AResource::Consume(float Amount)
{
	auto CurrentCapacity = ResourceSourceAttributeSet->GetResourceCapacity();
	if (Amount < CurrentCapacity)
	{
		ResourceSourceAttributeSet->SetResourceCapacity(CurrentCapacity-Amount);
		return Amount;
	}
	else
	{
		MarkPendingKill();
		return CurrentCapacity;
	}
}

void AResource::BeginPlay()
{
	Super::BeginPlay();
	
	// Set resource amount
	int SquaredSizeOfMesh = UKismetMathLibrary::VSizeSquared(GetActorScale3D());
	if (UKismetMathLibrary::Round(SquaredSizeOfMesh) != 0)
	{
		auto NewAmount = UKismetMathLibrary::FTrunc(SquaredSizeOfMesh * ResourceSourceAttributeSet->GetInitResourceCapacity());
		ResourceSourceAttributeSet->SetResourceCapacity(NewAmount);
	}
}
