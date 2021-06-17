#include "FogOfWar.h"

#include "FogOfWarInfluencerComponent.h"
#include "FogOfWarBoundsVolume.h"
#include "Engine/PostProcessVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Rendering/Texture2DResource.h"

AFogOfWar::AFogOfWar()
{
 	PrimaryActorTick.bCanEverTick = true;

}

void AFogOfWar::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFogOfWar::Initialize(ARTSPlayerController* Controller)
{
	// Find FOWBoundsVolume
	if (!FOWBoundsVolume)
	{
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFogOfWarBoundsVolume::StaticClass(), OutActors);
		for (auto& a : OutActors)
		{
			AFogOfWarBoundsVolume* test = Cast<AFogOfWarBoundsVolume>(a);
			if (test) FOWBoundsVolume = test;
		}
		if (!FOWBoundsVolume)
		{
			FOWBoundsVolume = GetWorld()->SpawnActor<AFogOfWarBoundsVolume>(AFogOfWarBoundsVolume::StaticClass(), FVector(0, 0, 0), FRotator(0, 0, 0));
		}
	}
	FOWBoundsVolume->Initialize();
	// Find FOWPostProcessVolume
	if (!FOWPostProcessVolume)
	{
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), OutActors);
		for (auto& a : OutActors)
		{
			APostProcessVolume* test = Cast<APostProcessVolume>(a);
			if (test) FOWPostProcessVolume = test;
		}
		if (!FOWPostProcessVolume)
		{
			FOWPostProcessVolume = GetWorld()->SpawnActor<APostProcessVolume>(APostProcessVolume::StaticClass(), FVector(0, 0, 0), FRotator(0, 0, 0));
		}
	}
	FOWPostProcessVolume->bUnbound = true;

	VolumeLengthInCells = FOWBoundsVolume->GetVolumeLengthInCells();
	TextureBufferSize = VolumeLengthInCells * VolumeLengthInCells * 4;
	VolumeLength = static_cast<float>(FOWBoundsVolume->GetVolumeLength());
	FOWTexture = UTexture2D::CreateTransient(VolumeLengthInCells, VolumeLengthInCells);
	FOWTexture->AddToRoot();
	FOWTexture->UpdateResource();
	FOWTextureBuffer = new uint8[TextureBufferSize];
	// Will update whole texture at once
	FOWUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, VolumeLengthInCells, VolumeLengthInCells);
	FOWMaterialInstance = UMaterialInstanceDynamic::Create(FOWMaterial, nullptr);
	FOWMaterialInstance->SetTextureParameterValue(FName("VisibilityMask"), FOWTexture);
	FOWMaterialInstance->SetScalarParameterValue(FName("OneOverWorldSize"), 1.0f / VolumeLength);
	FOWMaterialInstance->SetScalarParameterValue(FName("OneOverTileSize"), 1.0f / VolumeLengthInCells);

	FOWPostProcessVolume->AddOrUpdateBlendable(FOWMaterialInstance);

	FString out = "";
	out += FString("\nSizeInTiles = ") + FString::SanitizeFloat(VolumeLengthInCells);
	out += FString("\nOneOverTileSize = ") + FString::SanitizeFloat(1.0f / VolumeLengthInCells);
	out += FString("\nOneOverWorldSize = ") + FString::SanitizeFloat(1.0f / VolumeLength);
	out += FString("\nBufferSize = ") + FString::SanitizeFloat(TextureBufferSize);
	GEngine->AddOnScreenDebugMessage(-1, 100, FColor::White, out);

	// Paint texture to FOW_UNKNOWN
	for (uint32 Y = 0; Y < VolumeLengthInCells; Y++)
	{
		for (uint32 X = 0; X < VolumeLengthInCells; X++)
		{
			const int i = X + Y * VolumeLengthInCells;
			const int Blue = i * 4 + 0;
			const int Green = i * 4 + 1;
			const int Red = i * 4 + 2;
			const int Alpha = i * 4 + 3;
			FOWTextureBuffer[Blue] = 0;
			FOWTextureBuffer[Green] = 0;
			FOWTextureBuffer[Red] = 0;
			FOWTextureBuffer[Alpha] = 0;
		}
	}
	FOWTexture->UpdateTextureRegions(0, 1, FOWUpdateTextureRegion, VolumeLengthInCells * 4, static_cast<uint32>(4), FOWTextureBuffer);
}

