// Copyright JG


#include "AbilitySystem/ModMagCalcs/FP_MMC_HeatDissapation.h"
#include "AbilitySystem/FP_AttributeSet.h"

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
	ThermalMass = FMath::Max(0.f, ThermalMass);

	float CoolingCoefficient = 0.f;
	GetCapturedAttributeMagnitude(CoolingCoefficientDef, Spec, EvaluationParameters, CoolingCoefficient);
	CoolingCoefficient = FMath::Max(0.f, CoolingCoefficient);

	float Heat = 0.f;
	GetCapturedAttributeMagnitude(HeatDef, Spec, EvaluationParameters, Heat);

	const float DissipationValue = CoolingCoefficient * ThermalMass * (Heat - AmbientTemperature) * GEPeriod;
	return DissipationValue;
}
