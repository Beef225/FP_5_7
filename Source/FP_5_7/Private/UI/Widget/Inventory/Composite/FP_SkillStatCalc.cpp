// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_SkillStatCalc.h"
#include "AbilitySystem/Abilities/FP_DamageGameplayAbility.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "AbilitySystem/Data/FP_SkillLevelUpInfo.h"
#include "AbilitySystemComponent.h"
#include "Player/FP_PlayerState.h"

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

namespace
{
	float GetAttr(const UAbilitySystemComponent* ASC, FGameplayAttribute Attr)
	{
		if (!ASC) return 0.f;
		bool bFound = false;
		const float Val = ASC->GetGameplayAttributeValue(Attr, bFound);
		return bFound ? Val : 0.f;
	}

	// Maps the leaf portion of a SkillTagMod tag to a pair of attribute getters.
	struct FDamageTagEntry
	{
		const TCHAR*          TagLeaf;
		FGameplayAttribute  (*IncrGetter)();
		FGameplayAttribute  (*MoreGetter)();
	};

	static const FDamageTagEntry DamageTagTable[] =
	{
		// Weapon types
		{ TEXT("Pistol"),     UFP_AttributeSet::GetIncreasedDamagePistolAttribute,     UFP_AttributeSet::GetMoreDamagePistolAttribute     },
		{ TEXT("Rifle"),      UFP_AttributeSet::GetIncreasedDamageRifleAttribute,      UFP_AttributeSet::GetMoreDamageRifleAttribute      },
		{ TEXT("Shotgun"),    UFP_AttributeSet::GetIncreasedDamageShotgunAttribute,    UFP_AttributeSet::GetMoreDamageShotgunAttribute    },
		{ TEXT("Drone"),      UFP_AttributeSet::GetIncreasedDamageDroneAttribute,      UFP_AttributeSet::GetMoreDamageDroneAttribute      },
		{ TEXT("Axe"),        UFP_AttributeSet::GetIncreasedDamageAxeAttribute,        UFP_AttributeSet::GetMoreDamageAxeAttribute        },
		{ TEXT("Sword"),      UFP_AttributeSet::GetIncreasedDamageSwordAttribute,      UFP_AttributeSet::GetMoreDamageSwordAttribute      },
		{ TEXT("Club"),       UFP_AttributeSet::GetIncreasedDamageClubAttribute,       UFP_AttributeSet::GetMoreDamageClubAttribute       },
		{ TEXT("Unarmed"),    UFP_AttributeSet::GetIncreasedDamageUnarmedAttribute,    UFP_AttributeSet::GetMoreDamageUnarmedAttribute    },
		{ TEXT("Launcher"),   UFP_AttributeSet::GetIncreasedDamageLauncherAttribute,   UFP_AttributeSet::GetMoreDamageLauncherAttribute   },
		{ TEXT("Grenade"),    UFP_AttributeSet::GetIncreasedDamageGrenadeAttribute,    UFP_AttributeSet::GetMoreDamageGrenadeAttribute    },
		{ TEXT("Melee"),      UFP_AttributeSet::GetIncreasedDamageMeleeAttribute,      UFP_AttributeSet::GetMoreDamageMeleeAttribute      },
		{ TEXT("Projectile"), UFP_AttributeSet::GetIncreasedDamageProjectileAttribute, UFP_AttributeSet::GetMoreDamageProjectileAttribute },
		{ TEXT("AoE"),        UFP_AttributeSet::GetIncreasedDamageAoEAttribute,        UFP_AttributeSet::GetMoreDamageAoEAttribute        },
		// Damage element types
		{ TEXT("Physical"),   UFP_AttributeSet::GetIncreasedPhysicalDamageAttribute,   UFP_AttributeSet::GetMorePhysicalDamageAttribute   },
		{ TEXT("Energy"),     UFP_AttributeSet::GetIncreasedEnergyDamageAttribute,     UFP_AttributeSet::GetMoreEnergyDamageAttribute     },
		{ TEXT("Explosive"),  UFP_AttributeSet::GetIncreasedExplosiveDamageAttribute,  UFP_AttributeSet::GetMoreExplosiveDamageAttribute  },
		{ TEXT("Radiation"),  UFP_AttributeSet::GetIncreasedRadiationDamageAttribute,  UFP_AttributeSet::GetMoreRadiationDamageAttribute  },
		{ TEXT("Chemical"),   UFP_AttributeSet::GetIncreasedChemicalDamageAttribute,   UFP_AttributeSet::GetMoreChemicalDamageAttribute   },
	};
}

