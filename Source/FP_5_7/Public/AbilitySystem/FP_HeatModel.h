// Copyright JG

#pragma once

#include "CoreMinimal.h"

/**
 * Thermal model shared by heat dissipation (UFP_MMC_HeatDissapation) and skill heat costs
 * (UFP_GameplayAbility::ComputeSkillCost).
 *
 * Thermal Mass is a heat BUFFER: more mass = heat changes less, both ways.
 *   - Skill heat (heating or cooling, Endothermic spends included) is scaled by MassFactor.
 *   - Newton cooling is scaled by MassFactor too, so more mass also retains heat longer.
 *   MassFactor = ReferenceThermalMass / ThermalMass (1 at the base 40; 80 mass = half the change).
 *
 * Heat Retention (Heat.Retention) divides the cooling rate, the same shape as Heat.SkillCostEfficiency:
 *   CoolingRate = CoolingCoefficient x ReferenceThermalMass x MassFactor / (1 + HeatRetention)
 *   +1.0 = half the cooling rate, +3.0 = a quarter (never reaches 0); -0.5 = double.
 *   Floored at MinHeatRetention (-0.9 = at most 10x) so it can't divide by ~0.
 * Cooling per second = CoolingRate x (Heat - AmbientTemperature).
 * At base stats (coefficient 0.02, mass 40, retention 0) the rate is 0.8/s, unchanged from the old model.
 */
namespace FPHeat
{
	constexpr float ReferenceThermalMass = 40.f;
	constexpr float MinHeatRetention     = -0.9f;

	inline float MassFactor(float ThermalMass)
	{
		return ReferenceThermalMass / FMath::Max(ThermalMass, 1.f);
	}

	inline float RetentionDivisor(float HeatRetention)
	{
		return 1.f + FMath::Max(HeatRetention, MinHeatRetention);
	}

	/** Fraction of (Heat - Ambient) lost per second. */
	inline float CoolingRate(float CoolingCoefficient, float ThermalMass, float HeatRetention)
	{
		return FMath::Max(0.f, CoolingCoefficient) * ReferenceThermalMass * MassFactor(ThermalMass)
			/ RetentionDivisor(HeatRetention);
	}
}
