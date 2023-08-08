#include "GAS/EntitlementUIData.h"

#include "GAS/EntitlementBase.h"

#if WITH_EDITOR
void UEntitlementUIData::Validate(const UEntitlementBase& Parent, TArray<FString>& ValidationErrors) const
{
	const auto Name = GetName();
	// check the brush size and the texture size are the same
	ValidateBrushMatchesTextureSize(FString::Printf(TEXT("%s:Icon"), *Name), Icon, ValidationErrors);
}

void UEntitlementUIData::ValidateBrushMatchesTextureSize(const FString& Name, const FSlateBrush& Brush, TArray<FString>& ValidationErrors)
{
	const auto* ResourceObject = Brush.GetResourceObject();
	if (ResourceObject == nullptr)
	{
		ValidationErrors.Add(FString::Printf(TEXT("%s has no data"), *Name));
		return;
	}
	
	if(ResourceObject->IsA<UMaterialInterface>())
	{
		// Materials do not have a texture size!
		return;	
	}

	if (const auto* Texture = Cast<UTexture>(ResourceObject))
	{
		const auto BrushSize = Brush.GetImageSize();
		const auto BrushW = FMath::RoundToInt(BrushSize.X);
		const auto BrushH = FMath::RoundToInt(BrushSize.Y);
	
		const auto SourceW = Texture->Source.GetSizeX();
		const auto SourceH = Texture->Source.GetSizeY();
		if (SourceW != BrushW || SourceH != BrushH)
		{
			// rather than reporting brush size not matching as an error (which breaks the data-build), reporting as a warning
			UE_LOG(LogTemp, Warning, TEXT("%s brush size (%d x %d) does not match texture size (%d x %d)"), *Name, BrushW, BrushH, SourceW, SourceH);			
		}
	}
	else
	{
		ValidationErrors.Add(FString::Printf(TEXT("%s is not a texture or material"), *Name));
	}
}

void UEntitlementUIData::ValidateBrushIsSpecifiedSize(const FString& Name, const FSlateBrush& Brush, int Width, int Height, TArray<FString>& ValidationErrors)
{
	if (Brush.HasUObject() == false)
	{
		ValidationErrors.Add(FString::Printf(TEXT("%s has no data"), *Name));
		return;
	}

	const auto BrushSize = Brush.GetImageSize();
	const auto BrushW = FMath::RoundToInt(BrushSize.X);
	const auto BrushH = FMath::RoundToInt(BrushSize.Y);
	if (BrushW != Width || BrushH != Height)
	{
		// not making incorrect size a failure, just a warning
		UE_LOG(LogTemp, Warning, TEXT("%s brush (%d x %d) does not match required size (%d x %d)"), *Name, BrushW, BrushH, Width, Height);
	}
}

#endif