// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "FP_MMC_Overheat.generated.h"

/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_MMC_Overheat : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UFP_MMC_Overheat();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:

	FGameplayEffectAttributeCaptureDefinition HeatDef;
	FGameplayEffectAttributeCaptureDefinition MaxHeatThresholdDef;
	FGameplayEffectAttributeCaptureDefinition OverheatDamageResistanceDef;
	FGameplayEffectAttributeCaptureDefinition OverheatMaxResistanceDef;
	FGameplayEffectAttributeCaptureDefinition HitPointsDef;
};
