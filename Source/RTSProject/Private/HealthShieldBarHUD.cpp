#include "HealthShieldBarHUD.h"

void UHealthShieldBarHUD::ShowUI_Implementation()
{
	this->SetVisibility(ESlateVisibility::Visible);
}


void UHealthShieldBarHUD::HideUI_Implementation()
{
	this->SetVisibility(ESlateVisibility::Hidden);
}