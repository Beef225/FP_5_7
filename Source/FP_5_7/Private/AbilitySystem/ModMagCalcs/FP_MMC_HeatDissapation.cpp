// Copyright JG


#include "AbilitySystem/ModMagCalcs/FP_MMC_HeatDissapation.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"
#include "AbilitySystem/FP_HeatModel.h"

UFP_MMC_HeatDissapation::UFP_MMC_HeatDissapation()
{
	AmbientTemperatureDef.AttributeToCapture = UFP_AttributeSet::GetAmbientTemperatureAttribute();
	AmbientTemperatureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	AmbientTemperatureDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(AmbientTemperatureDef);

	ThermalMassDef.AttributeToCapture = UFP_AttributeSet::GetThermalMassAttribute();
	ThermalMassDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	ThermalMassDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(ThermalMassDef);

	CoolingCoefficientDef.AttributeToCapture = UFP_AttributeSet::GetCoolingCoefficientAttribute();
	CoolingCoefficientDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	CoolingCoefficientDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(CoolingCoefficientDef);

	HeatDef.AttributeToCapture = UFP_AttributeSet::GetHeatAttribute();
	HeatDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	HeatDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(HeatDef);

	MaxHeatThresholdDef.AttributeToCapture = UFP_AttributeSet::GetMaxHeatThresholdAttribute();
	MaxHeatThresholdDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	MaxHeatThresholdDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(MaxHeatThresholdDef);

	HeatRetentionDef.AttributeToCapture = UFP_AttributeSet::GetHeatRetentionAttribute();
	HeatRetentionDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	HeatRetentionDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(HeatRetentionDef);
}

float UFP_MMC_HeatDissapation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Must match the GE period.
	const float GEPeriod = 0.1f;

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float AmbientTemperature = 0.f;
	GetCapturedAttributeMagnitude(AmbientTemperatureDef, Spec, EvaluationParameters, AmbientTemperature);

	float ThermalMass = 0.f;
	GetCapturedAttributeMagnitude(ThermalMassDef, Spec, EvaluationParameters, ThermalMass);

	float CoolingCoefficient = 0.f;
	GetCapturedAttributeMagnitude(CoolingCoefficientDef, Spec, EvaluationParameters, CoolingCoefficient);

	float Heat = 0.f;
	GetCapturedAttributeMagnitude(HeatDef, Spec, EvaluationParameters, Heat);

	// Endothermic: no Newton cooling at or below Max (UFP_GE_EndothermicRegen holds Heat just under it).
	// Above Max (overheated) cooling runs as normal until Heat is back under Max.
	// Self-applied effect, so the instigator ASC is the owner.
	if (UFP_AbilitySystemComponent::IsEndothermic(Spec.GetContext().GetInstigatorAbilitySystemComponent()))
	{
		float MaxHeat = 0.f;
		GetCapturedAttributeMagnitude(MaxHeatThresholdDef, Spec, EvaluationParameters, MaxHeat);
		if (Heat <= MaxHeat)
		{
			return 0.f;
		}
	}

	float HeatRetention = 0.f;
	GetCapturedAttributeMagnitude(HeatRetentionDef, Spec, EvaluationParameters, HeatRetention);

	// Newton cooling: more Thermal Mass and more Heat Retention both slow it (FP_HeatModel.h).
	const float DissipationValue = FPHeat::CoolingRate(CoolingCoefficient, ThermalMass, HeatRetention)
		* (Heat - AmbientTemperature) * GEPeriod;
	return DissipationValue;
}
