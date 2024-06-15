#include "Miscellaneous/FactoriesFunctionLibrary.h"

#include "Actors/Buildings/Building.h"
#include "Core/RTSPlayerController.h"
#include "Actors/Units/Ship.h"
#include "Actors/Units/Turret.h"
#include "Core/FactoryAssets.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Actors/Projectile.h"
#include "Actors/Units/Fighter.h"
#include "Actors/Units/Squad.h"
#include "Core/RTSGameMode.h"
#include "Systems/RTSPlayerState.h"
#include "Systems/UnitMovement/BoidsMovementSystem.h"


AShip* UFactoriesFunctionLibrary::NewShip(const UObject* WorldContext, UClass* ClassType, AController* PlayerController, const FVector& Location, const FRotator& Rotation)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn ship, GetWorldFromContextObject() returns nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn ship, GetWorldFromContextObject() returns nullptr"));
		return nullptr;
	}
	return NewShip(World, ClassType, PlayerController, Location, Rotation);
}


AShip* UFactoriesFunctionLibrary::NewShip(UWorld* World, UClass* ClassType, AController* PlayerController, const FVector& Location, const FRotator& Rotation)
{
	if (!PlayerController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn ship, AController is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn ship, AController is nullptr"));
		return nullptr;
	}

	if (!ClassType)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn ship, UClass is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn ship, UClass is nullptr"));
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = PlayerController;
	//Some shit happens here. Before calling SpawnActor the Controller is AController type. After calling SpawnActor
	// the Controller becomes RTSAIController
	// APawn::PossessedBy changes the ownership when setting up AIController
	AShip* SpawnedShip = World->SpawnActor<AShip>(ClassType, FVector(Location.X, Location.Y, 150), Rotation, Params);
	if (!IsValid(SpawnedShip))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn ship"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn ship"));
		return nullptr;
	}
	
	if (auto* PlayerState = PlayerController->GetPlayerState<ARTSPlayerState>())
	{
		PlayerState->AddToPlayersUnits(SpawnedShip);
		SpawnedShip->SetGenericTeamId(PlayerState->GetGenericTeamId());
	}

	return SpawnedShip;
}

AFighter* UFactoriesFunctionLibrary::NewFighter(const UObject* WorldContext, UClass* ClassType, AController* PlayerController, ASquad* Squadron, const FVector& Location, const FRotator& Rotation)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn fighter, GetWorldFromContextObject() returns nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn fighter, GetWorldFromContextObject() returns nullptr"));
		return nullptr;
	}
	return NewFighter(World, ClassType, PlayerController, Squadron, Location, Rotation);
}

AFighter* UFactoriesFunctionLibrary::NewFighter(UWorld* World, UClass* ClassType, AController* PlayerController, ASquad* Squadron, const FVector& Location, const FRotator& Rotation)
{
	if (!PlayerController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn fighter, AController is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn fighter, AController is nullptr"));
		return nullptr;
	}

	if (!ClassType)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn fighter, UClass is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn fighter, UClass is nullptr"));
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = PlayerController;
	AFighter* SpawnedFighter = World->SpawnActor<AFighter>(ClassType, FVector(Location.X, Location.Y, 150), Rotation, Params);
	if (!IsValid(SpawnedFighter))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn fighter"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn fighter"));
		return nullptr;
	}

	if (auto* PlayerState = PlayerController->GetPlayerState<ARTSPlayerState>())
	{
		PlayerState->AddToPlayersUnits(SpawnedFighter);
	}
	
	if (IsValid(Squadron))
	{
		Squadron->AddToSquad(SpawnedFighter);
	}

	return SpawnedFighter;
}

ASquad* UFactoriesFunctionLibrary::NewFighterSquadron(const UObject* WorldContext, UClass* ClassType, AController* PlayerController, const FVector& Location, const FRotator& Rotation)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn fighter squadron, GetWorldFromContextObject() returns nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn fighter squadron, GetWorldFromContextObject() returns nullptr"));
		return nullptr;
	}
	return NewFighterSquadron(World, ClassType, PlayerController, Location, Rotation);
}

