﻿#include "UI/MouseCursorWidget.h"

#include "Components/Image.h"
#include "GAS/EntitlementUIData.h"

void UMouseCursorWidget::SetCursorToDefault() const
{
	CursorImage->Brush = DefaultCursor;
}

void UMouseCursorWidget::SetCursorFromAbility(UEntitlementUIData* UIData) const
{
	if (UIData == nullptr)
	{
		return;
	}

	CursorImage->Brush = UIData->Icon;
}

void UMouseCursorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetCursorToDefault();
}