#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Player/FP_PlayerState.h"
#include "AbilitySystem/Data/FP_SkillLibrary.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "AbilitySystemGlobals.h"
#include "FP_GameplayTags.h"
#include "AbilitySystem/FP_HeatModel.h"

namespace
{
	/** Value of Attribute on ASC, including modifiers gated on SkillTags (see GATED-STAT standard). */
	float GetSkillFilteredValue(UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute,
	                            const FGameplayTagContainer& SkillTags)
	{
		if (SkillTags.IsEmpty())
		{
			return ASC->GetNumericAttribute(Attribute);
		}
		FGameplayTagRequirements SourceReqs;
		SourceReqs.RequireTags = SkillTags;
		return ASC->GetFilteredAttributeValue(Attribute, SourceReqs, FGameplayTagContainer());
	}
}

void UFP_GameplayAbility::AppendWeaponStatTags(FGameplayTagContainer& InOutTags)
{
	const FFP_GameplayTags& T = FFP_GameplayTags::Get();
	struct FWeaponStats { FGameplayTag Weapon; bool M, A, F, R; };
	// Source: the weapon / stat design table (Might, Agility, Fortitude, Resonance).
	const FWeaponStats Table[] = {
		{ T.SkillTagMod_Mace,            true,  false, false, false },
		{ T.SkillTagMod_Ordnance,        true,  false, false, false },
		{ T.SkillTagMod_Sword,           false, true,  false, false },
		{ T.SkillTagMod_Pistol,          false, true,  false, false },
		{ T.SkillTagMod_Gauntlets,       false, false, true,  false },
		{ T.SkillTagMod_Shotgun,         false, false, true,  false },
		{ T.SkillTagMod_Conduit,         false, false, false, true  },
		{ T.SkillTagMod_DroneController, false, false, false, true  },
		{ T.SkillTagMod_Rifle,           true,  true,  false, false },
		{ T.SkillTagMod_Axe,             true,  false, true,  false },
		{ T.SkillTagMod_Accelerator,     true,  false, false, true  },
		{ T.SkillTagMod_Spear,           false, true,  true,  false },
		{ T.SkillTagMod_Precision,       false, true,  false, true  },
		{ T.SkillTagMod_Projector,       false, false, true,  true  },
	};

	FGameplayTagContainer Derived;
	for (const FWeaponStats& Row : Table)
	{
		if (!Row.Weapon.IsValid() || !InOutTags.HasTagExact(Row.Weapon))
		{
			continue;
		}
		if (Row.M) Derived.AddTag(T.SkillTagMod_MightWeapon);
		if (Row.A) Derived.AddTag(T.SkillTagMod_AgilityWeapon);
		if (Row.F) Derived.AddTag(T.SkillTagMod_FortitudeWeapon);
		if (Row.R) Derived.AddTag(T.SkillTagMod_ResonanceWeapon);
	}
	InOutTags.AppendTags(Derived);
}

bool UFP_GameplayAbility::FindSkillEntry(const FGameplayAbilityActorInfo* ActorInfo, FFP_AbilityEntry& OutEntry,
                                         const UFP_SkillLibrary** OutLibrary) const
{
	const APawn* Pawn = ActorInfo ? Cast<APawn>(ActorInfo->AvatarActor.Get()) : nullptr;
	const AFP_PlayerState* PS = Pawn ? Pawn->GetPlayerState<AFP_PlayerState>() : nullptr;
	const UFP_SkillLibrary* Library = PS ? PS->GetSkillLibrary() : nullptr;
	if (!Library)
	{
		return false;
	}

	const FGameplayTag SkillTagRoot = FGameplayTag::RequestGameplayTag(FName("Skill"));
	const FGameplayTag SkillsTagRoot = FGameplayTag::RequestGameplayTag(FName("Skills"));
	for (const FGameplayTag& Tag : GetAssetTags())
	{
		if (Tag.MatchesTag(SkillTagRoot) || Tag.MatchesTag(SkillsTagRoot))
		{
			OutEntry = Library->FindAbilityEntryForTag(Tag, false);
			if (OutLibrary)
			{
				*OutLibrary = Library;
			}
			return OutEntry.SkillTag.IsValid();
		}
	}
	return false;
}

