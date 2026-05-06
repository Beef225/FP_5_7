// Copyright JG

#include "AbilitySystem/Abilities/FP_GA_CorrodedPassive.h"

#include "AbilitySystemComponent.h"
#include "FP_GameplayTags.h"
#include "AbilitySystem/FP_AttributeSet.h"

FGameplayTag UFP_GA_CorrodedPassive::GetStateTag() const
{
	return FFP_GameplayTags::Get().State_Corroded;
}

FGameplayTag UFP_GA_CorrodedPassive::GetBuildupEventTag() const
{
	return FFP_GameplayTags::Get().GameplayEvent_Debuff_Buildup_Corroded;
}

FGameplayAttribute UFP_GA_CorrodedPassive::GetThresholdAttribute() const
{
	return UFP_AttributeSet::GetCorrodedThresholdAttribute();
}

FGameplayAttribute UFP_GA_CorrodedPassive::GetDurationAttribute() const
{
	return UFP_AttributeSet::GetCorrodedDurationAttribute();
}

void UFP_GA_CorrodedPassive::OnDebuffApplied(UAbilitySystemComponent* TargetASC,
                                              UAbilitySystemComponent* SourceASC,
                                              float Duration)
{
	float Effectiveness = 1.f;
	if (SourceASC && SourceASC->GetSet<UFP_AttributeSet>())
	{
		const float Val = SourceASC->GetNumericAttribute(UFP_AttributeSet::GetCorrodedEffectivenessAttribute());
		if (Val > 0.f) { Effectiveness = Val; }
	}

	ApplyDebuffEffect(TargetASC, {
		{ UFP_AttributeSet::GetIncreasedArmourAttribute(), -0.1f * Effectiveness }
	});
}

void UFP_GA_CorrodedPassive::OnDebuffRemoved(UAbilitySystemComponent* TargetASC)
{
	if (TargetASC && DebuffEffectHandle.IsValid())
	{
		TargetASC->RemoveActiveGameplayEffect(DebuffEffectHandle);
		DebuffEffectHandle.Invalidate();
	}
}
