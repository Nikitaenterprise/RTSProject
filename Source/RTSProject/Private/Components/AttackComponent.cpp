#include "Components/AttackComponent.h"
#include "Interfaces/AttackInterface.h"
#include "Actors/Turret.h"

UAttackComponent::UAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UAttackComponent::RequestAttack(const AActor* ActorToAttack)
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, FString::Printf(TEXT("Attacking %s %s"), *ActorToAttack->GetName(), *GetOwner()->GetName()));
	if (IAttackInterface* AttackInterface = Cast<IAttackInterface>(GetOwner()))
	{
		auto Turrets = AttackInterface->GetTurrets();
		for(const auto& Turret : Turrets)
		{
			if (Turret)
			{
				Turret->RequestAttack(ActorToAttack);
			}
		}
	}
}