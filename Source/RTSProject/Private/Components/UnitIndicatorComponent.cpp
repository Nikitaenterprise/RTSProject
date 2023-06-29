#include "Components/UnitIndicatorComponent.h"

#include "Actors/Units/BaseUnit.h"
#include "Actors/Units/BaseUnitWithAbility.h"
#include "GAS/HealthShieldAttributeSet.h"
#include "UI/UnitIndicatorAdditionalDescriptionWidget.h"
#include "UI/UnitIndicatorWidget.h"


UUnitIndicatorComponent::UUnitIndicatorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UUnitIndicatorComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InitWidget();
	SetWidgetSpace(EWidgetSpace::Screen);
	UnitIndicatorWidget = Cast<UUnitIndicatorWidget>(GetWidget());
	if (IsValid(UnitIndicatorWidget) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("UUnitIndicatorComponent::BeginPlay() UnitIndicatorWidget is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("UUnitIndicatorComponent::BeginPlay() UnitIndicatorWidget is nullptr"));
	}
	SetVisibility(false);
	
	auto* BaseUnit = Cast<ABaseUnit>(GetOwner());
	if (BaseUnit == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("UUnitIndicatorComponent::BeginPlay() BaseUnit is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("UUnitIndicatorComponent::BeginPlay() BaseUnit is nullptr"));
		return;
	}

	if (AdditionalContextWidgetClass.Get())
	{
		AdditionalContextWidget = Cast<UUnitIndicatorAdditionalDescriptionWidget>(CreateWidget(GetWorld(), AdditionalContextWidgetClass));
	}
	else
	{
		// Fill with empty widget
		AdditionalContextWidget = Cast<UUnitIndicatorAdditionalDescriptionWidget>(CreateWidget(GetWorld(), UUnitIndicatorAdditionalDescriptionWidget::StaticClass()));
	}
	UnitIndicatorWidget->SetAdditionalContext(AdditionalContextWidget);
	UnitIndicatorWidget->SetBaseUnitOwner(BaseUnit);
}