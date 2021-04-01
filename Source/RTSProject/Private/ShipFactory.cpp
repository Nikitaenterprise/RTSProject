#include "ShipFactory.h"

#include "RTSPlayerController.h"
#include "Ship.h"
#include "Turret.h"
#include "FactoryAssets.h"
#include "AnglesFunctions.h"

//#include "Engine/World.h"
//#include "Engine/StaticMesh.h"


AShip* ShipFactory::NewShip(UWorld* _World, ARTSPlayerController* _Controller)
{
	return NewShip(_World, FVector(0, 0, 0), FRotator(0, 0, 0), _Controller);
}

AShip* ShipFactory::NewShip(UWorld* _World, const FVector& _Location, ARTSPlayerController* _Controller)
{
	return NewShip(_World, _Location, FRotator(0, 0, 0), _Controller);
}

AShip* ShipFactory::NewShip(UWorld* _World, const FVector& _Location, const FRotator& _Rotation, ARTSPlayerController* _Controller)
{
	if (!_Controller) return nullptr;
	
	const TSubclassOf<AShip> ShipClass = _Controller->GetFactoryAssets()->ShipClass;
	if (!ShipClass) return nullptr;
	
	AShip* SpawnedShip = _World->SpawnActor<AShip>(
						ShipClass.Get(), 
						_Location, 
						_Rotation, 
						GetDefaultSpawnParams());
	if (!SpawnedShip) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn ship"));
		return nullptr;
	}
	_Controller->PlayersActors.AddUnique(SpawnedShip);
	AddTurretsToShip(_World, _Controller, SpawnedShip);
	SpawnedShip->Initialize(_Controller);
	
	return SpawnedShip;
}

FActorSpawnParameters ShipFactory::GetDefaultSpawnParams()
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	Params.Instigator = nullptr;
	Params.Owner = nullptr;
	return Params;
}

void ShipFactory::AddTurretsToShip(UWorld* _World, ARTSPlayerController* _Controller, AShip* _Ship)
{
	if (_Ship->bHasWorkingTurrets) return;
	const TSubclassOf<ATurret> TurretClass = _Controller->GetFactoryAssets()->TurretClass;
	if(TurretClass)
	{
		UStaticMeshComponent* StaticMesh = _Ship->StaticMesh;
		for (auto& socket : StaticMesh->GetAllSocketNames())
		{
			FTransform SpawnTransform = StaticMesh->GetSocketTransform(socket);
			FActorSpawnParameters params = GetDefaultSpawnParams();
			params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ATurret* SpawnedTurret = _World->SpawnActor<ATurret>(TurretClass.Get(), SpawnTransform, params);
			if(!SpawnedTurret) 
			{
				GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn turret"));
				return;
			}
			SpawnedTurret->OwnerShip = _Ship;
			SpawnedTurret->OwnerShip->Turrets.AddUnique(SpawnedTurret);
			SpawnedTurret->PlayerController = _Controller;
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

// Decide on which side the turret is
void ShipFactory::SetTurretSide(ATurret* _Turret)
{
	const FVector ShipForward = _Turret->OwnerShip->GetActorForwardVector();
	// Probably :
	// Turret->GetActorLocation() - Turret->OwnerShip->GetActorLocation()
	const FVector FromCenterOfShipToTurret = (_Turret->OwnerShip->GetActorLocation() - _Turret->GetActorLocation()) * -1;
	const bool bClockwise = AnglesFunctions::FindRotationDirectionBetweenVectorsOn2D(ShipForward, FromCenterOfShipToTurret);

	if (bClockwise) _Turret->OnWhichSide = ESide::Right;
	if (!bClockwise) _Turret->OnWhichSide = ESide::Left;
}
