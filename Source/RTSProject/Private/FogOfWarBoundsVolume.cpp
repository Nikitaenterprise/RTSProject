// Fill out your copyright notice in the Description page of Project Settings.


#include "FogOfWarBoundsVolume.h"

void AFogOfWarBoundsVolume::Initialize()
{
	PaveVolumeWithGrid();
}

void AFogOfWarBoundsVolume::PaveVolumeWithGrid()
{
	Grid.Empty();
	const uint8 XMin = 0;
	const uint8 XMax = GetVolumeLength();
	const uint8 YMin = 0;
	const uint8 YMax = GetVolumeWidth();

	FString out = "";
	out += FString("XMax = ") + FString::SanitizeFloat(XMax);
	out += FString("YMax = ") + FString::SanitizeFloat(YMax);
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, out);

	for (uint8 i = XMin; i < XMax; i++)
	{
		for (uint8 j = YMin; j < YMax; j++)
		{
			FGridCell Cell(i, j);
			Grid.Emplace(Cell);
		}
	}
}
