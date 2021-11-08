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

	AFogOfWarBoundsVolume();
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void Initialize();
	
	FORCEINLINE float GetVolumeHeight() const { return GetBrushComponent()->CalcBounds(GetBrushComponent()->GetComponentTransform()).BoxExtent.Y * 2 * GetActorScale().Y; }
	FORCEINLINE float GetVolumeWidth() const { return GetBrushComponent()->CalcBounds(GetBrushComponent()->GetComponentTransform()).BoxExtent.X * 2 * GetActorScale().X; }
	FORCEINLINE uint32 GetCellSideLength() const { return CellSideLength; }
	FORCEINLINE uint32 GetVolumeHeightInCells() const { return static_cast<uint32>(floorf(GetVolumeHeight() / static_cast<float>(CellSideLength))); }
	FORCEINLINE uint32 GetVolumeWidthInCells() const { return static_cast<uint32>(floorf(GetVolumeWidth() / static_cast<float>(CellSideLength))); }

	template<class T>
	FGridCell<T>& GetGridCellByCoordinate(const FVector2D& Position)
	{
		uint32 X = floorf(Position.X / static_cast<float>(CellSideLength)) + static_cast<float>(GetVolumeWidthInCells()) / 2.0f;
		uint32 Y = floorf(Position.Y / static_cast<float>(CellSideLength)) + static_cast<float>(GetVolumeHeightInCells()) / 2.0f;
		if (X + Y * GetVolumeWidthInCells() < static_cast<uint32>(Grid.Num())) return Grid[X + Y * GetVolumeWidthInCells()];
		else return Grid[0];
	}

	/// <summary>
	/// Given the cell grid returns it's top left coordinates
	/// </summary>
	/// <typeparam name="T">Cell template type (bool)</typeparam>
	/// <param name="Cell">Cell to work with</param>
	/// <returns>Top left coordinate as FVecor2D (minimum X and Y)</returns>
	template<class T>
	FORCEINLINE FVector2D GetXYMinOfGridCell(const FGridCell<T>& Cell) const { return FVector2D(Cell.Column, Cell.Row) * CellSideLength; }

	/// <summary>
	/// Given the cell grid returns it's down right coordinates
	/// </summary>
	/// <typeparam name="T">Cell template type (bool)</typeparam>
	/// <param name="Cell">Cell to work with</param>
	/// <returns>Down right coordinate as FVecor2D (maximum X and Y)</returns>
	template<class T>
	FORCEINLINE FVector2D GetXYMaxOfGridCell(const FGridCell<T>& Cell) const { return FVector2D(Cell.Column + 1, Cell.Row + 1) * CellSideLength; }

private:
	
	void PaveVolumeWithGrid();
	
};