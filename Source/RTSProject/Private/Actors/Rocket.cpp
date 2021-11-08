#include "Actors/Rocket.h"

#include "Components/StaticMeshComponent.h"
#include "Core/RTSPlayerController.h"
#include "Actors/Ship.h"
#include "Actors/Turret.h"
#include "Miscellaneous/DamageDealer.h"

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

	ATurret* TestOwner = Cast<ATurret>(GetOwner());
	if (!IsValid(TestOwner))
	{
		UE_LOG(LogTemp, Error, TEXT("TestOwner is nullptr in ARocket::BeginPlay()"));
		return;
	}
	OwnerTurret = TestOwner;

	// Binding Destroy() function that will destroy rocket
	// when timer hits MaxLifeTime
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this]()
		{
			this->Destroy();
		});
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, MaxLifeTime, false);
}

void ARocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FVector dR = Speed * DeltaTime * GetActorForwardVector();
	SetActorLocation(dR + GetActorLocation(), false, nullptr, ETeleportType::None);
}

void ARocket::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
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
	if (!Ship)
	{
		return;
	}
	if (OwnerTurret->OwnerShip == Ship)
	{
		return;
	}
	DamageDealer::DealDamage(this, Ship);
	Destroy();
}

void ARocket::OnOverlapEnd_Implementation(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
											class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