ASquad* UFactoriesFunctionLibrary::NewFighterSquadron(UWorld* World, UClass* ClassType, AController* PlayerController, const FVector& Location, const FRotator& Rotation)
{
	if (!PlayerController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn fighter, AController is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn fighter squadron, AController is nullptr"));
		return nullptr;
	}

	if (!ClassType)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn fighter, UClass is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn fighter squadron, UClass is nullptr"));
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = PlayerController;
	ASquad* SpawnedSquadron = World->SpawnActor<ASquad>(ClassType, FVector(Location.X, Location.Y, 150), Rotation, Params);
	if (!IsValid(SpawnedSquadron))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn fighter"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn fighter"));
		return nullptr;
	}
	
	if (auto* PlayerState = PlayerController->GetPlayerState<ARTSPlayerState>())
	{
		PlayerState->AddToPlayersUnits(SpawnedSquadron);
	}

	return SpawnedSquadron;
}


void UFactoriesFunctionLibrary::AddTurretsToShip(UWorld* World, AShip* Ship)
{
	if (!IsValid(Ship) || Ship->GetHasWorkingTurrets())
	{
		UE_LOG(LogTemp, Error, TEXT("Ship is not valid in UFactoriesFunctionLibrary::AddTurretsToShip()"));
		return;
	}
	if(!IsValid(Ship->GetPlayerController()))
	{
		UE_LOG(LogTemp, Error, TEXT("Ship->PlayerController is not valid in UFactoriesFunctionLibrary::AddTurretsToShip()"));
		return;
	}

	auto* GameMode = Cast<ARTSGameMode>(UGameplayStatics::GetGameMode(World));
	if (GameMode == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("UFactoriesFunctionLibrary::AddTurretsToShip ARTSGameMode is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("UFactoriesFunctionLibrary::AddTurretsToShip ARTSGameMode is nullptr"));
		return;
	}
	
	const TSubclassOf<ATurret> TurretClass = GameMode->GetFactoryAssets()->GetTurretClass(0);
	if (TurretClass)
	{
		UStaticMeshComponent* StaticMesh = Ship->GetStaticMeshComponent();

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
		Ship->SetHasWorkingTurrets(true);
	}
}


ABuilding* UFactoriesFunctionLibrary::NewBuilding(const UObject* WorldContext, UClass* ClassType, AController* PlayerController, const FVector& Location, const FRotator& Rotation)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn building, GetWorldFromContextObject() returns nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn building, GetWorldFromContextObject() returns nullptr"));
		return nullptr;
	}
	return NewBuilding(World, ClassType, PlayerController, Location, Rotation);
}


ABuilding* UFactoriesFunctionLibrary::NewBuilding(UWorld* World, UClass* ClassType, AController* PlayerController, const FVector& Location, const FRotator& Rotation)
{
	if (!PlayerController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn building, AController is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn building, AController is nullptr"));
		return nullptr;
	}

	if (!ClassType)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn ship, UClass is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn ship, UClass is nullptr"));
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = PlayerController;

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

	if (auto* PlayerState = PlayerController->GetPlayerState<ARTSPlayerState>())
	{
		PlayerState->AddToPlayersUnits(SpawnedBuilding);
		SpawnedBuilding->SetGenericTeamId(PlayerState->GetGenericTeamId());
	}
	
	return SpawnedBuilding;
}

AProjectile* UFactoriesFunctionLibrary::NewRocket(const UObject* WorldContext, UClass* ClassType, AController* Controller, ATurret* Turret, const FVector& Location, const FRotator& Rotation)
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

AProjectile* UFactoriesFunctionLibrary::NewRocket(UWorld* World, UClass* ClassType, AController* Controller, ATurret* Turret, const FVector& Location, const FRotator& Rotation)
{
	if (!Controller)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn rocket, AController is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn rocket, AController is nullptr"));
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