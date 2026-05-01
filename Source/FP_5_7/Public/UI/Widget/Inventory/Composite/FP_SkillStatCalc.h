// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/FP_SkillLibrary.h"

class AFP_PlayerState;

/**
 * Stateless calculation helpers for skill description display stats.
 *
 * Damage model (dummy target):
 *   35% resistance on all types, 1000 Armour, no dodge/block/crit resistance.
 *
 * Damage pipeline:
 *   EffectiveBase = BaseHitDamage * SkillLevel
 *   → player IncreasedDamage + weapon/element tag bonuses (additive)
 *   → player MoreDamage × weapon/element More bonuses (multiplicative chain)
 *   → 35% resistance
 *   → armour formula: (5·D²) / (1000 + 5·D)
 *   → expected crit: 1 + CritChance·(CritMultiplier – 1)
 */
namespace FP_SkillStatCalc
{
	/** Average damage per activation against the display dummy. Returns 0 if BaseHitDamage == 0. */
	float CalcAvgDamage(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS);

	/** BaseSkillSpeed × player SkillSpeed attribute (defaults to 1× if attribute is zero). */
	float CalcEffectiveSpeed(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS);

	/** CalcAvgDamage × CalcEffectiveSpeed. Returns 0 if no damage or no speed. */
	float CalcDPS(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS);
}
