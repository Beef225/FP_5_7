// Copyright JG

#pragma once

#include "CoreMinimal.h"

/**
 * Heat-equilibrium damage: the further a TARGET's Heat is from its own AmbientTemperature
 * (steady state, ~20 "degrees"), the more damage it takes — in either direction, so very cold
 * targets count too.
 *
 *   Multiplier(d) = 1 + MaxBonus * d / (d + HalfSaturationDelta)        d = |TargetHeat - TargetAmbient|
 *
 * With MaxBonus = 4 and HalfSaturationDelta = 240:
 *   d =   0  -> 1.00x        d =  80 -> 2.00x  (e.g. 100 vs 20 ambient)
 *   d = 240  -> 3.00x        d = 720 -> 4.00x        d -> infinity -> 5.00x (never exceeded)
 * Diminishing returns by construction, so pushing a target's heat absurdly high can't run away.
 *
 * The attacker's IncreasedDamagePerHeatFromEquilibrium amplifies the BONUS part only
 * (0.5 = +50% of the heat bonus). Note it also raises the ceiling: 5x becomes 1 + 4 * 1.5 = 7x.
 */
namespace FPHeatDamage
{
	/** Bonus approached as d -> infinity (4 = up to 5x total damage). */
	constexpr float MaxBonus = 4.f;
	/** Distance from steady state at which half of MaxBonus is reached. 240 puts 2x at d = 80. */
	constexpr float HalfSaturationDelta = 240.f;

	inline float Multiplier(float TargetHeat, float TargetAmbient, float AttackerAmplify = 0.f)
	{
		const float Delta = FMath::Abs(TargetHeat - TargetAmbient);
		if (Delta <= KINDA_SMALL_NUMBER)
		{
			return 1.f;
		}
		const float Bonus = MaxBonus * Delta / (Delta + HalfSaturationDelta);
		return 1.f + Bonus * FMath::Max(1.f + AttackerAmplify, 0.f);
	}
}
