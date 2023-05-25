#include "UI/SelectionRectangleWidget.h"

#include "Actors/Buildings/Building.h"
#include "Core/RTSPlayerController.h"
#include "UI/GameHUD.h"

void USelectionRectangleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ARTSPlayerController* TestPlayerController = Cast<ARTSPlayerController>(GetOwningPlayer());
	if (TestPlayerController)
	{
		PlayerController = TestPlayerController;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("TestPlayerController is nullptr in USelectionRectangleWidget::NativeConstruct()"));
	}

	// For NativeOnKeyDown event to work properly the flag IsFocusable should be true in Widget->Designer->Interaction
	bIsFocusable = true;

	Visibility = ESlateVisibility::Visible;
}

void USelectionRectangleWidget::NativeTick(const FGeometry& MovieSceneBlends, float InDeltaTime)
{
	if (bIsLMBPressed)
	{
		const float CurrentTime = GetWorld()->GetTimeSeconds() - StartClickTime;
		if (CurrentTime < HoldTime)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.01, FColor::White, TEXT("Just click"));
			ClearSelection();
			SelectActorUnderCursor();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.01, FColor::White, TEXT("Just drawing"));
			bIsDrawingSelectionRectangle = true;
			UpdateSelection();
			GetActorsInSelectionRectangle<AActor>(StartClick, HoldingLocation, ShouldBeSelected, false);
		}
	}

	Super::NativeTick(MovieSceneBlends, InDeltaTime);
}

