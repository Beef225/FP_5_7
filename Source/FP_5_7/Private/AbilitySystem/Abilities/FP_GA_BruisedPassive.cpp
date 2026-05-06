// Copyright JG

#include "AbilitySystem/Abilities/FP_GA_BruisedPassive.h"

#include "AbilitySystemComponent.h"
#include "FP_GameplayTags.h"
#include "AbilitySystem/FP_AttributeSet.h"

FGameplayTag UFP_GA_BruisedPassive::GetStateTag() const
{
	return FFP_GameplayTags::Get().State_Bruised;
}

FGameplayTag UFP_GA_BruisedPassive::GetBuildupEventTag() const
{
	return FFP_GameplayTags::Get().GameplayEvent_Debuff_Buildup_Bruised;
}

FGameplayAttribute UFP_GA_BruisedPassive::GetThresholdAttribute() const
{
	return UFP_AttributeSet::GetBruisedThresholdAttribute();
}

FGameplayAttribute UFP_GA_BruisedPassive::GetDurationAttribute() const
{
	return UFP_AttributeSet::GetBruisedDurationAttribute();
}

void UFP_GA_BruisedPassive::OnDebuffApplied(UAbilitySystemComponent* TargetASC,
                                              UAbilitySystemComponent* SourceASC,
                                              float Duration)
{
	float Effectiveness = 1.f;
	if (SourceASC && SourceASC->GetSet<UFP_AttributeSet>())
	{
		const float Val = SourceASC->GetNumericAttribute(UFP_AttributeSet::GetBruisedEffectivenessAttribute());
		if (Val > 0.f) { Effectiveness = Val; }
	}

	ApplyDebuffEffect(TargetASC, {
		{ UFP_AttributeSet::GetIncreasedDamageAttribute(), -0.1f * Effectiveness }
	});
}

void UFP_GA_BruisedPassive::OnDebuffRemoved(UAbilitySystemComponent* TargetASC)
{
	if (TargetASC && DebuffEffectHandle.IsValid())
	{
		TargetASC->RemoveActiveGameplayEffect(DebuffEffectHandle);
		DebuffEffectHandle.Invalidate();
	}
}
