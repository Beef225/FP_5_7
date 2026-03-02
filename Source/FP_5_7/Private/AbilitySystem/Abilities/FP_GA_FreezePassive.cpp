// Copyright JG

#include "AbilitySystem/Abilities/FP_GA_FreezePassive.h"

#include "AbilitySystemComponent.h"
#include "FP_GameplayTags.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "Characters/FP_CharacterBase.h"

void UFP_GA_FreezePassive::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Poll at the same cadence as the heat dissipation GE.
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			FreezeTimerHandle,
			this,
			&UFP_GA_FreezePassive::OnFreezeTimerTick,
			0.1f,
			true
		);
	}
}

void UFP_GA_FreezePassive::OnFreezeTimerTick()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) { return; }

	const UFP_AttributeSet* AS = ASC->GetSet<UFP_AttributeSet>();
	if (!AS) { return; }

	const float Heat         = AS->GetHeat();
	const float MinThreshold = AS->GetMinHeatThreshold();

	// ChillStart is 25% of MinHeatThreshold from 0 (e.g. MinThreshold=-100 -> ChillStart=-25).
	const float ChillStart = MinThreshold * 0.25f;

	// Guard: if the two are identical (e.g. MinThreshold==0) there is no valid range.
	if (FMath::IsNearlyEqual(ChillStart, MinThreshold))
	{
		UpdateFreezeState(0.f);
		return;
	}

	// 0 when Heat >= ChillStart, ramps to 1 when Heat <= MinThreshold.
	const float FreezeRamp = FMath::Clamp(FMath::GetRangePct(ChillStart, MinThreshold, Heat), 0.f, 1.f);

	// Apply ramp to CharacterBase as a final multiplier on movement and skill speed.
	if (AFP_CharacterBase* CharBase = Cast<AFP_CharacterBase>(GetAvatarActorFromActorInfo()))
	{
		CharBase->SetFreezeRamp(FreezeRamp);
	}

	UpdateFreezeState(FreezeRamp);
}

void UFP_GA_FreezePassive::UpdateFreezeState(float FreezeRamp)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) { return; }

	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();

	const bool bShouldBeFrozen  = FreezeRamp >= 1.f;
	const bool bShouldBeChilled = FreezeRamp > 0.f && !bShouldBeFrozen;

	if (bShouldBeFrozen != bIsFrozen)
	{
		bIsFrozen = bShouldBeFrozen;
		if (bIsFrozen)
		{
			ASC->AddLooseGameplayTag(Tags.State_Frozen);
			if (bIsChilled)
			{
				ASC->RemoveLooseGameplayTag(Tags.State_Chilled);
				bIsChilled = false;
			}
		}
		else
		{
			ASC->RemoveLooseGameplayTag(Tags.State_Frozen);
		}
	}

	if (bShouldBeChilled != bIsChilled)
	{
		bIsChilled = bShouldBeChilled;
		if (bIsChilled)
		{
			ASC->AddLooseGameplayTag(Tags.State_Chilled);
		}
		else
		{
			ASC->RemoveLooseGameplayTag(Tags.State_Chilled);
		}
	}
}
