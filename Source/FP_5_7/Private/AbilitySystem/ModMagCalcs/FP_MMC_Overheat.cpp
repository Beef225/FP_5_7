// Copyright JG


#include "AbilitySystem/ModMagCalcs/FP_MMC_Overheat.h"

#include "AbilitySystem/FP_AttributeSet.h"

UFP_MMC_Overheat::UFP_MMC_Overheat()
{
	HeatDef.AttributeToCapture = UFP_AttributeSet::GetHeatAttribute();
	HeatDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	HeatDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(HeatDef);

	MaxHeatThresholdDef.AttributeToCapture = UFP_AttributeSet::GetMaxHeatThresholdAttribute();
	MaxHeatThresholdDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	MaxHeatThresholdDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(MaxHeatThresholdDef);

	OverheatDamageResistanceDef.AttributeToCapture = UFP_AttributeSet::GetOverheatDamageResistanceAttribute();
	OverheatDamageResistanceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	OverheatDamageResistanceDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(OverheatDamageResistanceDef);

	OverheatMaxResistanceDef.AttributeToCapture = UFP_AttributeSet::GetOverheatMaxResistanceAttribute();
	OverheatMaxResistanceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	OverheatMaxResistanceDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(OverheatMaxResistanceDef);

	HitPointsDef.AttributeToCapture = UFP_AttributeSet::GetHitPointsAttribute();
	HitPointsDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	HitPointsDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(HitPointsDef);
}

float UFP_MMC_Overheat::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Must match the GE period.
	const float GEPeriod = 0.1f;

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Heat = 0.f;
	GetCapturedAttributeMagnitude(HeatDef, Spec, EvaluationParameters, Heat);

	float MaxHeatThreshold = 0.f;
	GetCapturedAttributeMagnitude(MaxHeatThresholdDef, Spec, EvaluationParameters, MaxHeatThreshold);
	MaxHeatThreshold = FMath::Max(0.f, MaxHeatThreshold);

	float OverheatDamageResistance = 0.f;
	GetCapturedAttributeMagnitude(OverheatDamageResistanceDef, Spec, EvaluationParameters, OverheatDamageResistance);

	float OverheatMaxResistance = 0.f;
	GetCapturedAttributeMagnitude(OverheatMaxResistanceDef, Spec, EvaluationParameters, OverheatMaxResistance);
	OverheatMaxResistance = FMath::Max(0.f, OverheatMaxResistance);

	OverheatDamageResistance = FMath::Min(OverheatDamageResistance, OverheatMaxResistance);

	if (Heat <= MaxHeatThreshold)
	{
		return 0.f;
	}

	if (MaxHeatThreshold <= 0.f)
	{
		MaxHeatThreshold= 0.00001f;
	}

	const float PercentPastMaxHeat = FMath::Max(Heat / MaxHeatThreshold - 1.f, 0.f);
	const float DamageTaken = MaxHeatThreshold * PercentPastMaxHeat * (1.f - OverheatDamageResistance);

	float HitPoints = 0.f;
	GetCapturedAttributeMagnitude(HitPointsDef, Spec, EvaluationParameters, HitPoints);
	(void)HitPoints;

	return DamageTaken * GEPeriod;
}