void AFogOfWar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Update texture.
	if (!FOWBoundsVolume)
	{
		return;
	}

	for (const auto& Tuple : RegisteredActors)
	{
		const FVector2D Location = FVector2D(Tuple.Key->GetActorLocation());
		AFogOfWarBoundsVolume::FGridCell<bool> Cell = FOWBoundsVolume->GetGridCellByCoordinate<bool>(Location);
		const int32 SightRadiusInCells = FMath::FloorToInt(Tuple.Value->SightRadius / FOWBoundsVolume->GetCellSideLength());

		for (int32 RadiusY = -SightRadiusInCells; RadiusY <= SightRadiusInCells; RadiusY++)
		{
			const int32 TileY = Cell.Row + RadiusY;
			const int32 RadiusYSqr = RadiusY * RadiusY;
			int32 LocalTileY = RadiusY + SightRadiusInCells;

			if (TileY >= 0 && TileY < static_cast<int32>(VolumeLengthInCells))
			{
				for (int32 RadiusX = -SightRadiusInCells; RadiusX <= SightRadiusInCells; RadiusX++)
				{
					const int32 TileX = Cell.Column + RadiusX;
					const int32 RadiusXSqr = RadiusX * RadiusX;
					int32 LocalTileX = RadiusX + SightRadiusInCells;

					// Check if within circle.
					if (TileX >= 0 && TileX < static_cast<int32>(VolumeLengthInCells))
					{
						const int i = TileX + TileY * VolumeLengthInCells;
						const int Blue = i * 4 + 0;
						const int Green = i * 4 + 1;
						const int Red = i * 4 + 2;
						const int Alpha = i * 4 + 3;
						if (RadiusXSqr + RadiusYSqr <= SightRadiusInCells * SightRadiusInCells)
						{
							FOWTextureBuffer[Blue] = 0;
							FOWTextureBuffer[Green] = 0;
							FOWTextureBuffer[Red] = 255;
							FOWTextureBuffer[Alpha] = 0;
						}
					}
				}
			}
		}
	}
	/*for (uint32 Y = 0; Y < VolumeLengthInCells; Y++)
	{
		for (uint32 X = 0; X < VolumeLengthInCells; X++)
		{
			const int i = X + Y * VolumeLengthInCells;

			const int Blue = i * 4 + 0;
			const int Green = i * 4 + 1;
			const int Red = i * 4 + 2;
			const int Alpha = i * 4 + 3;
			
			switch(GetActorVisionData(X,Y))
			{
			case EFOWState::FOW_VISIBLE:
				FOWTextureBuffer[Blue] = 0;
				FOWTextureBuffer[Green] = 0;
				FOWTextureBuffer[Red] = 255;
				FOWTextureBuffer[Alpha] = 0;
				break;

			case EFOWState::FOW_KNOWN:
				FOWTextureBuffer[Blue] = 0;
				FOWTextureBuffer[Green] = 255;
				FOWTextureBuffer[Red] = 0;
				FOWTextureBuffer[Alpha] = 0;
				break;

			case EFOWState::FOW_TERRA_INCOGNITA:
				FOWTextureBuffer[Blue] = 0;
				FOWTextureBuffer[Green] = 0;
				FOWTextureBuffer[Red] = 0;
				FOWTextureBuffer[Alpha] = 0;
				break;
			}

		}
	}*/
	// SrcPitch is width * bytes_per_pixel in my case SizeInTiles * 4
	// SrcBpp is the size one pixel data in bytes so RGBA is 4 bytes
	FOWTexture->UpdateTextureRegions(0, 1, FOWUpdateTextureRegion, VolumeLengthInCells * 4, static_cast<uint32>(4), FOWTextureBuffer);
	//UpdateTextureRegions(FOWTexture, 0, 1, FOWUpdateTextureRegion, SizeInTiles * 4, static_cast<uint32>(4), FOWTextureBuffer, false);
}

void AFogOfWar::RegisterActor(AActor* ActorToRegister)
{
	if (ActorToRegister == nullptr) return;
	UFogOfWarInfluencerComponent* Influencer = Cast<UFogOfWarInfluencerComponent>(ActorToRegister->FindComponentByClass(UFogOfWarInfluencerComponent::StaticClass()));
	if (Influencer == nullptr) return;
	RegisteredActors.AddUnique(TPair<AActor*, UFogOfWarInfluencerComponent*>(ActorToRegister, Influencer));
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString::Printf(TEXT("Actor %s registered"), *ActorToRegister->GetHumanReadableName()));
}

void AFogOfWar::UnRegisterActor(AActor* ActorToRegister)
{
	if (ActorToRegister == nullptr) return;
	UFogOfWarInfluencerComponent* Influencer = Cast<UFogOfWarInfluencerComponent>(ActorToRegister->FindComponentByClass(UFogOfWarInfluencerComponent::StaticClass()));
	if (Influencer == nullptr) return;
	RegisteredActors.Remove(TPair<AActor*, UFogOfWarInfluencerComponent*>(ActorToRegister, Influencer));
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::White, FString::Printf(TEXT("Actor %s unregistered"), *ActorToRegister->GetHumanReadableName()));
}

