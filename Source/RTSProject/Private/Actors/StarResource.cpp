#include "Actors/StarResource.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

AStarResource::AStarResource()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(GetRootComponent());
	SelectionComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionComponent"));
	SelectionComponent->SetupAttachment(GetRootComponent());
	SelectionComponent->SetVisibility(false);
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ResourceComponent = CreateDefaultSubobject<UResourceComponent>(TEXT("ResourceComponent"));
	ResourceComponent->SetResourceType(EResourceType::Star);
	PrimaryActorTick.bCanEverTick = true;
}

void AStarResource::BeginPlay()
{
	Super::BeginPlay();

	ResourceComponent->InitializeCapacity([&]()->float
	{
		return 1000000.0;
	});
	
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