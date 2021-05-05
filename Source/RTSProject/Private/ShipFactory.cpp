#include "ShipFactory.h"

#include "RTSPlayerController.h"
#include "Ship.h"
#include "Turret.h"
#include "FactoryAssets.h"
#include "AnglesFunctions.h"


AShip* ShipFactory::NewShip(UWorld* World, UClass* ClassType, ARTSPlayerController* Controller, const FVector& Location, const FRotator& Rotation)
{
	if (!Controller) return nullptr;
	if (!ClassType) return nullptr;
	
	AShip* SpawnedShip = World->SpawnActor<AShip>(
							ClassType,
					FVector(Location.X, Location.Y, 150), 
							Rotation, 
							GetDefaultSpawnParams());
	if (!SpawnedShip) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn ship"));
		return nullptr;
	}
	Controller->PlayersActors.AddUnique(SpawnedShip);
	SpawnedShip->Initialize(Controller);
	SpawnedShip->bJustCreated = true;
	
	return SpawnedShip;
}

void ShipFactory::AddTurretsToShip(AShip* Ship)
{
	if (!Ship) return;
	if (Ship->bHasWorkingTurrets) return; 
	const TSubclassOf<ATurret> TurretClass = Ship->PlayerController->GetFactoryAssets()->TurretClass;
	if(TurretClass)
	{
		UStaticMeshComponent* StaticMesh = Ship->StaticMesh;
		for (auto& socket : StaticMesh->GetAllSocketNames())
		{
			FTransform SpawnTransform = StaticMesh->GetSocketTransform(socket);
			FActorSpawnParameters Params = GetDefaultSpawnParams();
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ATurret* SpawnedTurret = Ship->GetWorld()->SpawnActor<ATurret>(TurretClass.Get(), SpawnTransform, Params);
			if(!SpawnedTurret) 
			{
				GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn turret"));
				return;
			}
			SpawnedTurret->OwnerShip = Ship;
			SpawnedTurret->OwnerShip->Turrets.AddUnique(SpawnedTurret);
			SpawnedTurret->PlayerController = Ship->PlayerController;
			SetTurretSide(SpawnedTurret);
			SpawnedTurret->SetFacingLeftRight();
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget,
														EAttachmentRule::KeepWorld,
														EAttachmentRule::KeepRelative,
														true);

			SpawnedTurret->AttachToComponent(StaticMesh, AttachmentRules, socket);
			SpawnedTurret->OwnerShip->bHasWorkingTurrets = true;
		}
	}
}

FActorSpawnParameters ShipFactory::GetDefaultSpawnParams()
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	Params.Instigator = nullptr;
	Params.Owner = nullptr;
	return Params;
}


// Decide on which side the turret is
void ShipFactory::SetTurretSide(ATurret* Turret)
{
	const FVector ShipForward = Turret->OwnerShip->GetActorForwardVector();
	const FVector FromCenterOfShipToTurret = Turret->GetActorLocation() - Turret->OwnerShip->GetActorLocation();
	const bool bClockwise = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(ShipForward, FromCenterOfShipToTurret);

	if (bClockwise) Turret->OnWhichSide = ESide::Right;
	if (!bClockwise) Turret->OnWhichSide = ESide::Left;
}
