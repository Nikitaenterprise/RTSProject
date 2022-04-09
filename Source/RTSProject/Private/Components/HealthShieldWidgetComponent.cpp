#include "Components/HealthShieldWidgetComponent.h"
#include "UI/HealthShieldWidget.h"
#include "GAS/HealthShieldAttributeSet.h"

UHealthShieldWidgetComponent::UHealthShieldWidgetComponent()
{
}

void UHealthShieldWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	InitWidget();
	SetWidgetSpace(EWidgetSpace::Screen);
	HealthShieldWidget = Cast<UHealthShieldWidget>(GetWidget());
	if (!IsValid(HealthShieldWidget))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("HealthShieldWidget in UHealthShieldWidgetComponent::InitializeComponent() is nullptr"));
		UE_LOG(LogTemp, Error, TEXT("HealthShieldWidget in UHealthShieldWidgetComponent::InitializeComponent() is nullptr"));
	}
	SetVisibility(false);
}

void UHealthShieldWidgetComponent::SetHealthShieldAttributeSet(UHealthShieldAttributeSet* HealthShieldAttributeSet)
{
	if (IsValid(HealthShieldWidget))
	{
		HealthShieldWidget->SetHealthShieldAttributeSet(HealthShieldAttributeSet);
		HealthShieldWidget->UpdateHealthShieldValues();
	}
}