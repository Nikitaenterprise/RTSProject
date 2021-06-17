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

	UPROPERTY(BlueprintReadOnly)
	AFogOfWar* FOW = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int DesiredCellSizeLength = 32;
	template <class T>
	struct FGridCell
	{
		uint32 Column = 0;
		uint32 Row = 0;
		T GridData;
		FGridCell() { Column = 0; Row = 0; }
		FGridCell(const uint32 Column = 0, const uint32 Row = 0) { this->Column = Column; this->Row = Row; }
		FORCEINLINE bool operator==(const FGridCell& Cell) const { return Column == Cell.Column && Row == Cell.Row; }
		FORCEINLINE bool operator!=(const FGridCell& Cell) const { return Column != Cell.Column || Row != Cell.Row; }
		FORCEINLINE bool operator>(const FGridCell& Cell) const { return Column > Cell.Column && Row > Cell.Row; }
		FORCEINLINE bool operator>=(const FGridCell& Cell) const { return Column >= Cell.Column && Row >= Cell.Row; }
		FORCEINLINE bool operator<(const FGridCell& Cell) const { return Column < Cell.Column && Row < Cell.Row; }
		FORCEINLINE bool operator<=(const FGridCell& Cell) const { return Column <= Cell.Column && Row <= Cell.Row; }

		FORCEINLINE FGridCell operator*(uint32 Scale) const { return FGridCell(Column * Scale, Row * Scale); }
		FORCEINLINE FGridCell operator*(FGridCell& Cell) const { return FGridCell(Column * Cell.Column, Row * Cell.Row); }
	};

	TArray<FGridCell<bool>> Grid;

private:

	uint32 CellSideLength = 32;

public:

	//AFogOfWarBoundsVolume();
	void Initialize();
	
	FORCEINLINE float GetVolumeLength() const { return GetBrushComponent()->CalcBounds(GetBrushComponent()->GetComponentTransform()).BoxExtent.X * 2 * GetActorScale().X; }
	FORCEINLINE float GetVolumeWidth() const { return GetBrushComponent()->CalcBounds(GetBrushComponent()->GetComponentTransform()).BoxExtent.Y * 2 * GetActorScale().Y; }
	FORCEINLINE uint32 GetCellSideLength() const { return CellSideLength; }
	FORCEINLINE uint32 GetVolumeLengthInCells() const { return static_cast<uint32>(floorf(GetVolumeLength() / CellSideLength)); }
	FORCEINLINE uint32 GetVolumeWidthInCells() const { return static_cast<uint32>(floorf(GetVolumeWidth() / CellSideLength)); }
	
	template<class T>
	FGridCell<T> GetGridCellByCoordinate(const FVector2D& Position) const
	{
		uint32 X = floorf(Position.X / CellSideLength) + GetVolumeWidthInCells() / 2;
		uint32 Y = floorf(Position.Y / CellSideLength) + GetVolumeLengthInCells() / 2;
		return Grid[Y * GetVolumeWidthInCells() + X];
	}

	template<class T>
	FORCEINLINE FVector2D GetXYMinOfGridCell(const FGridCell<T>& Cell) const { return FVector2D(Cell.Column, Cell.Row) * CellSideLength; }
	template<class T>
	FORCEINLINE FVector2D GetXYMaxOfGridCell(const FGridCell<T>& Cell) const { return FVector2D(Cell.Column + 1, Cell.Row + 1) * CellSideLength; }

private:
	
	void PaveVolumeWithGrid();
	
};