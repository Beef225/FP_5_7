// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "FP_GE_EndothermicRegen.generated.h"

/**
 * Endothermic mastery (Flag.SkillTree.Endothermic). Infinite, periodic, applied by
 * UFP_AbilitySystemComponent while the flag tag is present: adds UFP_MMC_EndothermicRegen
 * to Heat every RegenPeriod seconds (regenerates towards just under MaxHeatThreshold).
 * Newton cooling (UFP_MMC_HeatDissapation) is paused while Heat <= Max and resumes above it.
 */
UCLASS()
class FP_5_7_API UFP_GE_EndothermicRegen : public UGameplayEffect
{
	GENERATED_BODY()

public:
	/** Must match the MMC's per-period step. Same cadence as the heat dissipation GE. */
	static constexpr float RegenPeriod = 0.1f;

	UFP_GE_EndothermicRegen();
};
