#include "Actors/StarResource.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

AStarResource::AStarResource()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(GetRootComponent());
	SelectionComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionComponent"));
	SelectionComponent->SetupAttachment(GetRootComponent());
	SelectionComponent->SetVisibility(false);
	PrimaryActorTick.bCanEverTick = true;
	ResourceType = EResourceType::Star;
}

void AStarResource::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AStarResource::OnOverlapBegan);
}

void AStarResource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Rotate asteroid
	//const FRotator Rotation = StaticMeshComponent->GetComponentRotation();
	//SetActorRotation(FRotator(Rotation.Pitch, Rotation.Yaw + (RotationSpeed * DeltaTime), Rotation.Roll), ETeleportType::None);

}

void AStarResource::OnOverlapBegan(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}
	// deal damage
}

void AStarResource::Selected_Implementation(bool bIsSelected)
{
	// can't be selected
}

void AStarResource::Highlighted_Implementation(bool bIsHighlighted)
{
	SelectionComponent->SetVisibility(bIsHighlighted);
}