bool UFP_GameplayAbility::ComputeSkillCost(const FGameplayAbilityActorInfo* ActorInfo, int32 Level,
                                           float& OutHeatCost, float& OutHitPointCost) const
{
	OutHeatCost = 0.f;
	OutHitPointCost = 0.f;

	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	FFP_AbilityEntry Entry;
	const UFP_SkillLibrary* Library = nullptr;
	if (!ASC || !FindSkillEntry(ActorInfo, Entry, &Library))
	{
		return false;
	}
	if (FMath::IsNearlyZero(Entry.BaseHeatCost) && GetCostGameplayEffect())
	{
		return false; // legacy Blueprint cost GE
	}

	FGameplayTagContainer SkillTags;
	GetSkillModifierTags(SkillTags);

	// 1. Level scaling (global, from the library) + skill tree flat heat.
	const float LevelScale = 1.f + Library->HeatCostPerLevel * static_cast<float>(FMath::Max(Level, 1) - 1);
	const float LevelCost  = Entry.BaseHeatCost * LevelScale + GetFlatHeatCostBonus(ActorInfo);
	if (FMath::IsNearlyZero(LevelCost))
	{
		return true;
	}

	// 2. Caster stats, read as seen by this skill (gated variants count only when they match).
	const float Increased  = GetSkillFilteredValue(ASC, UFP_AttributeSet::GetIncreasedHeatCostAttribute(), SkillTags);
	const float Efficiency = GetSkillFilteredValue(ASC, UFP_AttributeSet::GetHeatCostEfficiencyAttribute(), SkillTags);
	const float Flat       = GetSkillFilteredValue(ASC, UFP_AttributeSet::GetFlatHeatCostAttribute(), SkillTags);

	// Paid = |cost| * (1 + increased) / (1 + efficiency) + flat, never below 0.
	// Efficiency below -0.9 is floored so a big negative can't divide by ~0.
	const float Paid = FMath::Max(0.f,
		FMath::Abs(LevelCost) * FMath::Max(0.f, 1.f + Increased) / FMath::Max(0.1f, 1.f + Efficiency) + Flat);

	// 3. Where it is paid.
	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();
	const bool bLiquidCooled = ASC->HasMatchingGameplayTag(Tags.Flag_SkillTree_MightSkillsCostHPInsteadOfHeat)
		&& SkillTags.HasTagExact(Tags.SkillTagMod_MightWeapon);

	if (bLiquidCooled)
	{
		OutHitPointCost = Paid;      // HP is always a loss, heating or cooling skill
	}
	else
	{
		// Reversals never stack: any source that reverses a direction reverses it once.
		//   Endothermic           : heating skills spend heat, cooling skills restore it
		//   Cryoarethmetic Engine : heating skills lose heat; cooling skills unchanged
		const bool bEndothermic   = UFP_AbilitySystemComponent::IsEndothermic(ASC);
		const bool bReverseHeating = bEndothermic || ASC->HasMatchingGameplayTag(Tags.Flag_SkillTree_CryoarethmeticEngine);
		const bool bReverseCooling = bEndothermic;

		if (LevelCost > 0.f)
		{
			OutHeatCost = bReverseHeating ? -Paid : Paid;            // heat generated (or lost)
		}
		else
		{
			OutHeatCost = bReverseCooling ? -LevelCost : LevelCost;  // cooling: cost stats don't touch it
		}

		// Thermal Mass buffers every skill heat change, either direction (FP_HeatModel.h).
		OutHeatCost *= FPHeat::MassFactor(ASC->GetNumericAttribute(UFP_AttributeSet::GetThermalMassAttribute()));
	}
	return true;
}

