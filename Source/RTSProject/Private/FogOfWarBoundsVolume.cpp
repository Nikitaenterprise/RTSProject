#include "FogOfWarBoundsVolume.h"

void AFogOfWarBoundsVolume::Initialize()
{
	PaveVolumeWithGrid();
}

void AFogOfWarBoundsVolume::PaveVolumeWithGrid()
{
	Grid.Empty();
	const uint32 XMin = 0;
	const uint32 XMax = GetVolumeLength();
	const uint32 YMin = 0;
	const uint32 YMax = GetVolumeWidth();

	FString out = "";
	out += FString("\nXMax = ") + FString::SanitizeFloat(XMax);
	out += FString("\nYMax = ") + FString::SanitizeFloat(YMax);
	out += FString("\nBufferSize = ") + FString::SanitizeFloat(GetVolumeLength()* GetVolumeLength()*4);
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, out);

	Grid.Reserve(XMax * YMax);
	for (uint32 j = YMin, Row = 0; j < YMax; j += CellSideLength, Row++)
	{
		for (uint32 i = XMin, Column = 0; i < XMax; i += CellSideLength, Column++)
		{
			FGridCell<bool> Cell(Column,Row);
			Cell.GridData = false;
			Grid.Emplace(Cell);
		}
	}
	
}
