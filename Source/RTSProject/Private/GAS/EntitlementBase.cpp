#include "GAS/EntitlementBase.h"

#include "GameplayTagContainer.h"
#include "GAS/EntitlementUIData.h"

#if WITH_EDITOR
// right now all code is just for validation so its all in a #if WITH_EDITOR
EDataValidationResult UEntitlementBase::IsDataValid(TArray<FText>& ValidationErrors)
{
	// Assets are validated on-save, and freshly created assets may not yet have sub-objects created. Make sure
	// they are created by calling PostLoad() here.
	PostLoad();

	TArray<FString> Errors;
	auto bOk = Super::IsDataValid(ValidationErrors) != EDataValidationResult::Invalid;

	// other checks: will add to Errors if there is a problem
	ValidateUIData(Errors);

	if (Errors.Num() > 0)
	{
		bOk = false;
		// name the object as its hard to find individual entitlements
		ValidationErrors.Add(FText::FromString(FString::Printf(TEXT("UEntitlementBase %s failed validation"), *GetName())));
		for (const auto& Err : Errors)
		{
			ValidationErrors.Add(FText::FromString(Err));
		}
	}

	return bOk ? EDataValidationResult::Valid : EDataValidationResult::Invalid;
}

void UEntitlementBase::ValidateUIData(TArray<FString>& ValidationErrors) const
{
	if (const auto* UI = UIData.LoadSynchronous())
	{
		UI->Validate(*this, ValidationErrors);		
	}
	else
	{
		// missing UIData might be just a warning or error, depending upon the type 
		bool bIsWarning = true;

		if (bIsWarning)
		{
			UE_LOG(LogTemp, Warning, TEXT("Entitlement %s does not have UIData"), *GetName());
		}
		else
		{
			ValidationErrors.Add(TEXT("No UIData"));
		}
	}
}

void UEntitlementBase::PreSave(const class ITargetPlatform* TargetPlatform)
{
	Super::PreSave(TargetPlatform);
}

#endif // WITH_EDITOR
