#include "Miscellaneous/FactoriesFunctionLibrary.h"

#include "Actors/AsteroidField.h"
#include "Actors/Building.h"
#include "Core/RTSPlayerController.h"
#include "Actors/Ship.h"
#include "Actors/Turret.h"
#include "Core/FactoryAssets.h"

#include "Chaos/AABB.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Actors/AsteroidResource.h"
#include "Actors/ResourceManager.h"
#include "Actors/Projectile.h"


AShip* UFactoriesFunctionLibrary::NewShip(const UObject* WorldContext, UClass* ClassType, ARTSPlayerController* Controller, const FVector& Location, const FRotator& Rotation)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn ship, GetWorldFromContextObject() returns nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn ship, GetWorldFromContextObject() returns nullptr"));
		return nullptr;
	}
	return NewShip(World, ClassType, Controller, Location, Rotation);
}


AShip* UFactoriesFunctionLibrary::NewShip(UWorld* World, UClass* ClassType, ARTSPlayerController* Controller, const FVector& Location, const FRotator& Rotation)
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

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	Params.Owner = Controller;
	//Some shit happens here. Before calling SpawnActor the Controller is ARTSPlayerController type. After calling SpawnActor
	// the Controller becomes RTSAIController
	// APawn::PossessedBy changes the ownership when setting up AIController
	AShip* SpawnedShip = World->SpawnActor<AShip>(ClassType, FVector(Location.X, Location.Y, 150), Rotation, Params);
	if (!IsValid(SpawnedShip))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn ship"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn ship"));
		return nullptr;
	}
	Controller->AddToPlayersActors(SpawnedShip);
	SpawnedShip->bJustCreated = true;

	return SpawnedShip;
}


void UFactoriesFunctionLibrary::AddTurretsToShip(AShip* Ship)
{
	if (!IsValid(Ship) || Ship->bHasWorkingTurrets)
	{
		UE_LOG(LogTemp, Error, TEXT("Ship is not valid in UFactoriesFunctionLibrary::AddTurretsToShip()"));
		return;
	}
	if(!IsValid(Ship->PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("Ship->PlayerController is not valid in UFactoriesFunctionLibrary::AddTurretsToShip()"));
		return;
	}
	
	const TSubclassOf<ATurret> TurretClass = Ship->PlayerController->GetFactoryAssets()->GetTurretClass(0);
	if (TurretClass)
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

			FVector HitNormal(0, 0, 1);
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

			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			Params.Owner = Ship;

			ATurret* SpawnedTurret = Ship->GetWorld()->SpawnActor<ATurret>(
				TurretClass.Get(),
				SpawnTransform,
				Params);
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
		}
		Ship->bHasWorkingTurrets = true;
	}
}


ABuilding* UFactoriesFunctionLibrary::NewBuilding(const UObject* WorldContext, UClass* ClassType, ARTSPlayerController* Controller, const FVector& Location, const FRotator& Rotation)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn building, GetWorldFromContextObject() returns nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn building, GetWorldFromContextObject() returns nullptr"));
		return nullptr;
	}
	return NewBuilding(World, ClassType, Controller, Location, Rotation);
}


ABuilding* UFactoriesFunctionLibrary::NewBuilding(UWorld* World, UClass* ClassType, ARTSPlayerController* Controller, const FVector& Location, const FRotator& Rotation)
{
	if (!Controller)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn building, ARTSPlayerController is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn building, ARTSPlayerController is nullptr"));
		return nullptr;
	}

	if (!ClassType)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn ship, UClass is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn ship, UClass is nullptr"));
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	Params.Owner = Controller;

	ABuilding* SpawnedBuilding = World->SpawnActor<ABuilding>(
		ClassType,
		Location,
		Rotation,
		Params);
	if (!SpawnedBuilding)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn building"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn ship"));
		return nullptr;
	}
	
	return SpawnedBuilding;
}

