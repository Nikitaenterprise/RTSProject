#include "FogOfWar.h"


#include "FogOfWarBoundsVolume.h"
#include "FogOfWarInfluencer.h"
#include "Engine/PostProcessVolume.h"
#include "Kismet/GameplayStatics.h"

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

	FOWTexture = UTexture2D::CreateTransient(1024, 1024);
	FOWTexture->AddToRoot();
	FOWTexture->UpdateResource();
	FOWTextureBuffer = new uint8[1024 * 1024 * 4];
	FOWUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, 1024, 1024);
	FOWMaterialInstance = UMaterialInstanceDynamic::Create(FOWMaterial, nullptr);
	FOWMaterialInstance->SetTextureParameterValue(FName("VisibilityMask"), FOWTexture);
	FOWMaterialInstance->SetScalarParameterValue(FName("OneOverWorldSize"), 1.0f / 1024);
	FOWMaterialInstance->SetScalarParameterValue(FName("OneOverTileSize"), 1.0f / 1024);

	FOWPostProcessVolume->AddOrUpdateBlendable(FOWMaterialInstance);
}

void AFogOfWar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Update texture.
	if (!FOWBoundsVolume)
	{
		return;
	}

	for(auto& Actor : RegistredActors)
	{
		UFogOfWarInfluencer* ActorFOWInfluencer = Actor->FindComponentByClass<UFogOfWarInfluencer>();
		
	}
	
	int32 SizeInTiles = 1024;
	for (int32 Y = 0; Y < SizeInTiles; ++Y)
	{
		for (int32 X = 0; X < SizeInTiles; ++X)
		{
			const int i = X + Y * SizeInTiles;

			const int Blue = i * 4 + 0;
			const int Green = i * 4 + 1;
			const int Red = i * 4 + 2;
			const int Alpha = i * 4 + 3;

			FOWTextureBuffer[Blue] = 0;
			FOWTextureBuffer[Green] = 0;
			FOWTextureBuffer[Red] = 255;
			FOWTextureBuffer[Alpha] = 0;
		}
	}

	FOWTexture->UpdateTextureRegions(0, 1, FOWUpdateTextureRegion, SizeInTiles * 4, static_cast<uint32>(4), FOWTextureBuffer);
	//UpdateTextureRegions(FOWTexture, 0, 1, FOWUpdateTextureRegion, SizeInTiles * 4, static_cast<uint32>(4), FOWTextureBuffer, false);
}


void AFogOfWar::RegisterActor(AActor* ActorToRegister)
{
	if (ActorToRegister == nullptr) return;
	if (ActorToRegister->FindComponentByClass(UFogOfWarInfluencer::StaticClass()) == nullptr) return;
	RegistredActors.AddUnique(ActorToRegister);
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

		RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
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
				const int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
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
		});
	}
}


