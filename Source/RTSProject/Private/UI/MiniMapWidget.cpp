#include "UI/MiniMapWidget.h"

#include "Actors/Camera.h"
#include "Actors/FogOfWar.h"
#include "Volumes/FogOfWarBoundsVolume.h"
#include "Components/MiniMapIconComponent.h"
#include "Components/MiniMapInfluencerComponent.h"
#include "Core/RTSPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UMiniMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ARTSPlayerController* TestPlayerController = Cast<ARTSPlayerController>(GetOwningPlayer());
	if (!IsValid(TestPlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("TestPlayerController is nullptr in UMiniMapWidget::NativeConstruct()"));
		return;
	}
	PlayerController = TestPlayerController;

	if (!IsValid(PlayerController->GetFogOfWar()))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController->FogOfWar is nullptr in UMiniMapWidget::NativeConstruct()"));
		return;
	}
	if (!IsValid(PlayerController->GetFogOfWar()->FOWBoundsVolume))
	{
		UE_LOG(LogTemp, Error, TEXT("FOWBoundsVolume is nullptr in UMiniMapWidget::NativeConstruct()"));
		return;
	}
	FOWBoundsVolume = PlayerController->GetFogOfWar()->FOWBoundsVolume;

	FOWTexture = UTexture2D::CreateTransient(PlayerController->GetFogOfWar()->VolumeWidthInCells, PlayerController->GetFogOfWar()->VolumeHeightInCells);
	FOWTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	FOWTexture->AddToRoot();
	FOWTexture->UpdateResource();

	MiniMapUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, PlayerController->GetFogOfWar()->VolumeWidthInCells, PlayerController->GetFogOfWar()->VolumeHeightInCells);
	MiniMapTextureBuffer = new uint8[PlayerController->GetFogOfWar()->VolumeWidthInCells * PlayerController->GetFogOfWar()->VolumeHeightInCells * 4];
	MiniMapMaterialInstance = UMaterialInstanceDynamic::Create(MiniMapMaterial, nullptr);
	MiniMapMaterialInstance->SetTextureParameterValue(FName("FogOfWarMask"), FOWTexture);

	if (!DefaultIconBrush.HasUObject())
	{
		UE_LOG(LogTemp, Error, TEXT("DefaultIconBrush has no UTexture* in UMiniMapWidget::NativeConstruct()"));
		return;
	}

	Clipping = EWidgetClipping::ClipToBounds;
}

void UMiniMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	CleanUpArrays();

	if (IsValid(PlayerController->GetFogOfWar()))
	{
		for (uint32 TileY = 0; TileY < PlayerController->GetFogOfWar()->VolumeHeightInCells; TileY++)
		{
			for (uint32 TileX = 0; TileX < PlayerController->GetFogOfWar()->VolumeWidthInCells; TileX++)
			{
				const int i = TileX + TileY * PlayerController->GetFogOfWar()->VolumeWidthInCells;
				const int Blue = i * 4 + 0;
				const int Green = i * 4 + 1;
				const int Red = i * 4 + 2;
				const int Alpha = i * 4 + 3;
				MiniMapTextureBuffer[Blue] = PlayerController->GetFogOfWar()->FOWTextureBuffer[Blue];
				MiniMapTextureBuffer[Green] = PlayerController->GetFogOfWar()->FOWTextureBuffer[Green];
				MiniMapTextureBuffer[Red] = PlayerController->GetFogOfWar()->FOWTextureBuffer[Red];
				MiniMapTextureBuffer[Alpha] = PlayerController->GetFogOfWar()->FOWTextureBuffer[Alpha];
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController->FogOfWar is nullptr in UMiniMapWidget::NativeTick()"));
	}
	FOWTexture->UpdateTextureRegions(0, 1, MiniMapUpdateTextureRegion, PlayerController->GetFogOfWar()->VolumeWidthInCells * 4, static_cast<uint8>(4), MiniMapTextureBuffer);
}

int32 UMiniMapWidget::NativePaint(const FPaintArgs& MovieSceneBlends, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	LayerId = Super::NativePaint(MovieSceneBlends, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	Context.MaxLayer++;

	DrawIcons(Context);
	DrawCameraTrapezoid(Context);

	return FMath::Max(LayerId, Context.MaxLayer);
}

FReply UMiniMapWidget::NativeOnMouseButtonDown(const FGeometry& MovieSceneBlends, const FPointerEvent& InMouseEvent)
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::White, TEXT("UMiniMapWidget::NativeOnMouseButtonDown"));
	// If only one button is pressed and it's a LMB
	if (InMouseEvent.GetPressedButtons().Num() == 1 && InMouseEvent.GetPressedButtons().Contains(EKeys::LeftMouseButton))
	{
		bLMBPressed = true;
	}
	/*FVector2D ClickLocation = MovieSceneBlends.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	ClickLocation = ClickLocation / GetCachedGeometry().GetLocalSize();
	ClickLocation = (ClickLocation - 0.5) * FOWBoundsVolume->GetVolumeWidth();
	const FVector NewLocation = FVector(ClickLocation, PlayerController->CameraRef->GetActorLocation().Z);
	PlayerController->CameraRef->SetActorLocation(NewLocation);*/
	return FReply::Handled();
	return Super::NativeOnMouseButtonDown(MovieSceneBlends, InMouseEvent);
}

