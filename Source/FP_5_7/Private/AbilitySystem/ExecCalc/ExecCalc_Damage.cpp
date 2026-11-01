// Copyright JG

#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"
#include "FP_GameplayTags.h"
#include "FP_AbilityTypes.h"
#include "Libraries/FP_AbilitySystemLibrary.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "AbilitySystem/ExecCalc/FP_HeatDamageCurve.h"

struct FP_DamageStatics
{
	// =========================
	// Target mitigation/defense
	// =========================
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armour);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedArmour);

	DECLARE_ATTRIBUTE_CAPTUREDEF(ChanceToDodge);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockedDamage);

	DECLARE_ATTRIBUTE_CAPTUREDEF(ReducedDamageFromProjectiles);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ReducedDamageFromAoE);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ReducedDamageFromMelee);

	// Crit mitigation (damage taken from crits)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);

	// Resistances + caps
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalDamageResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalMaxResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ExplosiveDamageResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ExplosiveMaxResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(RadiationDamageResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(RadiationMaxResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ChemicalDamageResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ChemicalMaxResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(EnergyDamageResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(EnergyMaxResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(OverheatDamageResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(OverheatMaxResistance);

	// Deferred damage defence
	DECLARE_ATTRIBUTE_CAPTUREDEF(DeferredDamageAmount);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DeferredDamageTime);

	// "Taken as" conversions (defensive conversion)
	DECLARE_ATTRIBUTE_CAPTUREDEF(EnergyTakenAsRadiation);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ChemicalTakenAsExplosive);

	// =========================
	// Source offense/damage
	// =========================
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MoreDamage);

	// Flat added damage
	DECLARE_ATTRIBUTE_CAPTUREDEF(AddedPhysicalDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AddedExplosiveDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AddedRadiationDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AddedChemicalDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AddedEnergyDamage);

	// Per-type increased/more
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedPhysicalDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MorePhysicalDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedExplosiveDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MoreExplosiveDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedRadiationDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MoreRadiationDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedChemicalDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MoreChemicalDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedEnergyDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MoreEnergyDamage);
	
	// Weapon/skill-mod increased/more

	// Crit (offensive)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalStrikeChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedCriticalStrikeChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalStrikeMultiplier);

	// Penetration (offensive)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmourPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistancePenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ExplosiveResistancePenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(RadiationResistancePenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ChemicalResistancePenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(EnergyResistancePenetration);
	
	// Outgoing conversions (offensive conversion)
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalDamageToEnergy);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ChemicalDamageToRadiation);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ExplosiveDamageToPhysical);

	// Conditional damage scalars (still "damage", not "debuff stats")
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamageAgainstBruised);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamageAgainstConcussed);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamageAgainstIrradiated);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamageAgainstCorroded);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamageAgainstOverheated);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamageAgainstFrozen);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamagePerHeatFromEquilibrium);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ExecuteThreshold);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ProjectileSpeed);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ProjectileSpeedToDamage);

	// Ambient Heat (for comparing for IncreasedDamagePerHeatFromEquilibrium
	DECLARE_ATTRIBUTE_CAPTUREDEF(AmbientTemperature);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Heat);

	FP_DamageStatics()
	{
		// -------------------------
		// Target captures
		// -------------------------
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, Armour, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedArmour, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ChanceToDodge, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, BlockedDamage, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ReducedDamageFromProjectiles, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ReducedDamageFromAoE, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ReducedDamageFromMelee, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, CriticalHitResistance, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, PhysicalDamageResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, PhysicalMaxResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ExplosiveDamageResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ExplosiveMaxResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, RadiationDamageResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, RadiationMaxResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ChemicalDamageResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ChemicalMaxResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, EnergyDamageResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, EnergyMaxResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, OverheatDamageResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, OverheatMaxResistance, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, DeferredDamageAmount, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, DeferredDamageTime, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, EnergyTakenAsRadiation, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ChemicalTakenAsExplosive, Target, false);

		// -------------------------
		// Source captures
		// -------------------------
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, MoreDamage, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, AddedPhysicalDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, AddedExplosiveDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, AddedRadiationDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, AddedChemicalDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, AddedEnergyDamage, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedPhysicalDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, MorePhysicalDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedExplosiveDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, MoreExplosiveDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedRadiationDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, MoreRadiationDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedChemicalDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, MoreChemicalDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedEnergyDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, MoreEnergyDamage, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, CriticalStrikeChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedCriticalStrikeChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, CriticalStrikeMultiplier, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ArmourPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, PhysicalResistancePenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ExplosiveResistancePenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, RadiationResistancePenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ChemicalResistancePenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, EnergyResistancePenetration, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, PhysicalDamageToEnergy, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ChemicalDamageToRadiation, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ExplosiveDamageToPhysical, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedDamageAgainstBruised, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedDamageAgainstConcussed, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedDamageAgainstIrradiated, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedDamageAgainstCorroded, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedDamageAgainstOverheated, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedDamageAgainstFrozen, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedDamagePerHeatFromEquilibrium, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ExecuteThreshold, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ProjectileSpeed, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ProjectileSpeedToDamage, Source, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, AmbientTemperature, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, Heat, Target, false);
	}
};