int32 USelectionRectangleWidget::NativePaint(const FPaintArgs& MovieSceneBlends, const FGeometry& AllottedGeometry,
                                             const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
                                             const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	LayerId = Super::NativePaint(MovieSceneBlends, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	Context.MaxLayer++;
	
	if (bIsDrawingSelectionRectangle) 
	{
		DrawMarquee(Context);		
	}

	return FMath::Max(LayerId, Context.MaxLayer);
}

FReply USelectionRectangleWidget::NativeOnMouseButtonDown(const FGeometry& MovieSceneBlends,
	const FPointerEvent& InMouseEvent)
{
	auto Reply = Super::NativeOnMouseButtonDown(MovieSceneBlends, InMouseEvent);
	if (Reply.IsEventHandled())
	{
		return Reply;
	}
	
	if (bIsSelectionShouldBeBlocked)
	{
		return FReply::Unhandled();
	}
	
	// If only one button is pressed and it's a LMB
	if (InMouseEvent.GetPressedButtons().Num() == 1 && InMouseEvent.GetPressedButtons().Contains(EKeys::LeftMouseButton))
	{
		StartRectangleSelection(MovieSceneBlends, InMouseEvent);
		return FReply::Handled();
	}

	
	return FReply::Unhandled();
}

FReply USelectionRectangleWidget::NativeOnMouseMove(const FGeometry& MovieSceneBlends, const FPointerEvent& InMouseEvent)
{
	auto Reply = Super::NativeOnMouseMove(MovieSceneBlends, InMouseEvent);
	if (Reply.IsEventHandled())
	{
		return Reply;
	}

	if (bIsDrawingSelectionRectangle)
	{
		HoldingLocation = MovieSceneBlends.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		//return FReply::Handled();
	}
	else
	{
		HighlightActorsUnderCursor();
	}

	return FReply::Unhandled();
}

FReply USelectionRectangleWidget::NativeOnMouseButtonUp(const FGeometry& MovieSceneBlends, const FPointerEvent& InMouseEvent)
{
	// At the beginning there was a bug which was as follows:
	// 1. First click on widget fired only NativeOnMouseButtonDown but not NativeOnMouseButtonUp
	// 2. Only on second click NativeOnMouseButtonDown and NativeOnMouseButtonUp fired in turns
	// The solution was to set variable ProjectSettings->Input->ViewportProperties->DefaultViewportCaptureMode 
	// from CapturePermanentlyIncludingInitialMouseDown to CaptureDuringMouseDown  
	// https://answers.unrealengine.com/questions/748898/onclicked-event-only-fires-when-double-clicked-1.html

	auto Reply = Super::NativeOnMouseButtonUp(MovieSceneBlends, InMouseEvent);
	if (Reply.IsEventHandled())
	{
		return Reply;
	}

	if (bIsDrawingSelectionRectangle)
	{
		EndRectangleSelection(MovieSceneBlends, InMouseEvent);
	}

	bIsDrawingSelectionRectangle = false;
	bIsLMBPressed = false;
	StartClickTime = 0;
	return FReply::Handled();
}

FReply USelectionRectangleWidget::NativeOnKeyDown(const FGeometry& MovieSceneBlends, const FKeyEvent& InKeyEvent)
{
	// For this event to work properly the flag IsFocusable should be true in Widget->Designer->Interaction
	auto Reply = Super::NativeOnKeyDown(MovieSceneBlends, InKeyEvent);
	if (Reply.IsEventHandled())
	{
		return Reply;
	}

	if (InKeyEvent.GetKey() == EKeys::LeftShift)
	{
		bIsLeftShiftPressed = true;
	}
	return FReply::Unhandled();
}

FReply USelectionRectangleWidget::NativeOnKeyUp(const FGeometry& MovieSceneBlends, const FKeyEvent& InKeyEvent)
{
	auto Reply = Super::NativeOnKeyUp(MovieSceneBlends, InKeyEvent);
	if (Reply.IsEventHandled())
	{
		return Reply;
	}

	if (InKeyEvent.GetKey() == EKeys::LeftShift)
	{
		bIsLeftShiftPressed = false;
	}
	return FReply::Unhandled();
}

void USelectionRectangleWidget::StartRectangleSelection(const FGeometry& MovieSceneBlends, const FPointerEvent& InMouseEvent)
{
	StartClickTime = GetWorld()->GetTimeSeconds();

	StartClick = MovieSceneBlends.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	HoldingLocation = StartClick;

	// If shift pressed then new selected units will be added to already selected
	// thus SelectedActors shouldn't be emptied 
	if (!bIsLeftShiftPressed)
	{
		SelectedActors.Empty();
		PlayerController->GetSelectedActorsRef().Empty();
	}
	ShouldBeSelected.Empty();

	bIsLMBPressed = true;
}

void USelectionRectangleWidget::EndRectangleSelection(const FGeometry& MovieSceneBlends, const FPointerEvent& InMouseEvent)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr in USelectionRectangleWidget::NativeOnMouseButtonUp"));
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("PlayerController is nullptr in USelectionRectangleWidget::NativeOnMouseButtonUp"));
		return;
	}

	// Adding new actors (ShouldBeSelected) to selection pool (SelectedActors)
	if (ShouldBeSelected.Num() == 0)
	{
		return;
	}
	bool bOnlyBuildings = false, bOnlyShips = false;

	// Pre check fo actors in SelectedActors if shift is pressed
	// e.d. buildings or ships are already in SelectedActors
	if (SelectedActors.Num() != 0)
	{
		if (PlayerController->IsArrayContainThisTypeActors<AShip>(SelectedActors))
		{
			bOnlyShips = true;
		}
		else if (PlayerController->IsArrayContainThisTypeActors<ABuilding>(SelectedActors))
		{
			bOnlyBuildings = true;
		}
	}
	for (const auto& Actor : ShouldBeSelected)
	{
		if (PlayerController->GetPlayersActors().Contains(Actor))
		{
			// If SelectedActors has no actors then add first
			if (SelectedActors.Num() == 0)
			{
				SelectedActors.AddUnique(Actor);
				continue;
			}
			// If SelectedActors has at least one building then add
			// building but not ship
			if (!bOnlyShips && Cast<ABuilding>(Actor))
			{
				SelectedActors.AddUnique(Actor);
				bOnlyBuildings = true;
			}
			// If SelectedActors has at least one ship then add
			// ship but not building
			else if (!bOnlyBuildings && Cast<AShip>(Actor))
			{
				SelectedActors.AddUnique(Actor);
				bOnlyShips = true;
			}
		}
	}
	// Set ShouldBeSelected Execute_Highlighted to false
	// because actors was highlighted while rectangle was drawn
	for (auto& Actor : ShouldBeSelected)
	{
		auto Interface = Cast<ISelectable>(Actor);
		if (Interface) Interface->Execute_Highlighted(Actor, false);
	}
	// Set actors Execute_Selected to true
	for (auto& Actor : SelectedActors)
	{
		auto Interface = Cast<ISelectable>(Actor);
		if (Interface) Interface->Execute_Selected(Actor, true);
	}

	UpdatePlayerControllerSelectedActors();
}

