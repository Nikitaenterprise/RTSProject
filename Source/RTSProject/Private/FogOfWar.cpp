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
	if (Controller) PlayerController = Controller;
	else UE_LOG(LogTemp, Error, TEXT("PlayerController in AFogOfWar->Initialize is nullptr"));

	// Find FOWBoundsVolume
	if (!FOWBoundsVolume)
	{
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFogOfWarBoundsVolume::StaticClass(), OutActors);
		for (auto& Actor : OutActors)
		{
			AFogOfWarBoundsVolume* Test = Cast<AFogOfWarBoundsVolume>(Actor);
			if (Test) FOWBoundsVolume = Test;
		}
		if (!FOWBoundsVolume)
		{
			UE_LOG(LogTemp, Error, TEXT("Couldn't find AFogOfWarBoundsVolume on level, trying to spawn one"));
			FOWBoundsVolume = GetWorld()->SpawnActor<AFogOfWarBoundsVolume>(AFogOfWarBoundsVolume::StaticClass(), FVector(0, 0, 0), FRotator(0, 0, 0));
		}
	}
	FOWBoundsVolume->Initialize();
	
	// Find FOWPostProcessVolume
	if (!FOWPostProcessVolume)
	{
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), OutActors);
		for (auto& Actor : OutActors)
		{
			APostProcessVolume* Test = Cast<APostProcessVolume>(Actor);
			if (Test) FOWPostProcessVolume = Test;
		}
		if (!FOWPostProcessVolume)
		{
			UE_LOG(LogTemp, Error, TEXT("Couldn't find APostProcessVolume on level, trying to spawn one"));
			FOWPostProcessVolume = GetWorld()->SpawnActor<APostProcessVolume>(APostProcessVolume::StaticClass(), FVector(0, 0, 0), FRotator(0, 0, 0));
		}
	}
	//FOWPostProcessVolume->bUnbound = true;

	VolumeHeightInCells = FOWBoundsVolume->GetVolumeHeightInCells();
	VolumeWidthInCells = FOWBoundsVolume->GetVolumeWidthInCells();
	TextureBufferSize = VolumeWidthInCells * VolumeHeightInCells * 4;
	VolumeHeight = FOWBoundsVolume->GetVolumeHeight();
	VolumeWidth = FOWBoundsVolume->GetVolumeWidth();
	FOWTexture = UTexture2D::CreateTransient(VolumeWidthInCells, VolumeHeightInCells);
	FOWTexture->AddToRoot();
	FOWTexture->UpdateResource();
	FOWTextureBuffer = new uint8[TextureBufferSize];
	// Will update whole texture at once
	FOWUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, VolumeWidthInCells, VolumeHeightInCells);
	FOWMaterialInstance = UMaterialInstanceDynamic::Create(FOWMaterial, nullptr);
	FOWMaterialInstance->SetTextureParameterValue(FName("VisibilityMask"), FOWTexture);
	FOWMaterialInstance->SetScalarParameterValue(FName("OneOverWorldSizeX"), 1.0f / VolumeWidth);
	FOWMaterialInstance->SetScalarParameterValue(FName("OneOverTileSizeX"), 1.0f / static_cast<float>(VolumeWidthInCells));
	FOWMaterialInstance->SetScalarParameterValue(FName("OneOverWorldSizeY"), 1.0f / VolumeHeight);
	FOWMaterialInstance->SetScalarParameterValue(FName("OneOverTileSizeY"), 1.0f / static_cast<float>(VolumeHeightInCells));
	FOWMaterialInstance->SetScalarParameterValue(FName("MaxX"), FOWBoundsVolume->GetCellSideLength() * (FOWBoundsVolume->GetVolumeWidthInCells() + 1));
	FOWMaterialInstance->SetScalarParameterValue(FName("MaxY"), FOWBoundsVolume->GetCellSideLength() * (FOWBoundsVolume->GetVolumeHeightInCells() + 1));
	FOWMaterialInstance->SetScalarParameterValue(FName("MinX"), 0);
	FOWMaterialInstance->SetScalarParameterValue(FName("MinY"), 0);
	FOWPostProcessVolume->AddOrUpdateBlendable(FOWMaterialInstance);

	FString out = "";
	out += "\nVolumeWidth = " + FString::SanitizeFloat(FOWBoundsVolume->GetVolumeWidth());
	out += "\nVolumeWidthInCells = " + FString::SanitizeFloat(VolumeWidthInCells);
	out += "\nVolumeHeight = " + FString::SanitizeFloat(FOWBoundsVolume->GetVolumeHeight());
	out += "\nVolumeHeightInCells = " + FString::SanitizeFloat(VolumeHeightInCells);
	out += "\nOneOverWorldSizeX = " + FString::SanitizeFloat(1.0f / VolumeWidth);
	out += "\nOneOverTileSizeX = " + FString::SanitizeFloat(1.0f / VolumeWidthInCells);
	out += "\nOneOverWorldSizeY = " + FString::SanitizeFloat(1.0f / VolumeHeight);
	out += "\nOneOverTileSizeY = " + FString::SanitizeFloat(1.0f / VolumeHeightInCells);
	out += "\nBufferSize = " + FString::SanitizeFloat(TextureBufferSize);
	UE_LOG(LogTemp, Log, TEXT("%s"), *out);
	
	// Paint texture to FOW_UNKNOWN
	for (uint32 TileY = 0; TileY < VolumeHeightInCells; TileY++)
	{
		for (uint32 TileX = 0; TileX < VolumeWidthInCells; TileX++)
		{
			const int i = TileX + TileY * VolumeWidthInCells;
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
	FOWTexture->UpdateTextureRegions(0, 1, FOWUpdateTextureRegion, VolumeWidthInCells * 4, static_cast<uint8>(4), FOWTextureBuffer);
}

void AFogOfWar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Update texture.
	if (!FOWBoundsVolume)
	{
		UE_LOG(LogTemp, Error, TEXT("FOWBoundsVolume in FogOfWar->Tick() is nullptr"));
		return;
	}

	// Paint texture to FOW_KNOWN
	for (uint32 TileY = 0; TileY < VolumeHeightInCells; TileY++)
	{
		for (uint32 TileX = 0; TileX < VolumeWidthInCells; TileX++)
		{
			const int i = TileX + TileY * VolumeWidthInCells;
			if (FOWBoundsVolume->Grid[i].GridData == true)
			{
				const int Blue = i * 4 + 0;
				const int Green = i * 4 + 1;
				const int Red = i * 4 + 2;
				const int Alpha = i * 4 + 3;
				FOWTextureBuffer[Blue] = 0;
				FOWTextureBuffer[Green] = 255;
				FOWTextureBuffer[Red] = 0;
				FOWTextureBuffer[Alpha] = 0;
			}
		}
	}
	
	for (const auto& Tuple : RegisteredActors)
	{
		const FVector2D Location = FVector2D(Tuple.Key->GetActorLocation());
		AFogOfWarBoundsVolume::FGridCell<bool> ActorCell = FOWBoundsVolume->GetGridCellByCoordinate<bool>(Location);
		// Set this cell to be seen
		ActorCell.GridData = true;
		
		const int32 SightRadiusInCells = FMath::FloorToInt(static_cast<float>(Tuple.Value->SightRadius) / static_cast<float>(FOWBoundsVolume->GetCellSideLength()));

		for (int32 RadiusY = -SightRadiusInCells; RadiusY <= SightRadiusInCells; RadiusY++)
		{
			const int32 TileY = ActorCell.Row + RadiusY;
			//int32 LocalTileY = RadiusY + SightRadiusInCells;

			if (TileY >= 0 && TileY < static_cast<int32>(VolumeHeightInCells))
			{
				for (int32 RadiusX = -SightRadiusInCells; RadiusX <= SightRadiusInCells; RadiusX++)
				{
					const int32 TileX = ActorCell.Column + RadiusX;
					//int32 LocalTileX = RadiusX + SightRadiusInCells;

					// Check if within circle.
					if (TileX >= 0 && TileX < static_cast<int32>(VolumeWidthInCells))
					{
						const int i = TileX + TileY * VolumeWidthInCells;
						const int Blue = i * 4 + 0;
						const int Green = i * 4 + 1;
						const int Red = i * 4 + 2;
						const int Alpha = i * 4 + 3;
						if (RadiusX * RadiusX + RadiusY * RadiusY <= SightRadiusInCells * SightRadiusInCells)
						{
							FOWBoundsVolume->Grid[i].GridData = true;
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
	// SrcPitch is width * bytes_per_pixel
	// SrcBpp is the size one pixel data in bytes so RGBA is 4 bytes
	FOWTexture->UpdateTextureRegions(0, 1, FOWUpdateTextureRegion, VolumeWidthInCells * 4, static_cast<uint8>(4), FOWTextureBuffer);
	//UpdateTextureRegions(FOWTexture, 0, 1, FOWUpdateTextureRegion, SizeInTiles * 4, static_cast<uint32>(4), FOWTextureBuffer, false);
}

void AFogOfWar::RegisterActor(AActor* ActorToRegister)
{
	if (!ActorToRegister)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor is unregistered in FogOfWar because it's nullptr"));
		return;
	}
	UFogOfWarInfluencerComponent* Component = Cast<UFogOfWarInfluencerComponent>(ActorToRegister->FindComponentByClass(UFogOfWarInfluencerComponent::StaticClass()));
	if (!Component)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor %s is unregistered in FogOfWar because he hasn't UFogOfWarInfluencerComponent"), *ActorToRegister->GetHumanReadableName());
		return;
	}
	RegisteredActors.AddUnique(TPair<AActor*, UFogOfWarInfluencerComponent*>(ActorToRegister, Component));
	UE_LOG(LogTemp, Log, TEXT("Actor %s registered in FogOfWar"), *ActorToRegister->GetHumanReadableName());
}

void AFogOfWar::UnRegisterActor(AActor* ActorToRegister)
{
	if (ActorToRegister == nullptr) return;
	UFogOfWarInfluencerComponent* Influencer = Cast<UFogOfWarInfluencerComponent>(ActorToRegister->FindComponentByClass(UFogOfWarInfluencerComponent::StaticClass()));
	if (Influencer == nullptr) return;
	RegisteredActors.Remove(TPair<AActor*, UFogOfWarInfluencerComponent*>(ActorToRegister, Influencer));
	UE_LOG(LogTemp, Log, TEXT("Actor %s unregistered in FogOfWar"), *ActorToRegister->GetHumanReadableName());
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

		RegionData->Texture2DResource = static_cast<FTexture2DResource*>(Texture->Resource);
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

