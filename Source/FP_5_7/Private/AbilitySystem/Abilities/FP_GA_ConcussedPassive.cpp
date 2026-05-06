// Copyright JG

#include "AbilitySystem/Abilities/FP_GA_ConcussedPassive.h"

#include "AbilitySystemComponent.h"
#include "FP_GameplayTags.h"
#include "AbilitySystem/FP_AttributeSet.h"

FGameplayTag UFP_GA_ConcussedPassive::GetStateTag() const
{
	return FFP_GameplayTags::Get().State_Concussed;
}

FGameplayTag UFP_GA_ConcussedPassive::GetBuildupEventTag() const
{
	return FFP_GameplayTags::Get().GameplayEvent_Debuff_Buildup_Concussed;
}

FGameplayAttribute UFP_GA_ConcussedPassive::GetThresholdAttribute() const
{
	return UFP_AttributeSet::GetConcussedThresholdAttribute();
}

FGameplayAttribute UFP_GA_ConcussedPassive::GetDurationAttribute() const
{
	return UFP_AttributeSet::GetConcussedDurationAttribute();
}

void UFP_GA_ConcussedPassive::OnDebuffApplied(UAbilitySystemComponent* TargetASC,
                                               UAbilitySystemComponent* SourceASC,
                                               float Duration)
{
	float Effectiveness = 1.f;
	if (SourceASC && SourceASC->GetSet<UFP_AttributeSet>())
	{
		const float Val = SourceASC->GetNumericAttribute(UFP_AttributeSet::GetConcussedEffectivenessAttribute());
		if (Val > 0.f) { Effectiveness = Val; }
	}

	const float Magnitude = -0.05f * Effectiveness;

	ApplyDebuffEffect(TargetASC, {
		{ UFP_AttributeSet::GetMovementSpeedAttribute(), Magnitude },
		{ UFP_AttributeSet::GetSkillSpeedAttribute(),    Magnitude }
	});
}

void UFP_GA_ConcussedPassive::OnDebuffRemoved(UAbilitySystemComponent* TargetASC)
{
	if (TargetASC && DebuffEffectHandle.IsValid())
	{
		TargetASC->RemoveActiveGameplayEffect(DebuffEffectHandle);
		DebuffEffectHandle.Invalidate();
	}
}
