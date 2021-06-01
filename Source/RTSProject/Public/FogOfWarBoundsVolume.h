#pragma once

#include "CoreMinimal.h"

#include "Components/BrushComponent.h"
#include "GameFramework/Volume.h"
#include "FogOfWarBoundsVolume.generated.h"

class AFogOfWar;

UCLASS()
class RTSPROJECT_API AFogOfWarBoundsVolume : public AVolume
{
	GENERATED_BODY()

public:

	UPROPERTY()
	AFogOfWar* FOW = nullptr;

	struct FGridCell
	{
		uint8 Row = 0;
		uint8 Column = 0;
		FGridCell() { Row = 0; Column = 0; }
		FGridCell(const uint8 Row = 0, const uint8 Column = 0) { this->Row = Row; this->Column = Column; }
		FORCEINLINE bool operator==(const FGridCell& Cell) const { return Column == Cell.Column && Row == Cell.Row; }
		FORCEINLINE bool operator!=(const FGridCell& Cell) const { return Column != Cell.Column || Row != Cell.Row; }
		FORCEINLINE bool operator>(const FGridCell& Cell) const { return Column > Cell.Column && Row > Cell.Row; }
		FORCEINLINE bool operator>=(const FGridCell& Cell) const { return Column >= Cell.Column && Row >= Cell.Row; }
		FORCEINLINE bool operator<(const FGridCell& Cell) const { return Column < Cell.Column && Row < Cell.Row; }
		FORCEINLINE bool operator<=(const FGridCell& Cell) const { return Column <= Cell.Column && Row <= Cell.Row; }

		FORCEINLINE FGridCell operator*(uint8 Scale) const { return FGridCell(Column*Scale, Row*Scale); }
		FORCEINLINE FGridCell operator*(FGridCell& Cell) const { return FGridCell(Column * Cell.Column, Row * Cell.Row); }
	};

	TArray<FGridCell> Grid;

public:

	//AFogOfWarBoundsVolume();
	void Initialize();
	
	FORCEINLINE float GetVolumeLength() const { return GetBrushComponent()->CalcBounds(GetBrushComponent()->GetComponentTransform()).BoxExtent.X * 2; }
	FORCEINLINE float GetVolumeWidth() const { return GetBrushComponent()->CalcBounds(GetBrushComponent()->GetComponentTransform()).BoxExtent.Y * 2; }

	

private:
	
	void PaveVolumeWithGrid();
	
};
