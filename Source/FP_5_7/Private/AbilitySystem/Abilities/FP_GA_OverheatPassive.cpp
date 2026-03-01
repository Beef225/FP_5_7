// Copyright JG

#include "AbilitySystem/Abilities/FP_GA_OverheatPassive.h"

#include "AbilitySystemComponent.h"
#include "FP_GameplayTags.h"
#include "AbilitySystem/FP_AttributeSet.h"

void UFP_GA_OverheatPassive::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Poll at the same cadence as the overheat damage GE.
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			OverheatTimerHandle,
			this,
			&UFP_GA_OverheatPassive::OnOverheatTimerTick,
			0.1f,
			true
		);
	}
}

void UFP_GA_OverheatPassive::OnOverheatTimerTick()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) { return; }

	const UFP_AttributeSet* AS = ASC->GetSet<UFP_AttributeSet>();
	if (!AS) { return; }

	const float Heat         = AS->GetHeat();
	const float MaxThreshold = AS->GetMaxHeatThreshold();

	const bool bShouldBeOverheated = Heat > MaxThreshold;

	if (bShouldBeOverheated == bIsOverheated) { return; }

	bIsOverheated = bShouldBeOverheated;

	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();

	if (bIsOverheated)
	{
		ASC->AddLooseGameplayTag(Tags.State_Overheat);
	}
	else
	{
		ASC->RemoveLooseGameplayTag(Tags.State_Overheat);
	}
}
