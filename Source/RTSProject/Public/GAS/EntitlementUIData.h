#pragma once

#include "Engine/DataAsset.h"
#include "EntitlementUIData.generated.h"


UCLASS(BlueprintType)
class RTSPROJECT_API UEntitlementUIData : public UDataAsset
{
	GENERATED_BODY()

public:
	/* Used to show the entitlement as icon and in very small places */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	FSlateBrush Icon;

#if WITH_EDITOR
	void Validate(const UEntitlementBase& Parent, TArray<FString>& ValidationErrors) const;
	static void ValidateBrushMatchesTextureSize(const FString& Name, const FSlateBrush& Brush, TArray<FString>& ValidationErrors);
	static void ValidateBrushIsSpecifiedSize(const FString& Name, const FSlateBrush& Brush, int Width, int Height, TArray<FString>& ValidationErrors);
#endif
};
