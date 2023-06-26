#include "Actors/Resources/Resource.h"

#include "AbilitySystemComponent.h"
#include "Actors/Resources/ResourceManager.h"
#include "Components/WidgetComponent.h"
#include "Core/RTSGameMode.h"
#include "GAS/ResourceAttributeSet.h"
#include "UI/BasicValueViewer.h"


AResource::AResource()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(GetRootComponent());
	WidgetComponent->SetGeometryMode(EWidgetGeometryMode::Plane);
	WidgetComponent->SetTwoSided(false);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
}

void AResource::Highlighted_Implementation(bool bIsHighlighted)
{
	if (WidgetComponent == nullptr)
	{
		return;
	}

	if (bIsHighlighted)
	{
		if (auto* BasicValueViewerWidget = Cast<UBasicValueViewer>(WidgetComponent->GetWidget()))
		{
			BasicValueViewerWidget->SetText(ResourceAttributeSet->GetResourceAmount());
		}
		WidgetComponent->SetVisibility(true);
	}
	else
	{
		WidgetComponent->SetVisibility(false);
	}
}

void AResource::BeginPlay()
{
	Super::BeginPlay();

	ResourceAttributeSet = Cast<UResourceAttributeSet>(AbilitySystemComponent->InitStats(ResourceAttribute, nullptr));

	OnEmptyResourceDelegate = ResourceAttributeSet->OnResourceEmpty.CreateUObject(this, &ThisClass::OnResourceEmpty);

	WidgetComponent->InitWidget();
	WidgetComponent->SetVisibility(false);

	ResourceManager = ARTSGameMode::GetRTSGameMode(GetWorld())->GetResourceManager();
	if (IsValid(ResourceManager) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AResourceManager is nullptr in UResourceComponent::BeginPlay()"));
		return;
	}
	ResourceManager->AddResource(this);

	SetResourceAmount();
}

void AResource::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (ResourceManager)
	{
		ResourceManager->RemoveResource(this);
	}
}

void AResource::OnResourceEmpty()
{
	Destroy();
}

void AResource::SetResourceAmount() const
{
	UGameplayEffect* GameplayEffect = NewObject<UGameplayEffect>();
	GameplayEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo GameplayModifierInfo;
	GameplayModifierInfo.Attribute = ResourceAttributeSet->GetResourceAmountAttribute();
	GameplayModifierInfo.ModifierOp = EGameplayModOp::Override;
	GameplayModifierInfo.ModifierMagnitude = FScalableFloat(GetResourceAmount());
	GameplayEffect->Modifiers.Add(GameplayModifierInfo);
	AbilitySystemComponent->ApplyGameplayEffectToSelf(GameplayEffect, 0, AbilitySystemComponent->MakeEffectContext());
}

float AResource::GetResourceAmount_Implementation() const
{
	return 1000;
}