static const FP_DamageStatics& DamageStatics()
{
	static FP_DamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	// Target mitigation
	RelevantAttributesToCapture.Add(DamageStatics().ArmourDef);
	RelevantAttributesToCapture.Add(DamageStatics().IncreasedArmourDef);

	RelevantAttributesToCapture.Add(DamageStatics().ChanceToDodgeDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockedDamageDef);

	RelevantAttributesToCapture.Add(DamageStatics().ReducedDamageFromProjectilesDef);
	RelevantAttributesToCapture.Add(DamageStatics().ReducedDamageFromAoEDef);
	RelevantAttributesToCapture.Add(DamageStatics().ReducedDamageFromMeleeDef);

	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);

	RelevantAttributesToCapture.Add(DamageStatics().PhysicalDamageResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalMaxResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ExplosiveDamageResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ExplosiveMaxResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().RadiationDamageResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().RadiationMaxResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ChemicalDamageResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ChemicalMaxResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().EnergyDamageResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().EnergyMaxResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().OverheatDamageResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().OverheatMaxResistanceDef);

	RelevantAttributesToCapture.Add(DamageStatics().DeferredDamageAmountDef);
	RelevantAttributesToCapture.Add(DamageStatics().DeferredDamageTimeDef);

	RelevantAttributesToCapture.Add(DamageStatics().EnergyTakenAsRadiationDef);
	RelevantAttributesToCapture.Add(DamageStatics().ChemicalTakenAsExplosiveDef);

	// Source offense
	RelevantAttributesToCapture.Add(DamageStatics().IncreasedDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().MoreDamageDef);

	RelevantAttributesToCapture.Add(DamageStatics().AddedPhysicalDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().AddedExplosiveDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().AddedRadiationDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().AddedChemicalDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().AddedEnergyDamageDef);

	RelevantAttributesToCapture.Add(DamageStatics().IncreasedPhysicalDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().MorePhysicalDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().IncreasedExplosiveDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().MoreExplosiveDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().IncreasedRadiationDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().MoreRadiationDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().IncreasedChemicalDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().MoreChemicalDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().IncreasedEnergyDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().MoreEnergyDamageDef);

	RelevantAttributesToCapture.Add(DamageStatics().CriticalStrikeChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().IncreasedCriticalStrikeChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalStrikeMultiplierDef);

	RelevantAttributesToCapture.Add(DamageStatics().ArmourPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistancePenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().ExplosiveResistancePenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().RadiationResistancePenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().ChemicalResistancePenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().EnergyResistancePenetrationDef);

	RelevantAttributesToCapture.Add(DamageStatics().PhysicalDamageToEnergyDef);
	RelevantAttributesToCapture.Add(DamageStatics().ChemicalDamageToRadiationDef);
	RelevantAttributesToCapture.Add(DamageStatics().ExplosiveDamageToPhysicalDef);

	RelevantAttributesToCapture.Add(DamageStatics().IncreasedDamageAgainstBruisedDef);
	RelevantAttributesToCapture.Add(DamageStatics().IncreasedDamageAgainstConcussedDef);
	RelevantAttributesToCapture.Add(DamageStatics().IncreasedDamageAgainstIrradiatedDef);
	RelevantAttributesToCapture.Add(DamageStatics().IncreasedDamageAgainstCorrodedDef);
	RelevantAttributesToCapture.Add(DamageStatics().IncreasedDamageAgainstOverheatedDef);
	RelevantAttributesToCapture.Add(DamageStatics().IncreasedDamageAgainstFrozenDef);
	RelevantAttributesToCapture.Add(DamageStatics().IncreasedDamagePerHeatFromEquilibriumDef);
	RelevantAttributesToCapture.Add(DamageStatics().ExecuteThresholdDef);
	RelevantAttributesToCapture.Add(DamageStatics().ProjectileSpeedDef);
	RelevantAttributesToCapture.Add(DamageStatics().ProjectileSpeedToDamageDef);
	
	RelevantAttributesToCapture.Add(DamageStatics().AmbientTemperatureDef);
	RelevantAttributesToCapture.Add(DamageStatics().HeatDef);
}