FReply UMiniMapWidget::NativeOnMouseMove(const FGeometry& MovieSceneBlends, const FPointerEvent& InMouseEvent)
{
	if (bLMBPressed)
	{
		FVector2D HoldLocation = MovieSceneBlends.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		HoldLocation = HoldLocation / GetCachedGeometry().GetLocalSize();
		HoldLocation = (HoldLocation - 0.5) * FOWBoundsVolume->GetVolumeWidth();
		const FVector NewLocation = FVector(HoldLocation, PlayerController->GetCamera()->GetActorLocation().Z);
		PlayerController->GetCamera()->SetActorLocation(NewLocation);
	}
	return Super::NativeOnMouseMove(MovieSceneBlends, InMouseEvent);
}

FReply UMiniMapWidget::NativeOnMouseButtonUp(const FGeometry& MovieSceneBlends, const FPointerEvent& InMouseEvent)
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::White, TEXT("UMiniMapWidget::NativeOnMouseButtonUp"));
	if (bLMBPressed)
	{
		bLMBPressed = false;
	}
	return FReply::Handled();
	return Super::NativeOnMouseButtonUp(MovieSceneBlends, InMouseEvent);
}

void UMiniMapWidget::DrawIcons(const FPaintContext& Context) const
{
	for (const auto& Icon : Icons)
	{
		const FVector ActorLocation = Icon.Key->GetActorLocation();
		FVector2D MiniMap = WorldToMiniMap(ActorLocation);
		// Move up and left to center icon
		MiniMap -= Icon.Value.ImageSize / 2;

		// Get angle to rotate icon
		const float Rotation = FMath::DegreesToRadians(Icon.Key->GetActorRotation().Yaw);
		// Set Geometry to draw icon on minimap (includes translation and rotation of icon)
		FGeometry Geometry = Context.AllottedGeometry.MakeChild(
			Icon.Value.ImageSize,
			FSlateLayoutTransform(),
			FSlateRenderTransform(FQuat2D(Rotation), MiniMap),
			FVector2D(0.5,0.5)
		);

		// Draw icon on minimap
		FSlateDrawElement::MakeBox(
			Context.OutDrawElements,
			Context.MaxLayer,
			Geometry.ToPaintGeometry(),
			&Icon.Value,
			ESlateDrawEffect::None,
			Icon.Value.TintColor.GetSpecifiedColor());
	}
}

void UMiniMapWidget::DrawCameraTrapezoid(const FPaintContext& Context) const
{
	int32 ViewportWidth;
	int32 ViewportHeight;
	if (!PlayerController)
	{
		return;
	}
	PlayerController->GetViewportSize(ViewportWidth, ViewportHeight);

	// Cast four rays.
	const FVector2D ViewportTopLeft(0, 0);
	const FVector2D ViewportTopRight(ViewportWidth, 0);
	const FVector2D ViewportBottomLeft(0, ViewportHeight);
	const FVector2D ViewportBottomRight(ViewportWidth, ViewportHeight);

	const FVector WorldTopLeft = ViewportToWorld(ViewportTopLeft);
	const FVector WorldTopRight = ViewportToWorld(ViewportTopRight);
	const FVector WorldBottomLeft = ViewportToWorld(ViewportBottomLeft);
	const FVector WorldBottomRight = ViewportToWorld(ViewportBottomRight);

	// Convert to minimap space.
	const FVector2D MiniMapTopLeft = WorldToMiniMap(WorldTopLeft);
	const FVector2D MiniMapTopRight = WorldToMiniMap(WorldTopRight);
	const FVector2D MiniMapBottomLeft = WorldToMiniMap(WorldBottomLeft);
	const FVector2D MiniMapBottomRight = WorldToMiniMap(WorldBottomRight);

	// Draw camera trapezoid
	TArray<FVector2D> Points;

	Points.Add(MiniMapTopLeft);
	Points.Add(MiniMapTopRight);
	Points.Add(MiniMapBottomRight);
	Points.Add(MiniMapBottomLeft);
	Points.Add(MiniMapTopLeft);

	FSlateDrawElement::MakeLines(
		Context.OutDrawElements,
		Context.MaxLayer,
		Context.AllottedGeometry.ToPaintGeometry(),
		Points);
}

