#include "FogOfWarBoundsVolume.h"

void AFogOfWarBoundsVolume::Initialize()
{
	// Check if DesiredCellSizeLength is power of 2 and if so then set CellSideLength to it
	if ((DesiredCellSizeLength > 0) && ((DesiredCellSizeLength & (DesiredCellSizeLength - 1)) == 0)) CellSideLength = DesiredCellSizeLength;
	PaveVolumeWithGrid();
}

void AFogOfWarBoundsVolume::PaveVolumeWithGrid()
{
	Grid.Empty();
	const int32 XMin = -GetVolumeWidth()/2;
	const int32 XMax = GetVolumeWidth()/2;
	const int32 YMin = -GetVolumeLength()/2;
	const int32 YMax = GetVolumeLength()/2;


	Grid.Reserve((XMax - XMin) * (YMax - YMin));
	for (int32 j = YMin, Row = 0; j < YMax; j += CellSideLength, Row++)
	{
		for (int32 i = XMin, Column = 0; i < XMax; i += CellSideLength, Column++)
		{
			FGridCell<bool> Cell(Column,Row);
			Cell.GridData = false;
			Grid.Emplace(Cell);
		}
	}
	
	FString out = "";
	out += "FogOfWarBoundsVolume";
	out += FString("\nXMax = ") + FString::SanitizeFloat(XMax);
	out += FString("\nYMax = ") + FString::SanitizeFloat(YMax);
	out += FString("\nCreated Grid array with size = ") + FString::SanitizeFloat(Grid.Num());
	UE_LOG(LogTemp, Log, TEXT("%S"), *out);
}