inline TArray<AActor*> AFogOfWar::GetRegisteredActors() const
{
	TArray<AActor*> Actors;
	Actors.Reserve(RegisteredActors.Num());
	for (const auto& Tuple : RegisteredActors)
	{
		Actors.Add(Tuple.Key);
	}
	return Actors;
}

EFOWState AFogOfWar::GetActorVisionData(uint32 X, uint32 Y)
{
	for (auto& Tuple : RegisteredActors)
	{
		
		const FVector2D Location = FVector2D(Tuple.Key->GetActorLocation());
		AFogOfWarBoundsVolume::FGridCell<bool> Cell = FOWBoundsVolume->GetGridCellByCoordinate<bool>(Location);
		FVector2D XYMin = FOWBoundsVolume->GetXYMinOfGridCell(Cell);
		FVector2D XYMax = FOWBoundsVolume->GetXYMaxOfGridCell(Cell);
		if (Location.X > XYMin.X && Location.Y < XYMin.Y && Location.X < XYMax.X && Location.Y > XYMax.Y) return EFOWState::FOW_VISIBLE;
	}
	return EFOWState::FOW_TERRA_INCOGNITA;
}

void AFogOfWar::UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData)
{
	if (Texture && Texture->Resource)
	{
		struct FUpdateTextureRegionsData
		{
			FTexture2DResource* Texture2DResource;
			int32 MipIndex;
			uint32 NumRegions;
			FUpdateTextureRegion2D* Regions;
			uint32 SrcPitch;
			uint32 SrcBpp;
			uint8* SrcData;
		};

		FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

		RegionData->Texture2DResource = (FTexture2DResource*)(Texture->Resource);
		RegionData->MipIndex = MipIndex;
		RegionData->NumRegions = NumRegions;
		RegionData->Regions = Regions;
		RegionData->SrcPitch = SrcPitch;
		RegionData->SrcBpp = SrcBpp;
		RegionData->SrcData = SrcData;

		ENQUEUE_RENDER_COMMAND(FUpdateTextureRegionsData)([RegionData = RegionData, bFreeData = bFreeData](FRHICommandListImmediate& RHICmdList)
			{
				for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
				{
					int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
					if (RegionData->MipIndex >= CurrentFirstMip)
					{
						RHIUpdateTexture2D(
							RegionData->Texture2DResource->GetTexture2DRHI(),
							RegionData->MipIndex - CurrentFirstMip,
							RegionData->Regions[RegionIndex],
							RegionData->SrcPitch,
							RegionData->SrcData
							+ RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
							+ RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
						);
					}
				}
				if (bFreeData)
				{
					FMemory::Free(RegionData->Regions);
					FMemory::Free(RegionData->SrcData);
				}
				delete RegionData;
			}
		);
	}
}

void AFogOfWar::ApplyVision(TPair<AActor*, UFogOfWarInfluencerComponent*>* Tuple)
{
	const FVector2D Location = FVector2D(Tuple->Key->GetActorLocation());
	AFogOfWarBoundsVolume::FGridCell<bool> Cell = FOWBoundsVolume->GetGridCellByCoordinate<bool>(Location);
	const int32 SightRadiusInCells = FMath::FloorToInt(Tuple->Value->SightRadius / FOWBoundsVolume->GetCellSideLength());

	for (int32 RadiusY = -SightRadiusInCells; RadiusY <= SightRadiusInCells; RadiusY++)
	{
		uint32 TileY = Cell.Row + RadiusY;
		const int32 RadiusYSqr = RadiusY * RadiusY;
		int32 LocalTileY = RadiusY + SightRadiusInCells;

		if (TileY >= 0 && TileY < VolumeLengthInCells)
		{
			for (int32 RadiusX = -SightRadiusInCells; RadiusX <= SightRadiusInCells; RadiusX++)
			{
				uint32 TileX = Cell.Column + RadiusX;
				const int32 RadiusXSqr = RadiusX * RadiusX;
				int32 LocalTileX = RadiusX + SightRadiusInCells;

				// Check if within circle.
				if (TileX >= 0 && TileX < VolumeLengthInCells && (RadiusXSqr + RadiusYSqr <= SightRadiusInCells*SightRadiusInCells))
				{
					/*if (HasLocalVisionAt(LocalTileX, LocalTileY, ActorSightRadiusTile, TileX, TileY,
							ActorLocationTile.X, ActorLocationTile.Y, ActorLocationHeightLevel))
					{
						CachedTiles.Add(GetTileIndex(TileX, TileY));
					}*/
				}
			}
		}
	}
}