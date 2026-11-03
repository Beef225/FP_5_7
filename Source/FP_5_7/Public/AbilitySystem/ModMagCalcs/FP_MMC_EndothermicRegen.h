// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "FP_MMC_EndothermicRegen.generated.h"

/**
 * Endothermic heat regeneration per period (UFP_GE_EndothermicRegen, period 0.1 s).
 * Below the full point (MaxHeatThreshold - EndothermicFullEpsilon) Heat rises by
 * EndothermicRegenPerSecond x MaxHeatThreshold per second, never past the full point.
 * At or above the full point it returns 0: above Max the Newton dissipation takes over.
 */
UCLASS()
class FP_5_7_API UFP_MMC_EndothermicRegen : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UFP_MMC_EndothermicRegen();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition HeatDef;
	FGameplayEffectAttributeCaptureDefinition MaxHeatThresholdDef;
};
