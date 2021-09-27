#pragma once

#include "CoreMinimal.h"

#include "Actors/Ship.h"
#include "Blueprint/UserWidget.h"
#include "EngineUtils.h"
#include "Core/RTSPlayerController.h"
#include "GameFramework/HUD.h"

#include "DrawDebugHelpers.h"
#include "Actors/Building.h"
#include "Actors/Camera.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "SelectionRectangleWidget.generated.h"


class ARTSPlayerController;

UCLASS()
class RTSPROJECT_API USelectionRectangleWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class AGameHUD;

public:

	UPROPERTY(BlueprintReadOnly)
	ARTSPlayerController* PlayerController = nullptr;

private:

	FVector2D StartClick;
	FVector2D HoldingLocation;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> SelectedActors;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> ShouldBeSelected;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AActor* HighlightedActor = nullptr;

	bool bIsSelectionShouldBeBlocked = false;
	bool bIsDrawingSelectionRectangle = false;
	bool bIsLeftMouseButtonDown = false;
	bool bIsLeftShiftPressed = false;

public:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual int32 NativePaint(const FPaintArgs& Args, 
		const FGeometry& AllottedGeometry, 
		const FSlateRect& MyCullingRect, 
		FSlateWindowElementList& OutDrawElements, 
		int32 LayerId, 
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void UpdateSelection();
	void HighlightActorsUnderCursor();

	void DrawMarquee(const FPaintContext& Context) const;
	TArray<AActor*>& GetSelectedActors();

private:

	template <class T>
	bool GetActorsInSelectionRectangle(const FVector2D& FirstPoint, const FVector2D& SecondPoint, TArray<T*>& OutActors, bool bActorMustBeFullyEnclosed = false);
};

// Function that returns actors in rectangle drawn in viewport (in this widget)
// Initially was taken from AHUD class and reworked in such way that
// it's not needed to call to canvas
template <class T>
bool USelectionRectangleWidget::GetActorsInSelectionRectangle(const FVector2D& FirstPoint,
	const FVector2D& SecondPoint,
	TArray<T*>& OutActors,
	bool bActorMustBeFullyEnclosed)
{
	//Is Actor subclass?
	if (!T::StaticClass()->IsChildOf(AActor::StaticClass()))
	{
		return false;
	}

	if (!PlayerController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("TestPlayerController is nullptr in USelectionRectangleWidget::GetActorsInSelectionRectangle"));
		UE_LOG(LogTemp, Error, TEXT("TestPlayerController is nullptr in USelectionRectangleWidget::GetActorsInSelectionRectangle"));
		return false;
	}

	//Run Inner Function, output to Base AActor Array
	TArray<AActor*> OutActorsBaseArray;
	// Because this is a HUD function it is likely to get called each tick,
	// so make sure any previous contents of the out actor array have been cleared!
	OutActors.Reset();

	//Create Selection Rectangle from Points
	FBox2D SelectionRectangle(ForceInit);

	//This method ensures that an appropriate rectangle is generated, 
	//		no matter what the coordinates of first and second point actually are.
	SelectionRectangle += FirstPoint;
	SelectionRectangle += SecondPoint;


	//The Actor Bounds Point Mapping
	const FVector BoundsPointMapping[8] =
	{
		FVector(1.f, 1.f, 1.f),
		FVector(1.f, 1.f, -1.f),
		FVector(1.f, -1.f, 1.f),
		FVector(1.f, -1.f, -1.f),
		FVector(-1.f, 1.f, 1.f),
		FVector(-1.f, 1.f, -1.f),
		FVector(-1.f, -1.f, 1.f),
		FVector(-1.f, -1.f, -1.f) };

	//For Each Actor of the Class Filter Type
	for (TActorIterator<AActor> Itr(GetWorld(), T::StaticClass()); Itr; ++Itr)
	{
		AActor* EachActor = *Itr;

		//Get Actor Bounds				//casting to base class, checked by template in the .h
		//const FBox EachActorBounds = EachActor->GetComponentsBoundingBox(bIncludeNonCollidingComponents); /* All Components? */
		if (!EachActor->GetRootComponent())
		{
			/*FString str = UKismetSystemLibrary::GetDisplayName(EachActor);
			GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Cyan, str);*/
			continue;
		}
		FBox EachActorBounds = EachActor->GetRootComponent()->Bounds.GetBox();
		// If selected actor is a ship then select not by ROotComp
		// but by static mesh collision box
		AShip* Ship = Cast<AShip>(EachActor);
		if (Ship) EachActorBounds = Ship->StaticMesh->Bounds.GetBox();

		//Center
		const FVector BoxCenter = EachActorBounds.GetCenter();

		//Extents
		const FVector BoxExtents = EachActorBounds.GetExtent();
		
		// Build 2D bounding box of actor in screen space
		FBox2D ActorBox2D(ForceInit);
		for (uint8 BoundsPointItr = 0; BoundsPointItr < 8; BoundsPointItr++)
		{
			// Project vert into screen space.
			FVector2D ScreenPos;
			// There was a bug in selection rectangle because of projecting coordinates from world to screen not relative to widget
			// PlayerController->ProjectWorldLocationToScreen(BoxCenter + (BoundsPointMapping[BoundsPointItr] * BoxExtents), ScreenPos, false);
			UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PlayerController, BoxCenter + (BoundsPointMapping[BoundsPointItr] * BoxExtents), ScreenPos, false);
			// Add to 2D bounding box
			ActorBox2D += ScreenPos;
		}
		//Selection Box must fully enclose the Projected Actor Bounds
		if (bActorMustBeFullyEnclosed)
		{
			if (SelectionRectangle.IsInside(ActorBox2D))
			{
				OutActors.Add(EachActor);
			}
		}
		//Partial Intersection with Projected Actor Bounds
		else
		{
			if (SelectionRectangle.Intersect(ActorBox2D))
			{
				OutActors.Add(EachActor);
			}
		}
	}

	//Construct casted template type array
	for (AActor* EachActor : OutActorsBaseArray)
	{
		OutActors.Add(CastChecked<T>(EachActor));
	}

	return true;
};