AAsteroidResource* UFactoriesFunctionLibrary::NewAsteroid(const UObject* WorldContext, UClass* ClassType, AActor* Owner, const FTransform& Transform)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn asteroid, GetWorldFromContextObject() returns nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn asteroid, GetWorldFromContextObject() returns nullptr"));
		return nullptr;
	}
	return NewAsteroid(World, ClassType, Owner, Transform);
}

AAsteroidResource* UFactoriesFunctionLibrary::NewAsteroid(UWorld* World, UClass* ClassType, AActor* Owner, const FTransform& Transform)
{
	if (!ClassType)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn asteroid, UClass is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn asteroid, UClass is nullptr"));
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	Params.Instigator = nullptr;
	Params.Owner = Owner;

	AAsteroidResource* SpawnedAsteroid = World->SpawnActor<AAsteroidResource>(ClassType, Transform, Params);
	if (!SpawnedAsteroid)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn asteroid"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn asteroid"));
		return nullptr;
	}
	return SpawnedAsteroid;
}

AAsteroidField* UFactoriesFunctionLibrary::NewAsteroidField(const UObject* WorldContext, UClass* ClassType, ARTSPlayerController* Controller, const FVector& Location, const FRotator& Rotation)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn asteroid field, GetWorldFromContextObject() returns nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn asteroid field, GetWorldFromContextObject() returns nullptr"));
		return nullptr;
	}
	return NewAsteroidField(World, ClassType, Controller, Location, Rotation);
}

AAsteroidField* UFactoriesFunctionLibrary::NewAsteroidField(UWorld* World, UClass* ClassType, ARTSPlayerController* Controller, const FVector& Location, const FRotator& Rotation)
{
	if (!Controller)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn asteroid field, ARTSPlayerController is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn asteroid field, ARTSPlayerController is nullptr"));
		return nullptr;
	}

	if (!ClassType)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn asteroid field, UClass is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn asteroid field, UClass is nullptr"));
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Instigator = nullptr;
	Params.Owner = Controller;

	AAsteroidField* SpawnedAsteroidField = World->SpawnActor<AAsteroidField>(
		ClassType,
		Location, 
		Rotation,
		Params);
	if (!SpawnedAsteroidField)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn asteroid field"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn asteroid field"));
		return nullptr;
	}
	
	SpawnedAsteroidField->AddRandomNumberOfAsteroidsToField();
	const auto ResourceManager = Cast<AResourceManager>(UGameplayStatics::GetActorOfClass(World, AResourceManager::StaticClass()));
	if (ResourceManager)
	{
		ResourceManager->AddAsteroidField(SpawnedAsteroidField);
	}
	return SpawnedAsteroidField;
}

AProjectile* UFactoriesFunctionLibrary::NewRocket(const UObject* WorldContext, UClass* ClassType, ARTSPlayerController* Controller, ATurret* Turret, const FVector& Location, const FRotator& Rotation)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn rocket, GetWorldFromContextObject() returns nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn rocket, GetWorldFromContextObject() returns nullptr"));
		return nullptr;
	}
	return NewRocket(World, ClassType, Controller, Turret, Location, Rotation);
}

AProjectile* UFactoriesFunctionLibrary::NewRocket(UWorld* World, UClass* ClassType, ARTSPlayerController* Controller, ATurret* Turret, const FVector& Location, const FRotator& Rotation)
{
	if (!Controller)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn rocket, ARTSPlayerController is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn rocket, ARTSPlayerController is nullptr"));
		return nullptr;
	}

	if (!ClassType)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn rocket, UClass is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn rocket, UClass is nullptr"));
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Instigator = nullptr;
	Params.Owner = Turret;

	AProjectile* SpawnedRocket = World->SpawnActor<AProjectile>(
		ClassType,
		Location,
		Rotation,
		Params);
	if (!SpawnedRocket)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn rocket"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn rocket"));
		return nullptr;
	}
	return SpawnedRocket;
}