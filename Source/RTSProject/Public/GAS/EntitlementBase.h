#pragma once

#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "EntitlementBase.generated.h"

class UEntitlementUIData;

UCLASS(NotBlueprintable)
class RTSPROJECT_API UEntitlementBase : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	FText Name;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSoftObjectPtr<UEntitlementUIData> UIData;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EntitlementID;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	void ValidateUIData(TArray<FString>& ValidationErrors) const;
#endif
};