void UMiniMapWidget::RegisterActor(AActor* ActorToRegister)
{
	if (!ActorToRegister)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor is unregistered in UMiniMapWidget because it's nullptr"));
		return;
	}

	UMiniMapInfluencerComponent* MiniMapInfluencerComponent = Cast<UMiniMapInfluencerComponent>(ActorToRegister->FindComponentByClass(UMiniMapInfluencerComponent::StaticClass()));
	if (!MiniMapInfluencerComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor %s is unregistered in UMiniMapWidget because it hasn't UMiniMapInfluencerComponent"), *ActorToRegister->GetHumanReadableName());
		return;
	}
	MiniMapInfluencers.AddUnique(ActorToRegister);

	UMiniMapIconComponent* MiniMapIconComponent = Cast<UMiniMapIconComponent>(ActorToRegister->FindComponentByClass(UMiniMapIconComponent::StaticClass()));
	if (!MiniMapIconComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor %s has no UMiniMapIconComponent"), *ActorToRegister->GetHumanReadableName());
		return;
	}

	/*if (!MiniMapIconComponent->IconBrush.HasUObject())
	{
		UE_LOG(LogTemp, Error, TEXT("Actor %s has no Icon texture. Will set to default"), *ActorToRegister->GetHumanReadableName());
		MiniMapIconComponent->IconBrush = DefaultIconBrush;
	}*/
	Icons.AddUnique(TPair<AActor*, FSlateBrush&>(ActorToRegister, MiniMapIconComponent->IconBrush));
	
	UE_LOG(LogTemp, Log, TEXT("Actor %s registered in UMiniMapWidget"), *ActorToRegister->GetHumanReadableName());
}

void UMiniMapWidget::UnRegisterActor(AActor* ActorToUnregister)
{
	if (!ActorToUnregister)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor is nullptr in UMiniMapWidget::UnRegisterActor"));
		return;
	}

	int32 IndexToDelete = MiniMapInfluencers.Find(ActorToUnregister);
	if (IndexToDelete == -1)
	{
		UE_LOG(LogTemp, Error, TEXT("No such actor in MiniMapInfluencers array in UMiniMapWidget::UnRegisterActor"));
		return;
	}
	MiniMapInfluencers.RemoveAt(IndexToDelete);

	for (int32 Index = 0; Index < Icons.Num(); Index++)
	{
		if (Icons[Index].Key == ActorToUnregister)
		{
			IndexToDelete = Index;
		}
	}
	if (IndexToDelete == -1)
	{
		UE_LOG(LogTemp, Error, TEXT("No such actor in Icons array in UMiniMapWidget::UnRegisterActor"));
		return;
	}
	Icons.RemoveAt(IndexToDelete);
	UE_LOG(LogTemp, Log, TEXT("Actor %s unregistered from UMiniMapWidget"), *ActorToUnregister->GetHumanReadableName());
}

FVector2D UMiniMapWidget::WorldToMiniMap(const FVector& Location) const
{
	// Get relative world position
	const float RelativeWorldX = Location.X / FOWBoundsVolume->GetVolumeWidth() + 0.5f;
	const float RelativeWorldY = Location.Y / FOWBoundsVolume->GetVolumeHeight() + 0.5f;

	// Convert to minimap coordinates.
	const float MiniMapX = RelativeWorldX * GetCachedGeometry().GetLocalSize().X;
	const float MiniMapY = RelativeWorldY * GetCachedGeometry().GetLocalSize().Y;

	return FVector2D(MiniMapX, MiniMapY);
}

FVector UMiniMapWidget::ViewportToWorld(const FVector2D& ViewportPosition) const
{
	// Get ray
	FVector WorldOrigin;
	FVector WorldDirection;
	if (!UGameplayStatics::DeprojectScreenToWorld(PlayerController, ViewportPosition, WorldOrigin, WorldDirection))
	{
		return FVector(0, 0, 0);
	}

	// Make plane
	FPlane ZPlane = FPlane(FVector::ZeroVector, FVector::UpVector);

	// Calculate intersection point
	return FMath::LinePlaneIntersection(WorldOrigin, WorldOrigin + WorldDirection * 1000.0f, ZPlane);
}

void UMiniMapWidget::CleanUpArrays()
{
	int32 DeleteIndex = -1;
	for (int32 Index = 0; Index < Icons.Num(); Index++)
	{
		// Check if actor is alive
		if (!IsValid(Icons[Index].Key))
		{
			DeleteIndex = Index;
		}
	}
	if (Icons.IsValidIndex(DeleteIndex))
	{
		Icons.RemoveAt(DeleteIndex);
	}

	DeleteIndex = -1;
	for (int32 Index = 0; Index < MiniMapInfluencers.Num(); Index++)
	{
		// Check if actor is alive
		if (!IsValid(MiniMapInfluencers[Index]))
		{
			DeleteIndex = Index;
		}
	}
	if (MiniMapInfluencers.IsValidIndex(DeleteIndex))
	{
		MiniMapInfluencers.RemoveAt(DeleteIndex);
	}
}