void USelectionRectangleWidget::ClearSelection()
{
	if (!PlayerController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("TestPlayerController is nullptr in USelectionRectangleWidget::ClearSelection"));
		UE_LOG(LogTemp, Error, TEXT("TestPlayerController is nullptr in USelectionRectangleWidget::ClearSelection"));
		return;
	}

	// All actors should be deselected unless shift is pressed
	// in this case SelectedActors won't be deselected
	for (auto& Actor : PlayerController->GetPlayersActors())
	{
		if (!SelectedActors.Contains(Actor))
		{
			auto Interface = Cast<ISelectable>(Actor);
			if (Interface) Interface->Execute_Selected(Actor, false);
		}
	}
}

void USelectionRectangleWidget::UpdateSelection()
{
	ClearSelection();

	if (!PlayerController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("TestPlayerController is nullptr in USelectionRectangleWidget::UpdateSelection"));
		UE_LOG(LogTemp, Error, TEXT("TestPlayerController is nullptr in USelectionRectangleWidget::UpdateSelection"));
		return;
	}

	// Select actors which came from selection rectangle
	for (auto& Actor : ShouldBeSelected)
	{
		auto Interface = Cast<ISelectable>(Actor);
		if (Interface) Interface->Execute_Highlighted(Actor, true);
	}
}

void USelectionRectangleWidget::SelectActorUnderCursor()
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr in USelectionRectangleWidget::SelectActorUnderCursor"));
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("PlayerController is nullptr in USelectionRectangleWidget::SelectActorUnderCursor"));
		return;
	}

	FHitResult Hit;
	const bool bHit = PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Camera), false, Hit);
	if (bHit)
	{
		AActor* Actor = Hit.GetActor();
		if (PlayerController->GetPlayersActors().Contains(Actor))
		{
			ShouldBeSelected.AddUnique(Actor);
			SelectedActors.AddUnique(Actor);
			auto Interface = Cast<ISelectable>(Actor);
			if (Interface) Interface->Execute_Highlighted(Actor, true);
			UpdatePlayerControllerSelectedActors();
		}
	}
}

void USelectionRectangleWidget::HighlightActorsUnderCursor()
{
	// If HighlightedActor is in SelectedActors then
	// it shouldn't be highlighted
	if (SelectedActors.Contains(HighlightedActor))
	{
		return;
	}

	// Dehighlight unit
	auto Interface = Cast<ISelectable>(HighlightedActor);
	if (Interface) Interface->Execute_Highlighted(HighlightedActor, false);

	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr in USelectionRectangleWidget::HighlightActorsUnderCursor"));
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("PlayerController is nullptr in USelectionRectangleWidget::HighlightActorsUnderCursor"));
		return;
	}

	FHitResult Hit;
	const bool bHit = PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Camera), false, Hit);
	if (bHit)
	{
		HighlightedActor = Hit.GetActor();
		// Highlight unit
		Interface = Cast<ISelectable>(HighlightedActor);
		if (Interface) Interface->Execute_Highlighted(HighlightedActor, true);
	}
}

void USelectionRectangleWidget::DrawMarquee(const FPaintContext& Context) const
{
	TArray<FVector2D> Points;
	Points.Add(FVector2D(StartClick.X, StartClick.Y));
	Points.Add(FVector2D(HoldingLocation.X, StartClick.Y));
	Points.Add(FVector2D(HoldingLocation.X, HoldingLocation.Y));
	Points.Add(FVector2D(StartClick.X, HoldingLocation.Y));
	Points.Add(FVector2D(StartClick.X, StartClick.Y));

	constexpr float Thickness = 1;

	FSlateDrawElement::MakeLines(
		Context.OutDrawElements,
		Context.MaxLayer,
		Context.AllottedGeometry.ToPaintGeometry(),
		Points,
		ESlateDrawEffect::None,
		FLinearColor::Blue,
		true,
		Thickness);	
}

TArray<AActor*>& USelectionRectangleWidget::GetSelectedActors()
{
	return SelectedActors;
}

void USelectionRectangleWidget::UpdatePlayerControllerSelectedActors()
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr in USelectionRectangleWidget::HighlightActorsUnderCursor"));
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("PlayerController is nullptr in USelectionRectangleWidget::HighlightActorsUnderCursor"));
		return;
	}
	PlayerController->GetSelectedActorsRef() = SelectedActors;
}
