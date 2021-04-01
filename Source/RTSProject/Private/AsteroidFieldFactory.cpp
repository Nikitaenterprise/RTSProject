#include "AsteroidFieldFactory.h"

#include "RTSPlayerController.h"
#include "FactoryAssets.h"
#include "AsteroidResource.h"
#include "AsteroidField.h"

//#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"


AAsteroidField* AsteroidFieldFactory::NewAsteroidField(UWorld* _World, ARTSPlayerController* _Controller)
{
	return NewAsteroidField(_World, FVector(0, 0, 0), _Controller);
}

AAsteroidField* AsteroidFieldFactory::NewAsteroidField(UWorld* _World, const FVector& _Location,
                                                       ARTSPlayerController* _Controller)
{
	const TSubclassOf<AAsteroidField> AsteroidFieldClass = _Controller->GetFactoryAssets()->AsteroidFieldClass;
	if (AsteroidFieldClass) {
		
		AAsteroidField* SpawnedAsteroidField = _World->SpawnActor<AAsteroidField>(AsteroidFieldClass.Get(),
											_Location, FRotator::ZeroRotator, GetDefaultSpawnParams());
		if (!SpawnedAsteroidField)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Failed to spawn asteroid field"));
			return nullptr;
		}
		SpawnedAsteroidField->PlayerController = _Controller;
		SpawnedAsteroidField->AddRandomNumberOfAsteroidsToField();
		return SpawnedAsteroidField;
	}
	return nullptr;
}

FActorSpawnParameters AsteroidFieldFactory::GetDefaultSpawnParams()
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	Params.Instigator = nullptr;
	Params.Owner = nullptr;
	return Params;
}