void UFP_GameplayAbility::GetSkillCost(float& OutHeatCost, float& OutHitPointCost) const
{
	ComputeSkillCost(GetCurrentActorInfo(), GetAbilityLevel(), OutHeatCost, OutHitPointCost);
}

void UFP_GameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo) const
{
	const int32 Level = GetAbilityLevel(Handle, ActorInfo);
	float Heat = 0.f, HitPointCost = 0.f;
	if (!ComputeSkillCost(ActorInfo, Level, Heat, HitPointCost))
	{
		Super::ApplyCost(Handle, ActorInfo, ActivationInfo); // legacy: per-skill Cost GE
		return;
	}
	if (FMath::IsNearlyZero(Heat) && FMath::IsNearlyZero(HitPointCost))
	{
		return;
	}

	UFP_AbilitySystemComponent* ASC = Cast<UFP_AbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	if (!ASC)
	{
		return;
	}

	const FFP_GameplayTags& Tags = FFP_GameplayTags::Get();
	FGameplayEffectSpecHandle Spec(new FGameplayEffectSpec(
		ASC->GetSkillCostEffect(), MakeEffectContext(Handle, ActorInfo), static_cast<float>(Level)));
	Spec.Data->SetSetByCallerMagnitude(Tags.SetByCaller_SkillCost_Heat, Heat);
	Spec.Data->SetSetByCallerMagnitude(Tags.SetByCaller_SkillCost_HitPoints, -HitPointCost);
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, Spec);
}

bool UFP_GameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	float Heat = 0.f, HitPointCost = 0.f;
	const bool bNativeCost = ComputeSkillCost(ActorInfo, GetAbilityLevel(Handle, ActorInfo), Heat, HitPointCost);

	// Endothermic: no skills (anything with a heat cost, heating or cooling) while Heat < 0.
	if ((bNativeCost || GetCostGameplayEffect())
		&& UFP_AbilitySystemComponent::IsEndothermicHeatLocked(ActorInfo->AbilitySystemComponent.Get()))
	{
		if (OptionalRelevantTags)
		{
			const FGameplayTag& FailTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;
			if (FailTag.IsValid())
			{
				OptionalRelevantTags->AddTag(FailTag);
			}
		}
		return false;
	}

	if (bNativeCost)
	{
		// Heat may go anywhere (overheating is a mechanic, not a block). HP costs may not kill.
		const UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		if (HitPointCost > 0.f
			&& HitPointCost >= ASC->GetNumericAttribute(UFP_AttributeSet::GetHitPointsAttribute()))
		{
			if (OptionalRelevantTags)
			{
				const FGameplayTag& FailTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;
				if (FailTag.IsValid())
				{
					OptionalRelevantTags->AddTag(FailTag);
				}
			}
			return false;
		}
		return true;
	}

	if (bAllowNegativeCost)
	{
		return true;
	}
	return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
}

UAbilitySystemComponent* UFP_GameplayAbility::GetSourceASC() const
{
	return GetAbilitySystemComponentFromActorInfo();
}

float UFP_GameplayAbility::GetSourceAttributeValue(const FGameplayAttribute& Attribute, bool& bFound) const
{
	bFound = false;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC || !Attribute.IsValid())
	{
		return 0.f;
	}

	bFound = true;

	FGameplayTagContainer SkillTags;
	GetSkillModifierTags(SkillTags);
	if (SkillTags.IsEmpty())
	{
		return ASC->GetNumericAttribute(Attribute);
	}

	// Evaluate as if the skill's tags were source tags, so modifiers that require them
	// (gated stats like Speed.Skill.Rifle -> SkillTagMod.Rifle) are counted for this skill only.
	FGameplayTagRequirements SourceReqs;
	SourceReqs.RequireTags = SkillTags;
	return ASC->GetFilteredAttributeValue(Attribute, SourceReqs, FGameplayTagContainer());
}
