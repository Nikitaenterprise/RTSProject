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
	if (TestPlayerController) PlayerController = TestPlayerController;
	else UE_LOG(LogTemp, Error, TEXT("TestPlayerController is nullptr in UMiniMapWidget::NativeConstruct()"));

	if (PlayerController->FogOfWar->FOWBoundsVolume) FOWBoundsVolume = PlayerController->FogOfWar->FOWBoundsVolume;
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FOWBoundsVolume in UMiniMapWidget::NativeConstruct() is nullptr"));
		return;
	}

	FOWTexture = UTexture2D::CreateTransient(PlayerController->FogOfWar->VolumeWidthInCells, PlayerController->FogOfWar->VolumeHeightInCells);
	FOWTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	FOWTexture->AddToRoot();
	FOWTexture->UpdateResource();

	MiniMapUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, PlayerController->FogOfWar->VolumeWidthInCells, PlayerController->FogOfWar->VolumeHeightInCells);
	MiniMapTextureBuffer = new uint8[PlayerController->FogOfWar->VolumeWidthInCells * PlayerController->FogOfWar->VolumeHeightInCells * 4];
	MiniMapMaterialInstance = UMaterialInstanceDynamic::Create(MiniMapMaterial, nullptr);
	MiniMapMaterialInstance->SetTextureParameterValue(FName("FogOfWarMask"), FOWTexture);

	if (!DefaultIconBrush.HasUObject())
	{
		UE_LOG(LogTemp, Error, TEXT("DefaultIconBrush in UMiniMapWidget::NativeConstruct() has no UTexture*"));
		return;
	}

	Clipping = EWidgetClipping::ClipToBounds;
}

void UMiniMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (PlayerController->FogOfWar)
	{
		for (uint32 TileY = 0; TileY < PlayerController->FogOfWar->VolumeHeightInCells; TileY++)
		{
			for (uint32 TileX = 0; TileX < PlayerController->FogOfWar->VolumeWidthInCells; TileX++)
			{
				const int i = TileX + TileY * PlayerController->FogOfWar->VolumeWidthInCells;
				const int Blue = i * 4 + 0;
				const int Green = i * 4 + 1;
				const int Red = i * 4 + 2;
				const int Alpha = i * 4 + 3;
				MiniMapTextureBuffer[Blue] = PlayerController->FogOfWar->FOWTextureBuffer[Blue];
				MiniMapTextureBuffer[Green] = PlayerController->FogOfWar->FOWTextureBuffer[Green];
				MiniMapTextureBuffer[Red] = PlayerController->FogOfWar->FOWTextureBuffer[Red];
				MiniMapTextureBuffer[Alpha] = PlayerController->FogOfWar->FOWTextureBuffer[Alpha];
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController->FogOfWar in UMiniMapWidget::NativeTick() is nullptr"));
	}

	FOWTexture->UpdateTextureRegions(0, 1, MiniMapUpdateTextureRegion, PlayerController->FogOfWar->VolumeWidthInCells * 4, static_cast<uint8>(4), MiniMapTextureBuffer);
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
	FVector2D ClickLocation = MovieSceneBlends.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	ClickLocation = ClickLocation / GetCachedGeometry().GetLocalSize();
	ClickLocation = (ClickLocation - 0.5) * FOWBoundsVolume->GetVolumeWidth();
	const FVector NewLocation = FVector(ClickLocation, PlayerController->CameraRef->GetActorLocation().Z);
	PlayerController->CameraRef->SetActorLocation(NewLocation);
	
	return Super::NativeOnMouseButtonDown(MovieSceneBlends, InMouseEvent);
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
		UE_LOG(LogTemp, Error, TEXT("Actor %s is unregistered in UMiniMapWidget because he hasn't UMiniMapInfluencerComponent"), *ActorToRegister->GetHumanReadableName());
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