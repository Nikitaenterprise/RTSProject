#include "UI/ResourceAdditionalDescriptionWidget.h"

#include "Actors/Units/BaseUnitWithAbility.h"
#include "Components/TextBlock.h"
#include "GAS/ResourceAttributeSet.h"

void UResourceAdditionalDescriptionWidget::BindAttributes()
{
	Super::BindAttributes();

	auto* ResourceSet = Cast<UResourceAttributeSet>(AbilitySystemComponent->GetSet<UResourceAttributeSet>());
	if (ResourceSet == nullptr)
	{
		return;
	}

	const auto ResourceAttribute = ResourceSet->GetResourceAmountAttribute();
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(ResourceAttribute).AddUObject(this, &ThisClass::OnAttributeChanged);

	ResourceCapacity->SetText(FText::FromString(FString::SanitizeFloat(ResourceSet->GetResourceAmount())));
}

void UResourceAdditionalDescriptionWidget::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
	ResourceCapacity->SetText(FText::FromString(FString::SanitizeFloat(Data.NewValue)));
}
