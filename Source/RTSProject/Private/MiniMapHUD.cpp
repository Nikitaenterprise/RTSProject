#include "MiniMapHUD.h"

#include "FogOfWar.h"
#include "FogOfWarBoundsVolume.h"
#include "MiniMapIconComponent.h"
#include "MiniMapInfluencerComponent.h"
#include "RTSPlayerController.h"


void UMiniMapHUD::Initialize(ARTSPlayerController* Controller)
{
	if (Controller) PlayerController = Controller;
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController in UMiniMapHUD->Initialize is nullptr"));
		return;
	}

	if (PlayerController->FogOfWar->FOWBoundsVolume) FOWBoundsVolume = PlayerController->FogOfWar->FOWBoundsVolume;
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FOWBoundsVolume in UMiniMapHUD->Initialize is nullptr"));
		return;
	}

	FOWTexture = UTexture2D::CreateTransient(PlayerController->FogOfWar->VolumeWidthInCells, PlayerController->FogOfWar->VolumeHeightInCells);
	FOWTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	FOWTexture->AddToRoot();
	FOWTexture->UpdateResource();
	
	MiniMapUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, PlayerController->FogOfWar->VolumeWidthInCells, PlayerController->FogOfWar->VolumeHeightInCells);
	MiniMapTextureBuffer = new uint8[PlayerController->FogOfWar->VolumeWidthInCells * PlayerController->FogOfWar->VolumeHeightInCells * 4];
	MiniMapMaterialInstance = UMaterialInstanceDynamic::Create(MiniMapMaterial, nullptr);
	MiniMapMaterialInstance->SetTextureParameterValue(FName("FogOfWarMask"), FOWTexture);

	if (!DefaultIcon)
	{
		UE_LOG(LogTemp, Error, TEXT("DefaultIcon in UMiniMapHUD->Initialize is nullptr"));
		return;
	}
}

void UMiniMapHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	TArray<TPair<uint32, uint32>> Actors;
	for (const auto& Actor : MiniMapInfluencers)
	{
		FVector Location = Actor->GetActorLocation();
		AFogOfWarBoundsVolume::FGridCell<bool> Cell = FOWBoundsVolume->GetGridCellByCoordinate<bool>(FVector2D(Location));
		Actors.Add(TPair<uint32, uint32> (Cell.Column, Cell.Row));
		GEngine->AddOnScreenDebugMessage(-1, 0.01, FColor::White, FString::Printf(TEXT("%s, X=%s, Y=%s"), *Actor->GetName(), *FString::SanitizeFloat(Cell.Column), *FString::SanitizeFloat(Cell.Row)));
	}

	for (uint32 TileY = 0; TileY < PlayerController->FogOfWar->VolumeHeightInCells; TileY++)
	{
		for (uint32 TileX = 0; TileX < PlayerController->FogOfWar->VolumeWidthInCells; TileX++)
		{
			const int i = TileX + TileY * PlayerController->FogOfWar->VolumeWidthInCells;
			const int Blue = i * 4 + 0;
			const int Green = i * 4 + 1;
			const int Red = i * 4 + 2;
			const int Alpha = i * 4 + 3;
			MiniMapTextureBuffer[Blue] = PlayerController->FogOfWar->FOWTextureBuffer[Blue];
			MiniMapTextureBuffer[Green] = PlayerController->FogOfWar->FOWTextureBuffer[Green];
			MiniMapTextureBuffer[Red] = PlayerController->FogOfWar->FOWTextureBuffer[Red];
			MiniMapTextureBuffer[Alpha] = PlayerController->FogOfWar->FOWTextureBuffer[Alpha];

			for(const auto& Data : Actors)
			{
				if (Data.Key == TileX && Data.Value == TileY)
				{
					MiniMapTextureBuffer[Blue] = 255;
					/*MiniMapTextureBuffer[(TileX - 1 + (TileY - 1) * PlayerController->FogOfWar->VolumeWidthInCells)*4] = 255;
					MiniMapTextureBuffer[(TileX - 1 + (TileY + 1) * PlayerController->FogOfWar->VolumeWidthInCells)*4] = 255;
					MiniMapTextureBuffer[(TileX + 1 + (TileY - 1) * PlayerController->FogOfWar->VolumeWidthInCells)*4] = 255;
					MiniMapTextureBuffer[(TileX + 1 + (TileY + 1) * PlayerController->FogOfWar->VolumeWidthInCells)*4] = 255;
					MiniMapTextureBuffer[(TileX + 1 + TileY * PlayerController->FogOfWar->VolumeWidthInCells)*4] = 255;
					MiniMapTextureBuffer[(TileX - 1 + TileY * PlayerController->FogOfWar->VolumeWidthInCells)*4] = 255;
					MiniMapTextureBuffer[(TileX + (TileY + 1) * PlayerController->FogOfWar->VolumeWidthInCells)*4] = 255;
					MiniMapTextureBuffer[(TileX + (TileY - 1) * PlayerController->FogOfWar->VolumeWidthInCells)*4] = 255;*/
				}
			}
		}
	}
	FOWTexture->UpdateTextureRegions(0, 1, MiniMapUpdateTextureRegion, PlayerController->FogOfWar->VolumeWidthInCells * 4, static_cast<uint8>(4), MiniMapTextureBuffer);
}

void UMiniMapHUD::RegisterActor(AActor* ActorToRegister)
{
	if (!ActorToRegister)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor is unregistered in UMiniMapHUD because it's nullptr"));
		return;
	}

	UMiniMapInfluencerComponent* Component = Cast<UMiniMapInfluencerComponent>(ActorToRegister->FindComponentByClass(UMiniMapInfluencerComponent::StaticClass()));
	if (!Component)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor %s is unregistered in UMiniMapHUD because he hasn't UMiniMapInfluencerComponent"), *ActorToRegister->GetHumanReadableName());
		return;
	}
	MiniMapInfluencers.AddUnique(ActorToRegister);

	UMiniMapIconComponent* Icon = Cast<UMiniMapIconComponent>(ActorToRegister->FindComponentByClass(UMiniMapIconComponent::StaticClass()));
	if (!Icon)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor %s has no UMiniMapIconComponent. Will set default icon"), *ActorToRegister->GetHumanReadableName());
		Icon->MiniMapIcon = DefaultIcon;
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Actor %s registered in UMiniMapHUD"), *ActorToRegister->GetHumanReadableName());
}