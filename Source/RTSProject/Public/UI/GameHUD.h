#pragma once

#include "CoreMinimal.h"

#include "EngineUtils.h"
#include "Actors/Ship.h"
#include "GameFramework/HUD.h"
#include "Kismet/KismetSystemLibrary.h"

#include "GameHUD.generated.h"

class ARTSPlayerController;
class UMiniMapWidget;

UCLASS()
class RTSPROJECT_API AGameHUD : public AHUD
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ARTSPlayerController* PlayerController = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UMiniMapWidget> MiniMapWidgetClass;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UMiniMapWidget* MiniMapWidget = nullptr;
	
private:

	FVector2D StartClick;
	FVector2D HoldingLocation;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> SelectedActors;
	
	bool bIsDrawingSelectionRectangle = false;

public:

	AGameHUD(const FObjectInitializer& OI);
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;
	virtual void Tick(float DeltaTime) override;

	// Selection rectangle
	void OnInputStart();
	void OnInputHold();
	void OnInputRelease();
	void DrawMarquee();
	void DrawSelectionRectAndSelectActors();
	TArray<AActor*>& GetSelectedActors();

	template<class T>
	bool SetupWidget(T*& HUDReference, TSubclassOf<T>& WidgetTemplate);

private:
	
	template <class T>
	bool GetActorsInSelectionRectangle(const FVector2D& FirstPoint, const FVector2D& SecondPoint, TArray<T*>& OutActors, bool bActorMustBeFullyEnclosed = false);

};

template<class T>
inline bool AGameHUD::SetupWidget(T*& HUDReference, TSubclassOf<T>& WidgetTemplate)
{
	if (WidgetTemplate)	{
		HUDReference = CreateWidget<T>(GetWorld(), WidgetTemplate);
		if (HUDReference) return true;
		return false;
	}
	return false;
}

template <class T>
bool AGameHUD::GetActorsInSelectionRectangle(const FVector2D& FirstPoint, 
									const FVector2D& SecondPoint,
									TArray<T*>& OutActors,
									bool bActorMustBeFullyEnclosed)
{
	//Is Actor subclass?
	if (!T::StaticClass()->IsChildOf(AActor::StaticClass()))
	{
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

	//~~~

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
			const FVector ProjectedWorldLocation = Project(BoxCenter + (BoundsPointMapping[BoundsPointItr] * BoxExtents));
			// Add to 2D bounding box
			ActorBox2D += FVector2D(ProjectedWorldLocation.X, ProjectedWorldLocation.Y);
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