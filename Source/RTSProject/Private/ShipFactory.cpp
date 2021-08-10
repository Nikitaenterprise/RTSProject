#include "ShipFactory.h"

#include "RTSPlayerController.h"
#include "Ship.h"
#include "Turret.h"
#include "FactoryAssets.h"

#include "Chaos/AABB.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"

AShip* ShipFactory::NewShip(UWorld* World, UClass* ClassType, ARTSPlayerController* Controller, const FVector& Location, const FRotator& Rotation)
{
	if (!Controller)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn ship, ARTSPlayerController is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn ship, ARTSPlayerController is nullptr"));
		return nullptr;
	}
	if (!ClassType)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn ship, UClass is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn ship, UClass is nullptr"));
		return nullptr;
	}
	AShip* SpawnedShip = World->SpawnActor<AShip>(
							ClassType,
					FVector(Location.X, Location.Y, 150), 
							Rotation, 
							GetDefaultSpawnParams());
	if (!SpawnedShip) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn ship"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn ship"));
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
	const TSubclassOf<ATurret> TurretClass = Ship->PlayerController->GetFactoryAssets()->GetTurretClass(0);
	if(TurretClass)
	{
		UStaticMeshComponent* StaticMesh = Ship->StaticMesh;

		for (auto& Socket : StaticMesh->GetAllSocketNames())
		{
			FTransform SocketTransform = StaticMesh->GetSocketTransform(Socket);

			FHitResult Hit;
			FCollisionObjectQueryParams OQParams;
			// Set collision to hit ship mesh which is WorldDynamic
			// Capsule component is PawnCapsule, so it won't be hit
			OQParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
			FCollisionQueryParams QParams = QParams.DefaultQueryParam;
			// Trace complex mesh geometry
			QParams.bTraceComplex = true;
			
			bool bHit = Ship->GetWorld()->LineTraceSingleByObjectType(Hit, SocketTransform.GetLocation() + FVector(0, 0, 10), SocketTransform.GetLocation() - FVector(0, 0, 10), OQParams, QParams);

			FVector HitNormal (0,0,1);
			if (bHit) HitNormal = Hit.ImpactNormal;
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("Finding normal for turret placing failed"));
				UE_LOG(LogTemp, Error, TEXT("Finding normal for turret placing failed"));
			}
			
			// Find rotation for turret to be aligned with surface normal
			FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(FVector(0, 0, 1), HitNormal);
			// Set spawn transform for turret at socket position but with rotation that will align turret up vector with surface normal
			FTransform SpawnTransform = FTransform(Rotation, SocketTransform.GetTranslation(), SocketTransform.GetScale3D());

			FActorSpawnParameters Params = GetDefaultSpawnParams();
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ATurret* SpawnedTurret = Ship->GetWorld()->SpawnActor<ATurret>(TurretClass.Get(), SpawnTransform, Params);
			if (!SpawnedTurret)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn turret"));
				UE_LOG(LogTemp, Error, TEXT("Failed to spawn turret"));
				return;
			}
			
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget,
														EAttachmentRule::KeepWorld,
														EAttachmentRule::KeepRelative,
														true);
			SpawnedTurret->AttachToComponent(StaticMesh, AttachmentRules, Socket);
			SpawnedTurret->Initialize(Ship->PlayerController, Ship);
			Ship->Turrets.AddUnique(SpawnedTurret);
		}
		Ship->bHasWorkingTurrets = true;
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

