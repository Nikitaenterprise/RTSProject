#include "GAS/EntitlementBase.h"

#include "GameplayTagContainer.h"
#include "GAS/EntitlementUIData.h"
#include "Misc/DataValidation.h"

#if WITH_EDITOR
// right now all code is just for validation so its all in a #if WITH_EDITOR
EDataValidationResult UEntitlementBase::IsDataValid(FDataValidationContext& Context) const
{
	// Assets are validated on-save, and freshly created assets may not yet have sub-objects created. Make sure
	// they are created by calling PostLoad() here.
	//PostLoad();

	TArray<FString> Errors;
	auto bOk = Super::IsDataValid(Context) != EDataValidationResult::Invalid;

	// other checks: will add to Errors if there is a problem
	ValidateUIData(Errors);

	if (Errors.Num() > 0)
	{
		bOk = false;
		// name the object as its hard to find individual entitlements
		Context.AddError(FText::FromString(FString::Printf(TEXT("UEntitlementBase %s failed validation"), *GetName())));
		for (const auto& Err : Errors)
		{
			Context.AddError(FText::FromString(Err));
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
		const bool bIsWarning = true;

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

#endif // WITH_EDITOR
