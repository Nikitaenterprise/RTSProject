#include "Rocket.h"

#include "Components/StaticMeshComponent.h"
#include "RTSPlayerController.h"
#include "Ship.h"
#include "Turret.h"
#include "DamageDealer.h"

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

void ARocket::BeginPlay()
{
	Super::BeginPlay();
	
	// Binding Destroy() function that will destroy rocket
	// when timer hits MaxLifeTime
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("Destroy"), false, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, MaxLifeTime, false);
}

void ARocket::Initialize(ARTSPlayerController* RTSController, ATurret* Turret)
{
	if (RTSController)
	{
		PlayerController = RTSController;
	}
	if (Turret)
	{
		OwnerTurret = Turret;
	}
}

void ARocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FVector dR = Speed * DeltaTime * GetActorForwardVector();
	SetActorLocation(dR + GetActorLocation(), false, nullptr, ETeleportType::None);
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
	AShip* Ship = Cast<AShip>(OtherActor);
	if (!Ship) return;
	if (OwnerTurret->OwnerShip == Ship) return;
	DamageDealer::DealDamage(this, Ship);
	Destroy(false, false);
}

void ARocket::OnOverlapEnd_Implementation(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
											class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
