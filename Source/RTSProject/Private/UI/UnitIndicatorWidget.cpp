// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UnitIndicatorWidget.h"

#include "Components/NamedSlot.h"
#include "UI/HealthShieldWidget.h"
#include "UI/UnitIndicatorAdditionalDescriptionWidget.h"

void UUnitIndicatorWidget::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	HealthShieldWidget->SetVisibility(InVisibility);
	AdditionalContext->SetVisibility(InVisibility);
}

void UUnitIndicatorWidget::SetBaseUnitOwner(ABaseUnit* InOwnerBaseUnit)
{
	OwnerBaseUnit = InOwnerBaseUnit;

	HealthShieldWidget->SetBaseUnitOwner(OwnerBaseUnit);
	AdditionalContext->SetBaseUnitOwner(OwnerBaseUnit);
}

void UUnitIndicatorWidget::SetAdditionalContext(UUnitIndicatorAdditionalDescriptionWidget* InAdditionalContext)
{
	AdditionalContext = InAdditionalContext;
	AdditionalContextSlot->AddChild(AdditionalContext);
}