void UExecCalc_Damage::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	auto GetCaptured = [&](const FGameplayEffectAttributeCaptureDefinition& Def, float DefaultValue = 0.f)
	{
		float OutValue = DefaultValue;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Def, EvaluationParameters, OutValue);
		return OutValue;
	};

	auto Clamp01 = [](float V) { return FMath::Clamp(V, 0.f, 1.f); };

	// -------------------------
	// Debug log helpers
	// -------------------------
	const FString SourceName = SourceAvatar ? SourceAvatar->GetName() : TEXT("None");
	const FString TargetName = TargetAvatar ? TargetAvatar->GetName() : TEXT("None");

	auto LogStep = [&](const TCHAR* Step, float CurrentDamage, const FString& Details)
	{
		if (!bDebugDamage) return;

		UE_LOG(LogTemp, Log, TEXT("[ExecCalc_Damage] %s | Damage=%.4f | Src=%s | Tgt=%s | %s"),
			Step, CurrentDamage, *SourceName, *TargetName, *Details);
	};

	// -------------------------------------------------------
	// 0) DODGE FIRST
	// -------------------------------------------------------
	const float DodgeChanceRaw = GetCaptured(DamageStatics().ChanceToDodgeDef, 0.f);
	const float DodgeChance = Clamp01(DodgeChanceRaw);

	const float DodgeRoll = FMath::FRand();
	const bool bDodged = (DodgeChance > 0.f) && (DodgeRoll < DodgeChance);

	FGameplayEffectContextHandle EffectContectHandle = Spec.GetContext();
	UFP_AbilitySystemLibrary::SetIsDodgedHit(EffectContectHandle, bDodged);
	
	LogStep(TEXT("0) DodgeCheck"),
		0.f,
		FString::Printf(TEXT("DodgeChanceRaw=%.4f DodgeChance=%.4f Roll=%.4f Dodged=%s"),
			DodgeChanceRaw, DodgeChance, DodgeRoll, bDodged ? TEXT("true") : TEXT("false")));

	if (bDodged)
	{
		const FGameplayModifierEvaluatedData EvaluatedData(
			UFP_AttributeSet::GetIncomingDamageAttribute(),
			EGameplayModOp::Additive,
			0.f);

		OutExecutionOutput.AddOutputModifier(EvaluatedData);
		return;
	}

	// -------------------------------------------------------
	// 1) BASE DAMAGE + ADDED + TYPE SCALING
	// -------------------------------------------------------
	const FFP_GameplayTags& GameplayTags = FFP_GameplayTags::Get();

	// SetByCaller base per type (plus legacy generic Damage for backwards compatibility).
	const float SetByCallerGeneric = Spec.GetSetByCallerMagnitude(GameplayTags.Damage, false, 0.f);
	const float SetByCallerPhysical  = Spec.GetSetByCallerMagnitude(GameplayTags.Damage_Physical, false, 0.f);
	const float SetByCallerExplosive = Spec.GetSetByCallerMagnitude(GameplayTags.Damage_Explosive, false, 0.f);
	const float SetByCallerRadiation = Spec.GetSetByCallerMagnitude(GameplayTags.Damage_Radiation, false, 0.f);
	const float SetByCallerChemical  = Spec.GetSetByCallerMagnitude(GameplayTags.Damage_Chemical, false, 0.f);
	const float SetByCallerEnergy    = Spec.GetSetByCallerMagnitude(GameplayTags.Damage_Energy, false, 0.f);

	const float AddedPhys = GetCaptured(DamageStatics().AddedPhysicalDamageDef, 0.f);
	const float AddedExpl = GetCaptured(DamageStatics().AddedExplosiveDamageDef, 0.f);
	const float AddedRad  = GetCaptured(DamageStatics().AddedRadiationDamageDef, 0.f);
	const float AddedChem = GetCaptured(DamageStatics().AddedChemicalDamageDef, 0.f);
	const float AddedEng  = GetCaptured(DamageStatics().AddedEnergyDamageDef, 0.f);

	const float IncreasedGeneric = GetCaptured(DamageStatics().IncreasedDamageDef, 0.f);
	const float IncreasedPhys = GetCaptured(DamageStatics().IncreasedPhysicalDamageDef, 0.f);
	const float IncreasedExpl = GetCaptured(DamageStatics().IncreasedExplosiveDamageDef, 0.f);
	const float IncreasedRad  = GetCaptured(DamageStatics().IncreasedRadiationDamageDef, 0.f);
	const float IncreasedChem = GetCaptured(DamageStatics().IncreasedChemicalDamageDef, 0.f);
	const float IncreasedEng  = GetCaptured(DamageStatics().IncreasedEnergyDamageDef, 0.f);

	const float MoreGeneric = GetCaptured(DamageStatics().MoreDamageDef, 0.f);
	const float MorePhys = GetCaptured(DamageStatics().MorePhysicalDamageDef, 0.f);
	const float MoreExpl = GetCaptured(DamageStatics().MoreExplosiveDamageDef, 0.f);
	const float MoreRad  = GetCaptured(DamageStatics().MoreRadiationDamageDef, 0.f);
	const float MoreChem = GetCaptured(DamageStatics().MoreChemicalDamageDef, 0.f);
	const float MoreEng  = GetCaptured(DamageStatics().MoreEnergyDamageDef, 0.f);

	// -------------------------------------------------------
	// Skill-passive bonus buckets (stamped on spec at cast time by the ability)
	// -------------------------------------------------------
	const float SkillBonusIncreasedDamage  = Spec.GetSetByCallerMagnitude(GameplayTags.SetByCaller_SkillBonus_IncreasedDamage,         false, 0.f);
	const float SkillBonusMoreDamage       = Spec.GetSetByCallerMagnitude(GameplayTags.SetByCaller_SkillBonus_MoreDamage,              false, 0.f);
	const float SkillBonusIncreasedChem    = Spec.GetSetByCallerMagnitude(GameplayTags.SetByCaller_SkillBonus_IncreasedChemicalDamage, false, 0.f);
	const float SkillBonusMoreChem         = Spec.GetSetByCallerMagnitude(GameplayTags.SetByCaller_SkillBonus_MoreChemicalDamage,      false, 0.f);
	const float SkillBonusIncreasedPhys    = Spec.GetSetByCallerMagnitude(GameplayTags.SetByCaller_SkillBonus_IncreasedPhysicalDamage, false, 0.f);
	const float SkillBonusMorePhys         = Spec.GetSetByCallerMagnitude(GameplayTags.SetByCaller_SkillBonus_MorePhysicalDamage,      false, 0.f);
	const float SkillBonusCritChance       = Spec.GetSetByCallerMagnitude(GameplayTags.SetByCaller_SkillBonus_CritChance,              false, 0.f);
	const float SkillBonusIncCritChance    = Spec.GetSetByCallerMagnitude(GameplayTags.SetByCaller_SkillBonus_IncreasedCritChance,     false, 0.f);
	const float SkillBonusCritMultiplier   = Spec.GetSetByCallerMagnitude(GameplayTags.SetByCaller_SkillBonus_CritMultiplier,          false, 0.f);

	FGameplayTagContainer SpecAssetTags;
	Spec.GetAllAssetTags(SpecAssetTags);

	const float TargetHeat  = GetCaptured(DamageStatics().HeatDef, 0.f);
	const float AmbientTemp = GetCaptured(DamageStatics().AmbientTemperatureDef, 0.f);
	const float HeatAmplify = GetCaptured(DamageStatics().IncreasedDamagePerHeatFromEquilibriumDef, 0.f);
	// Separate multiplier (not part of the "increased" pool): 2x at 80 from steady state, -> 5x max.
	const float HeatMultiplier = FPHeatDamage::Multiplier(TargetHeat, AmbientTemp, HeatAmplify);
	const float TempDeltaIncrease = 0.f; // legacy additive slot in ComputeTypeDamage, superseded by HeatMultiplier

	auto ComputeTypeDamage =
		[HeatMultiplier](float BaseSetByCaller, float FlatAdded, float GenericInc, float TypeInc, float TempInc, float GenericMore, float TypeMore)
		{
			const float Flat = BaseSetByCaller + FlatAdded;
			if (Flat <= 0.f)
			{
				return 0.f;
			}

			float Out = Flat;
			Out *= (1.f + GenericInc + TypeInc + TempInc);
			Out *= (1.f + GenericMore);
			Out *= (1.f + TypeMore);
			Out *= HeatMultiplier;
			return Out;
		};

	// Conditional "increased damage against <State>" — attacker's attribute, applied only when the
	// target currently has that state tag. Additive with every other "increased" source.
	float ConditionalIncreased = 0.f;
	if (TargetASC)
	{
		auto AddIfTargetHas = [&](const FGameplayTag& StateTag, const FGameplayEffectAttributeCaptureDefinition& Def)
		{
			if (TargetASC->HasMatchingGameplayTag(StateTag))
			{
				ConditionalIncreased += GetCaptured(Def, 0.f);
			}
		};
		AddIfTargetHas(GameplayTags.State_Bruised,    DamageStatics().IncreasedDamageAgainstBruisedDef);
		AddIfTargetHas(GameplayTags.State_Concussed,  DamageStatics().IncreasedDamageAgainstConcussedDef);
		AddIfTargetHas(GameplayTags.State_Irradiated, DamageStatics().IncreasedDamageAgainstIrradiatedDef);
		AddIfTargetHas(GameplayTags.State_Corroded,   DamageStatics().IncreasedDamageAgainstCorrodedDef);
		AddIfTargetHas(GameplayTags.State_Overheat,   DamageStatics().IncreasedDamageAgainstOverheatedDef);
		AddIfTargetHas(GameplayTags.State_Frozen,     DamageStatics().IncreasedDamageAgainstFrozenDef);
	}

	// Weapon / hit-type damage (Damage.Increased.Rifle, Damage.More.Melee, ...) is gated onto IncreasedDamage /
	// MoreDamage via source-tag requirements, so it is already inside IncreasedGeneric / MoreGeneric here.
	// Velocity Transfer & co.: ProjectileSpeed (gated, as this skill sees it) x conversion fraction,
	// into the ADDITIVE increased pool. Negative speed (reductions) lowers damage.
	const float SpeedToDamage = GetCaptured(DamageStatics().ProjectileSpeedToDamageDef, 0.f);
	const float SpeedIncreased = (SpeedToDamage != 0.f)
		? GetCaptured(DamageStatics().ProjectileSpeedDef, 0.f) * SpeedToDamage : 0.f;

	// Target-side increased damage taken (e.g. Bide while deferring) joins the same additive pool.
	const UFP_AbilitySystemComponent* FPTargetASC = Cast<UFP_AbilitySystemComponent>(TargetASC);
	const float TakenIncreased = FPTargetASC ? FPTargetASC->GetIncreasedDamageTaken() : 0.f;

	const float TotalIncreasedBonus = SkillBonusIncreasedDamage + ConditionalIncreased + SpeedIncreased + TakenIncreased;
	const float TotalMoreBonus      = SkillBonusMoreDamage;

	float GenericDamage   = ComputeTypeDamage(SetByCallerGeneric,   0.f,      IncreasedGeneric + TotalIncreasedBonus, 0.f,                                    TempDeltaIncrease, MoreGeneric + TotalMoreBonus, 0.f);
	float PhysicalDamage  = ComputeTypeDamage(SetByCallerPhysical,  AddedPhys, IncreasedGeneric + TotalIncreasedBonus, IncreasedPhys + SkillBonusIncreasedPhys, TempDeltaIncrease, MoreGeneric + TotalMoreBonus, MorePhys + SkillBonusMorePhys);
	float ExplosiveDamage = ComputeTypeDamage(SetByCallerExplosive, AddedExpl, IncreasedGeneric + TotalIncreasedBonus, IncreasedExpl,                          TempDeltaIncrease, MoreGeneric + TotalMoreBonus, MoreExpl);
	float RadiationDamage = ComputeTypeDamage(SetByCallerRadiation, AddedRad,  IncreasedGeneric + TotalIncreasedBonus, IncreasedRad,                           TempDeltaIncrease, MoreGeneric + TotalMoreBonus, MoreRad);
	float ChemicalDamage  = ComputeTypeDamage(SetByCallerChemical,  AddedChem, IncreasedGeneric + TotalIncreasedBonus, IncreasedChem + SkillBonusIncreasedChem, TempDeltaIncrease, MoreGeneric + TotalMoreBonus, MoreChem + SkillBonusMoreChem);
	float EnergyDamage    = ComputeTypeDamage(SetByCallerEnergy,    AddedEng,  IncreasedGeneric + TotalIncreasedBonus, IncreasedEng,                           TempDeltaIncrease, MoreGeneric + TotalMoreBonus, MoreEng);

	LogStep(TEXT("1) PerTypeOffense"),
		GenericDamage + PhysicalDamage + ExplosiveDamage + RadiationDamage + ChemicalDamage + EnergyDamage,
		FString::Printf(TEXT("SBCC(Generic=%.4f Phys=%.4f Expl=%.4f Rad=%.4f Chem=%.4f Eng=%.4f) | SpeedInc=%.4f TakenInc=%.4f | Offense(Generic=%.4f Phys=%.4f Expl=%.4f Rad=%.4f Chem=%.4f Eng=%.4f)"),
			SetByCallerGeneric, SetByCallerPhysical, SetByCallerExplosive, SetByCallerRadiation, SetByCallerChemical, SetByCallerEnergy,
			SpeedIncreased, TakenIncreased,
			GenericDamage, PhysicalDamage, ExplosiveDamage, RadiationDamage, ChemicalDamage, EnergyDamage));

	// -------------------------------------------------------
	// 2) RESISTANCES (per type, before block)
	// -------------------------------------------------------
	auto ApplyResistance = [](float InDamage, float Resistance, float MaxResistance, float Penetration)
	{
		if (InDamage <= 0.f)
		{
			return 0.f;
		}

		const float CappedResistance = FMath::Min(Resistance, MaxResistance);
		const float EffectiveResistance = FMath::Clamp(CappedResistance - Penetration, 0.f, 1.f);
		return InDamage * (1.f - EffectiveResistance);
	};

	const float PhysRes = GetCaptured(DamageStatics().PhysicalDamageResistanceDef, 0.f);
	const float PhysMaxRes = GetCaptured(DamageStatics().PhysicalMaxResistanceDef, 1.f);
	const float PhysPen = GetCaptured(DamageStatics().PhysicalResistancePenetrationDef, 0.f);

	const float ExplRes = GetCaptured(DamageStatics().ExplosiveDamageResistanceDef, 0.f);
	const float ExplMaxRes = GetCaptured(DamageStatics().ExplosiveMaxResistanceDef, 1.f);
	const float ExplPen = GetCaptured(DamageStatics().ExplosiveResistancePenetrationDef, 0.f);

	const float RadRes = GetCaptured(DamageStatics().RadiationDamageResistanceDef, 0.f);
	const float RadMaxRes = GetCaptured(DamageStatics().RadiationMaxResistanceDef, 1.f);
	const float RadPen = GetCaptured(DamageStatics().RadiationResistancePenetrationDef, 0.f);

	const float ChemRes = GetCaptured(DamageStatics().ChemicalDamageResistanceDef, 0.f);
	const float ChemMaxRes = GetCaptured(DamageStatics().ChemicalMaxResistanceDef, 1.f);
	const float ChemPen = GetCaptured(DamageStatics().ChemicalResistancePenetrationDef, 0.f);

	const float EngRes = GetCaptured(DamageStatics().EnergyDamageResistanceDef, 0.f);
	const float EngMaxRes = GetCaptured(DamageStatics().EnergyMaxResistanceDef, 1.f);
	const float EngPen = GetCaptured(DamageStatics().EnergyResistancePenetrationDef, 0.f);

	PhysicalDamage = ApplyResistance(PhysicalDamage, PhysRes, PhysMaxRes, PhysPen);
	ExplosiveDamage = ApplyResistance(ExplosiveDamage, ExplRes, ExplMaxRes, ExplPen);
	RadiationDamage = ApplyResistance(RadiationDamage, RadRes, RadMaxRes, RadPen);
	ChemicalDamage = ApplyResistance(ChemicalDamage, ChemRes, ChemMaxRes, ChemPen);
	EnergyDamage = ApplyResistance(EnergyDamage, EngRes, EngMaxRes, EngPen);

	float Damage = GenericDamage + PhysicalDamage + ExplosiveDamage + RadiationDamage + ChemicalDamage + EnergyDamage;
	// Block and crit scale the whole hit uniformly, so this share still holds at the armour step.
	const float ExplosiveShare = (Damage > 0.f) ? Clamp01(ExplosiveDamage / Damage) : 0.f;

	LogStep(TEXT("2) PerTypeResisted"),
		Damage,
		FString::Printf(TEXT("Resisted(Generic=%.4f Phys=%.4f Expl=%.4f Rad=%.4f Chem=%.4f Eng=%.4f)"),
			GenericDamage, PhysicalDamage, ExplosiveDamage, RadiationDamage, ChemicalDamage, EnergyDamage));

	if (Damage <= 0.f)
	{
		const FGameplayModifierEvaluatedData EvaluatedData(
			UFP_AttributeSet::GetIncomingDamageAttribute(),
			EGameplayModOp::Additive,
			0.f);

		OutExecutionOutput.AddOutputModifier(EvaluatedData);
		return;
	}

	// -------------------------------------------------------
	// 4) BLOCK
	// -------------------------------------------------------
	const float BlockChanceRaw = GetCaptured(DamageStatics().BlockChanceDef, 0.f);
	const float BlockChance = Clamp01(BlockChanceRaw);

	const float BlockRoll = FMath::FRand();
	const bool bBlocked = (BlockChance > 0.f) && (BlockRoll < BlockChance);

	UFP_AbilitySystemLibrary::SetIsBlockedHit(EffectContectHandle, bBlocked);
	
	float BlockedDamageRaw = 0.f;
	float BlockedDamage = 0.f;

	const float DamageBeforeBlock = Damage;
	if (bBlocked)
	{
		BlockedDamageRaw = GetCaptured(DamageStatics().BlockedDamageDef, 0.f);
		BlockedDamage = FMath::Clamp(BlockedDamageRaw, 0.f, 0.75f);
		Damage *= (1.f - BlockedDamage);
	}

	LogStep(TEXT("4) Block"),
		Damage,
		FString::Printf(TEXT("BlockChance=%.4f Roll=%.4f Blocked=%s BlockedDamage=%.4f (%.4f->%.4f)"),
			BlockChance, BlockRoll, bBlocked ? TEXT("true") : TEXT("false"),
			BlockedDamage, DamageBeforeBlock, Damage));

	// -------------------------------------------------------
	// 5) CRIT
	// -------------------------------------------------------
	const float BaseCritChance = Clamp01(GetCaptured(DamageStatics().CriticalStrikeChanceDef, 0.f) + SkillBonusCritChance);
	const float IncCritChance  = GetCaptured(DamageStatics().IncreasedCriticalStrikeChanceDef, 0.f) + SkillBonusIncCritChance;

	const float FinalCritChance = Clamp01(BaseCritChance * (1.f + IncCritChance));
	const float CritRoll = FMath::FRand();
	const bool bCritical = (FinalCritChance > 0.f) && (CritRoll < FinalCritChance);

	UFP_AbilitySystemLibrary::SetIsCriticalHit(EffectContectHandle, bCritical);
	
	const float DamageBeforeCrit = Damage;

	if (bCritical)
	{
		float CritMultiplier = GetCaptured(DamageStatics().CriticalStrikeMultiplierDef, 1.f) + SkillBonusCritMultiplier;
		CritMultiplier = FMath::Max(CritMultiplier, 1.f);

		float CritBonusResist = Clamp01(GetCaptured(DamageStatics().CriticalHitResistanceDef, 0.f));

		const float CritBonus = CritMultiplier - 1.f;
		const float EffectiveCritBonus = CritBonus * (1.f - CritBonusResist);
		const float FinalCritMultiplier = 1.f + EffectiveCritBonus;

		Damage *= FinalCritMultiplier;
	}

	LogStep(TEXT("5) Crit"),
		Damage,
		FString::Printf(TEXT("FinalCritChance=%.4f Roll=%.4f Critical=%s (%.4f->%.4f)"),
			FinalCritChance, CritRoll, bCritical ? TEXT("true") : TEXT("false"),
			DamageBeforeCrit, Damage));

	// -------------------------------------------------------
	// 6) ARMOUR
	// -------------------------------------------------------
	float Armour = FMath::Max(GetCaptured(DamageStatics().ArmourDef, 0.f), 0.f);
	float IncreasedArmour = GetCaptured(DamageStatics().IncreasedArmourDef, 0.f);
	Armour *= (1.f + IncreasedArmour);
	// Attacker's armour penetration ignores that fraction of the target's (post-increase) armour.
	const float ArmourPen = Clamp01(GetCaptured(DamageStatics().ArmourPenetrationDef, 0.f));
	Armour *= (1.f - ArmourPen);

	const float DamageBeforeArmour = Damage;

	// Flag.SkillTree.DoubleArmourVsExplosive: the explosive share of the hit is mitigated by 2x armour.
	// Both parts use the WHOLE hit's size in the formula (armour scales with hit size), so with no
	// explosive damage or no flag this is identical to the plain formula.
	const float ExplosiveArmourMult = (TargetASC && ExplosiveShare > 0.f
		&& TargetASC->HasMatchingGameplayTag(GameplayTags.Flag_SkillTree_DoubleArmourVsExplosive)) ? 2.f : 1.f;

	if (Armour > 0.f && Damage > 0.f)
	{
		auto ArmourRatio = [&](float A) { return (5.f * DamageBeforeArmour) / (A + 5.f * DamageBeforeArmour); };
		Damage = DamageBeforeArmour * ((1.f - ExplosiveShare) * ArmourRatio(Armour)
			+ ExplosiveShare * ArmourRatio(Armour * ExplosiveArmourMult));
	}

	LogStep(TEXT("6) Armour"),
		Damage,
		FString::Printf(TEXT("Armour=%.4f ExplShare=%.4f ExplArmourMult=%.1f (%.4f->%.4f)"),
			Armour, ExplosiveShare, ExplosiveArmourMult, DamageBeforeArmour, Damage));

	// -------------------------------------------------------
	// 6b) DEFERRED DAMAGE (target defence)
	//     After ALL mitigation: the target's DeferredDamageAmount fraction of this
	//     hit is not taken now but dripped into HitPoints over DeferredDamageTime
	//     seconds (UFP_AbilitySystemComponent deferred instances). Only this direct-
	//     damage calc defers; ExecCalc_DamageOverTime never does.
	//     Restorative Nanites (Flag.SkillTree.RecoupInsteadOfDefer): nothing is subtracted — the
	//     hit lands in full — and the same deferred share becomes a recoup instance that HEALS
	//     over DeferredDamageTime (still counts as deferring, e.g. for Bide).
	// -------------------------------------------------------
	float DeferredDamage = 0.f;
	{
		const float DeferredFraction = Clamp01(GetCaptured(DamageStatics().DeferredDamageAmountDef, 0.f));
		const float DeferredTime     = GetCaptured(DamageStatics().DeferredDamageTimeDef, 0.f);
		const bool bRecoup = UFP_AbilitySystemComponent::RecoupsDeferredDamage(TargetASC);
		if (DeferredFraction > 0.f && DeferredTime > 0.f && Damage > 0.f)
		{
			DeferredDamage = Damage * DeferredFraction;
			if (!bRecoup)
			{
				Damage -= DeferredDamage;
			}
		}
		LogStep(TEXT("6b) Deferred"), Damage,
			FString::Printf(TEXT("Fraction=%.4f Time=%.2fs %s=%.4f Immediate=%.4f"),
				DeferredFraction, DeferredTime, bRecoup ? TEXT("Recoup") : TEXT("Deferred"), DeferredDamage, Damage));
	}

	// -------------------------------------------------------
	// 6c) EXECUTE (attacker offence)
	//     Threshold = attacker's ExecuteThreshold as seen by this hit (gated variants such as
	//     Execute.Threshold.Axe count when the spec carries the tag) + skill-specific
	//     SetByCaller.SkillBonus.ExecuteThreshold. If this hit leaves the target at or below
	//     Threshold x MaxHitPoints, the hit deals exactly the target's remaining life instead,
	//     undeferred, so the normal death / XP / kill-credit path runs. Dodged hits returned earlier.
	// -------------------------------------------------------
	{
		const float ExecuteThreshold =
			GetCaptured(DamageStatics().ExecuteThresholdDef, 0.f)
			+ Spec.GetSetByCallerMagnitude(GameplayTags.SetByCaller_SkillBonus_ExecuteThreshold, false, 0.f);

		if (ExecuteThreshold > 0.f && TargetASC)
		{
			const float TargetHP    = TargetASC->GetNumericAttribute(UFP_AttributeSet::GetHitPointsAttribute());
			const float TargetMaxHP = TargetASC->GetNumericAttribute(UFP_AttributeSet::GetMaxHitPointsAttribute());
			const float HPAfterHit  = TargetHP - Damage; // deferred part hasn't landed yet

			const bool bExecute = TargetHP > 0.f && TargetMaxHP > 0.f
				&& HPAfterHit <= Clamp01(ExecuteThreshold) * TargetMaxHP;
			if (bExecute)
			{
				Damage = FMath::Max(Damage, TargetHP);
				DeferredDamage = 0.f;
				UFP_AbilitySystemLibrary::SetIsExecutedHit(EffectContectHandle, true);
			}

			LogStep(TEXT("6c) Execute"), Damage,
				FString::Printf(TEXT("Threshold=%.4f TargetHP=%.1f/%.1f AfterHit=%.1f Executed=%s"),
					ExecuteThreshold, TargetHP, TargetMaxHP, HPAfterHit, bExecute ? TEXT("true") : TEXT("false")));
		}
	}

	// -------------------------------------------------------
	// 7) OUTPUT
	// -------------------------------------------------------
	LogStep(TEXT("7) Output"), Damage, TEXT("Applying IncomingDamage (+ IncomingDeferredDamage)"));

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		UFP_AttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage));
	if (DeferredDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			UFP_AttributeSet::GetIncomingDeferredDamageAttribute(), EGameplayModOp::Additive, DeferredDamage));
	}
}

