

#include "Rocket.h"

#include "Components/StaticMeshComponent.h"
#include "RTSPlayerController.h"
#include "Ship.h"
#include "Turret.h"
#include "DamageDealer.h"

// Sets default values
ARocket::ARocket()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());

	StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &ARocket::OnOverlapBegin);
	StaticMesh->OnComponentEndOverlap.AddDynamic(this, &ARocket::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ARocket::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<ARTSPlayerController>(GetWorld()->GetFirstPlayerController());
}

// Called every frame
void ARocket::Tick(float mainDeltaTime)
{
	Super::Tick(mainDeltaTime);
	DeltaTime = mainDeltaTime;
	PastTime += mainDeltaTime;
	if (PastTime > MaxLifeTime) Destroy(false, true);
	
	// Should be in function
	FHitResult hit;
	FVector forward = GetActorForwardVector();
	SetActorLocation(((Speed * DeltaTime * forward) + GetActorLocation()), false, &hit, ETeleportType::None);
}

bool ARocket::Destroy_Implementation(bool bNetForce, bool bShouldModifyLevel)
{
	return Super::Destroy(bNetForce, bShouldModifyLevel);
}

void ARocket::DealDamage_Implementation(AActor* actor)
{
	
}

void ARocket::OnOverlapBegin_Implementation(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
											class UPrimitiveComponent* OtherComp,
											int32 OtherBodyIndex, bool bFromSweep,
											const FHitResult& SweepResult)
{
	if (PastTime < 0.01) return;
	
	AShip* Ship = Cast<AShip>(OtherActor);
	if (!Ship) return;
	if (OwnerTurret->OwnerShip == Ship) return;
	DamageDealer::DealDamage(this, Ship);
	Destroy();
}

void ARocket::OnOverlapEnd_Implementation(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
											class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ARocket::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	ATurret* Turret = Cast<ATurret>(NewOwner);
	if (Turret) OwnerTurret = Turret;
}