// ---------------------------------------------------------------------------

namespace FP_SkillStatCalc
{

float CalcAvgDamage(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS)
{
	// Resolve CDO — only UFP_DamageGameplayAbility subclasses have damage curves
	const UFP_DamageGameplayAbility* CDO = Entry.SkillAbility
		? Entry.SkillAbility->GetDefaultObject<UFP_DamageGameplayAbility>()
		: nullptr;
	if (!CDO || CDO->GetDamageTypes().IsEmpty()) return 0.f;

	const UAbilitySystemComponent* ASC = PS ? PS->GetAbilitySystemComponent() : nullptr;

	// Current skill level from XP curve
	const UFP_SkillLevelUpInfo* Curve = Entry.SkillLevelUpInfo;
	const int32 CurrentXP  = PS ? PS->GetSkillXP(Entry.SkillTag) : 0;
	const int32 MaxLevel   = Curve ? FMath::Min(Entry.MaxLevel, Curve->GetMaxLevel()) : Entry.MaxLevel;
	const int32 SkillLevel = FMath::Max(Curve ? FMath::Min(Curve->FindLevelForXP(CurrentXP), MaxLevel) : 1, 1);

	// Sum average (min+max)/2 across all damage types at current skill level
	float EffectiveBase = 0.f;
	for (const auto& Pair : CDO->GetDamageTypes())
	{
		const float Min = Pair.Value.DamageMin.GetValueAtLevel(static_cast<float>(SkillLevel));
		const float Max = Pair.Value.DamageMax.GetValueAtLevel(static_cast<float>(SkillLevel));
		EffectiveBase += (FMath::Min(Min, Max) + FMath::Max(Min, Max)) * 0.5f;
	}
	if (EffectiveBase <= 0.f) return 0.f;

	// Collect tag-driven bonuses (additive Increased, multiplicative More)
	float TotalIncreased = GetAttr(ASC, UFP_AttributeSet::GetIncreasedDamageAttribute());
	float TotalMore      = 1.f + GetAttr(ASC, UFP_AttributeSet::GetMoreDamageAttribute());

	for (const FGameplayTag& Tag : Entry.SkillModifierTags)
	{
		FString Right;
		Tag.GetTagName().ToString().Split(TEXT("."), nullptr, &Right, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		const FName Leaf(*Right);

		for (const FDamageTagEntry& Row : DamageTagTable)
		{
			if (Leaf == Row.TagLeaf)
			{
				TotalIncreased += GetAttr(ASC, Row.IncrGetter());
				TotalMore      *= 1.f + GetAttr(ASC, Row.MoreGetter());
				break;
			}
		}
	}

	float Damage = EffectiveBase * (1.f + TotalIncreased) * TotalMore;

	// 25% dummy resistance
	Damage *= 0.75f;

	// 100-armour formula: (5·D²) / (100 + 5·D)
	if (Damage > 0.f)
		Damage = (5.f * Damage * Damage) / (100.f + 5.f * Damage);

	// Expected crit multiplier (no crit resistance on dummy)
	const float CritChance = FMath::Clamp(
		GetAttr(ASC, UFP_AttributeSet::GetCriticalStrikeChanceAttribute())
		* (1.f + GetAttr(ASC, UFP_AttributeSet::GetIncreasedCriticalStrikeChanceAttribute())),
		0.f, 1.f);
	const float CritMultiplier = FMath::Max(GetAttr(ASC, UFP_AttributeSet::GetCriticalStrikeMultiplierAttribute()), 1.f);
	Damage *= 1.f + CritChance * (CritMultiplier - 1.f);

	return Damage;
}

float CalcEffectiveSpeed(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS)
{
	if (Entry.BaseSkillSpeed <= 0.f) return 0.f;

	const UAbilitySystemComponent* ASC = PS ? PS->GetAbilitySystemComponent() : nullptr;
	float PlayerSpeed = GetAttr(ASC, UFP_AttributeSet::GetSkillSpeedAttribute());
	if (FMath::IsNearlyZero(PlayerSpeed)) PlayerSpeed = 1.f;

	return Entry.BaseSkillSpeed * PlayerSpeed;
}

float CalcDPS(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS)
{
	const float Damage = CalcAvgDamage(Entry, PS);
	const float Speed  = CalcEffectiveSpeed(Entry, PS);
	return Damage * Speed;
}

} // namespace FP_SkillStatCalc
