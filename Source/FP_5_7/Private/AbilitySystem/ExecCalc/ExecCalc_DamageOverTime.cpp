// Copyright JG

#include "AbilitySystem/ExecCalc/ExecCalc_DamageOverTime.h"
#include "AbilitySystemComponent.h"
#include "FP_GameplayTags.h"
#include "AbilitySystem/FP_AttributeSet.h"

struct FP_DotStatics
{
	// =========================
	// Target — resistances only (no dodge/block for DoT)
	// =========================
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armour);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedArmour);

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

	DECLARE_ATTRIBUTE_CAPTUREDEF(EnergyTakenAsRadiation);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ChemicalTakenAsExplosive);

	// =========================
	// Source — offense
	// =========================
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MoreDamage);

	DECLARE_ATTRIBUTE_CAPTUREDEF(AddedPhysicalDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AddedExplosiveDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AddedRadiationDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AddedChemicalDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AddedEnergyDamage);

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

	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmourPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistancePenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ExplosiveResistancePenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(RadiationResistancePenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ChemicalResistancePenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(EnergyResistancePenetration);

	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalDamageToEnergy);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ChemicalDamageToRadiation);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ExplosiveDamageToPhysical);

	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamageAgainstBruised);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamageAgainstConcussed);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamageAgainstIrradiated);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamageAgainstCorroded);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamageAgainstOverheated);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamageAgainstFrozen);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamagePerHeatFromEquilibrium);

	DECLARE_ATTRIBUTE_CAPTUREDEF(AmbientTemperature);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Heat);

	// DoT-specific multipliers
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncreasedDamageOverTime);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageOverTimeMultiplier);

	FP_DotStatics()
	{
		// Target
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, Armour, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedArmour, Target, false);

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

		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, EnergyTakenAsRadiation, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, ChemicalTakenAsExplosive, Target, false);

		// Source
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

		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, AmbientTemperature, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, Heat, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, IncreasedDamageOverTime, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UFP_AttributeSet, DamageOverTimeMultiplier, Source, false);
	}
};

static const FP_DotStatics& DotStatics()
{
	static FP_DotStatics Statics;
	return Statics;
}

UExecCalc_DamageOverTime::UExecCalc_DamageOverTime()
{
	// Target
	RelevantAttributesToCapture.Add(DotStatics().ArmourDef);
	RelevantAttributesToCapture.Add(DotStatics().IncreasedArmourDef);

	RelevantAttributesToCapture.Add(DotStatics().PhysicalDamageResistanceDef);
	RelevantAttributesToCapture.Add(DotStatics().PhysicalMaxResistanceDef);
	RelevantAttributesToCapture.Add(DotStatics().ExplosiveDamageResistanceDef);
	RelevantAttributesToCapture.Add(DotStatics().ExplosiveMaxResistanceDef);
	RelevantAttributesToCapture.Add(DotStatics().RadiationDamageResistanceDef);
	RelevantAttributesToCapture.Add(DotStatics().RadiationMaxResistanceDef);
	RelevantAttributesToCapture.Add(DotStatics().ChemicalDamageResistanceDef);
	RelevantAttributesToCapture.Add(DotStatics().ChemicalMaxResistanceDef);
	RelevantAttributesToCapture.Add(DotStatics().EnergyDamageResistanceDef);
	RelevantAttributesToCapture.Add(DotStatics().EnergyMaxResistanceDef);
	RelevantAttributesToCapture.Add(DotStatics().OverheatDamageResistanceDef);
	RelevantAttributesToCapture.Add(DotStatics().OverheatMaxResistanceDef);

	RelevantAttributesToCapture.Add(DotStatics().EnergyTakenAsRadiationDef);
	RelevantAttributesToCapture.Add(DotStatics().ChemicalTakenAsExplosiveDef);

	// Source
	RelevantAttributesToCapture.Add(DotStatics().IncreasedDamageDef);
	RelevantAttributesToCapture.Add(DotStatics().MoreDamageDef);

	RelevantAttributesToCapture.Add(DotStatics().AddedPhysicalDamageDef);
	RelevantAttributesToCapture.Add(DotStatics().AddedExplosiveDamageDef);
	RelevantAttributesToCapture.Add(DotStatics().AddedRadiationDamageDef);
	RelevantAttributesToCapture.Add(DotStatics().AddedChemicalDamageDef);
	RelevantAttributesToCapture.Add(DotStatics().AddedEnergyDamageDef);

	RelevantAttributesToCapture.Add(DotStatics().IncreasedPhysicalDamageDef);
	RelevantAttributesToCapture.Add(DotStatics().MorePhysicalDamageDef);
	RelevantAttributesToCapture.Add(DotStatics().IncreasedExplosiveDamageDef);
	RelevantAttributesToCapture.Add(DotStatics().MoreExplosiveDamageDef);
	RelevantAttributesToCapture.Add(DotStatics().IncreasedRadiationDamageDef);
	RelevantAttributesToCapture.Add(DotStatics().MoreRadiationDamageDef);
	RelevantAttributesToCapture.Add(DotStatics().IncreasedChemicalDamageDef);
	RelevantAttributesToCapture.Add(DotStatics().MoreChemicalDamageDef);
	RelevantAttributesToCapture.Add(DotStatics().IncreasedEnergyDamageDef);
	RelevantAttributesToCapture.Add(DotStatics().MoreEnergyDamageDef);

	RelevantAttributesToCapture.Add(DotStatics().ArmourPenetrationDef);
	RelevantAttributesToCapture.Add(DotStatics().PhysicalResistancePenetrationDef);
	RelevantAttributesToCapture.Add(DotStatics().ExplosiveResistancePenetrationDef);
	RelevantAttributesToCapture.Add(DotStatics().RadiationResistancePenetrationDef);
	RelevantAttributesToCapture.Add(DotStatics().ChemicalResistancePenetrationDef);
	RelevantAttributesToCapture.Add(DotStatics().EnergyResistancePenetrationDef);

	RelevantAttributesToCapture.Add(DotStatics().PhysicalDamageToEnergyDef);
	RelevantAttributesToCapture.Add(DotStatics().ChemicalDamageToRadiationDef);
	RelevantAttributesToCapture.Add(DotStatics().ExplosiveDamageToPhysicalDef);

	RelevantAttributesToCapture.Add(DotStatics().IncreasedDamageAgainstBruisedDef);
	RelevantAttributesToCapture.Add(DotStatics().IncreasedDamageAgainstConcussedDef);
	RelevantAttributesToCapture.Add(DotStatics().IncreasedDamageAgainstIrradiatedDef);
	RelevantAttributesToCapture.Add(DotStatics().IncreasedDamageAgainstCorrodedDef);
	RelevantAttributesToCapture.Add(DotStatics().IncreasedDamageAgainstOverheatedDef);
	RelevantAttributesToCapture.Add(DotStatics().IncreasedDamageAgainstFrozenDef);
	RelevantAttributesToCapture.Add(DotStatics().IncreasedDamagePerHeatFromEquilibriumDef);

	RelevantAttributesToCapture.Add(DotStatics().AmbientTemperatureDef);
	RelevantAttributesToCapture.Add(DotStatics().HeatDef);

	RelevantAttributesToCapture.Add(DotStatics().IncreasedDamageOverTimeDef);
	RelevantAttributesToCapture.Add(DotStatics().DamageOverTimeMultiplierDef);
}

void UExecCalc_DamageOverTime::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags  = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = SourceTags;
	EvalParams.TargetTags = TargetTags;

	auto GetCaptured = [&](const FGameplayEffectAttributeCaptureDefinition& Def, float DefaultValue = 0.f)
	{
		float OutValue = DefaultValue;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Def, EvalParams, OutValue);
		return OutValue;
	};

	const FString SourceName = SourceAvatar ? SourceAvatar->GetName() : TEXT("None");
	const FString TargetName = TargetAvatar ? TargetAvatar->GetName() : TEXT("None");

	auto LogStep = [&](const TCHAR* Step, float CurrentDamage, const FString& Details)
	{
		if (!bDebugDamage) return;
		UE_LOG(LogTemp, Log, TEXT("[ExecCalc_DoT] %s | DPS=%.4f | Src=%s | Tgt=%s | %s"),
			Step, CurrentDamage, *SourceName, *TargetName, *Details);
	};

	const FFP_GameplayTags& GameplayTags = FFP_GameplayTags::Get();

	// -------------------------------------------------------
	// 1) BASE DAMAGE PER SECOND + FLAT ADDED + TYPE SCALING
	//    SetByCaller values are damage-per-second; we divide by
	//    DoTTickRate at the end to get the per-tick output.
	// -------------------------------------------------------
	const float SetByCallerGeneric  = Spec.GetSetByCallerMagnitude(GameplayTags.Damage,            false, 0.f);
	const float SetByCallerPhysical  = Spec.GetSetByCallerMagnitude(GameplayTags.Damage_Physical,  false, 0.f);
	const float SetByCallerExplosive = Spec.GetSetByCallerMagnitude(GameplayTags.Damage_Explosive, false, 0.f);
	const float SetByCallerRadiation = Spec.GetSetByCallerMagnitude(GameplayTags.Damage_Radiation, false, 0.f);
	const float SetByCallerChemical  = Spec.GetSetByCallerMagnitude(GameplayTags.Damage_Chemical,  false, 0.f);
	const float SetByCallerEnergy    = Spec.GetSetByCallerMagnitude(GameplayTags.Damage_Energy,    false, 0.f);

	const float AddedPhys = GetCaptured(DotStatics().AddedPhysicalDamageDef);
	const float AddedExpl = GetCaptured(DotStatics().AddedExplosiveDamageDef);
	const float AddedRad  = GetCaptured(DotStatics().AddedRadiationDamageDef);
	const float AddedChem = GetCaptured(DotStatics().AddedChemicalDamageDef);
	const float AddedEng  = GetCaptured(DotStatics().AddedEnergyDamageDef);

	const float IncreasedGeneric = GetCaptured(DotStatics().IncreasedDamageDef);
	const float IncreasedPhys    = GetCaptured(DotStatics().IncreasedPhysicalDamageDef);
	const float IncreasedExpl    = GetCaptured(DotStatics().IncreasedExplosiveDamageDef);
	const float IncreasedRad     = GetCaptured(DotStatics().IncreasedRadiationDamageDef);
	const float IncreasedChem    = GetCaptured(DotStatics().IncreasedChemicalDamageDef);
	const float IncreasedEng     = GetCaptured(DotStatics().IncreasedEnergyDamageDef);

	const float MoreGeneric = GetCaptured(DotStatics().MoreDamageDef);
	const float MorePhys    = GetCaptured(DotStatics().MorePhysicalDamageDef);
	const float MoreExpl    = GetCaptured(DotStatics().MoreExplosiveDamageDef);
	const float MoreRad     = GetCaptured(DotStatics().MoreRadiationDamageDef);
	const float MoreChem    = GetCaptured(DotStatics().MoreChemicalDamageDef);
	const float MoreEng     = GetCaptured(DotStatics().MoreEnergyDamageDef);

	// DoT-specific multipliers — applied after the standard Increased/More chain
	const float IncreasedDoT = GetCaptured(DotStatics().IncreasedDamageOverTimeDef);
	const float MoreDoT      = GetCaptured(DotStatics().DamageOverTimeMultiplierDef);

	const float TargetHeat  = GetCaptured(DotStatics().HeatDef);
	const float AmbientTemp = GetCaptured(DotStatics().AmbientTemperatureDef);
	const float TempDelta   = FMath::Abs(TargetHeat - AmbientTemp);
	const float IncPerHeat  = GetCaptured(DotStatics().IncreasedDamagePerHeatFromEquilibriumDef);
	const float TempDeltaIncrease = TempDelta * IncPerHeat;

	auto ComputeTypeDamage =
		[&](float BaseSetByCaller, float FlatAdded, float TypeInc, float TypeMore) -> float
		{
			const float Flat = BaseSetByCaller + FlatAdded;
			if (Flat <= 0.f) return 0.f;

			float Out = Flat;
			Out *= (1.f + IncreasedGeneric + TypeInc + TempDeltaIncrease);
			Out *= (1.f + MoreGeneric);
			Out *= (1.f + TypeMore);
			Out *= (1.f + IncreasedDoT);
			Out *= (1.f + MoreDoT);
			return Out;
		};

	float GenericDamage  = ComputeTypeDamage(SetByCallerGeneric,   0.f,      0.f,          0.f);
	float PhysicalDamage  = ComputeTypeDamage(SetByCallerPhysical,  AddedPhys, IncreasedPhys, MorePhys);
	float ExplosiveDamage = ComputeTypeDamage(SetByCallerExplosive, AddedExpl, IncreasedExpl, MoreExpl);
	float RadiationDamage = ComputeTypeDamage(SetByCallerRadiation, AddedRad,  IncreasedRad,  MoreRad);
	float ChemicalDamage  = ComputeTypeDamage(SetByCallerChemical,  AddedChem, IncreasedChem, MoreChem);
	float EnergyDamage    = ComputeTypeDamage(SetByCallerEnergy,    AddedEng,  IncreasedEng,  MoreEng);

	LogStep(TEXT("1) PerTypeOffense"),
		GenericDamage + PhysicalDamage + ExplosiveDamage + RadiationDamage + ChemicalDamage + EnergyDamage,
		FString::Printf(TEXT("SBC(Gen=%.2f Phys=%.2f Expl=%.2f Rad=%.2f Chem=%.2f Eng=%.2f) IncDoT=%.4f MoreDoT=%.4f"),
			SetByCallerGeneric, SetByCallerPhysical, SetByCallerExplosive,
			SetByCallerRadiation, SetByCallerChemical, SetByCallerEnergy,
			IncreasedDoT, MoreDoT));

	// -------------------------------------------------------
	// 2) RESISTANCES
	// -------------------------------------------------------
	auto ApplyResistance = [](float InDamage, float Resistance, float MaxResistance, float Penetration) -> float
	{
		if (InDamage <= 0.f) return 0.f;
		const float CappedRes     = FMath::Min(Resistance, MaxResistance);
		const float EffectiveRes  = FMath::Clamp(CappedRes - Penetration, 0.f, 1.f);
		return InDamage * (1.f - EffectiveRes);
	};

	const float PhysPen  = GetCaptured(DotStatics().PhysicalResistancePenetrationDef);
	const float ExplPen  = GetCaptured(DotStatics().ExplosiveResistancePenetrationDef);
	const float RadPen   = GetCaptured(DotStatics().RadiationResistancePenetrationDef);
	const float ChemPen  = GetCaptured(DotStatics().ChemicalResistancePenetrationDef);
	const float EngPen   = GetCaptured(DotStatics().EnergyResistancePenetrationDef);

	PhysicalDamage  = ApplyResistance(PhysicalDamage,  GetCaptured(DotStatics().PhysicalDamageResistanceDef),  GetCaptured(DotStatics().PhysicalMaxResistanceDef,  1.f), PhysPen);
	ExplosiveDamage = ApplyResistance(ExplosiveDamage, GetCaptured(DotStatics().ExplosiveDamageResistanceDef), GetCaptured(DotStatics().ExplosiveMaxResistanceDef, 1.f), ExplPen);
	RadiationDamage = ApplyResistance(RadiationDamage, GetCaptured(DotStatics().RadiationDamageResistanceDef), GetCaptured(DotStatics().RadiationMaxResistanceDef, 1.f), RadPen);
	ChemicalDamage  = ApplyResistance(ChemicalDamage,  GetCaptured(DotStatics().ChemicalDamageResistanceDef),  GetCaptured(DotStatics().ChemicalMaxResistanceDef,  1.f), ChemPen);
	EnergyDamage    = ApplyResistance(EnergyDamage,    GetCaptured(DotStatics().EnergyDamageResistanceDef),    GetCaptured(DotStatics().EnergyMaxResistanceDef,    1.f), EngPen);

	float Damage = GenericDamage + PhysicalDamage + ExplosiveDamage + RadiationDamage + ChemicalDamage + EnergyDamage;

	LogStep(TEXT("2) Resisted"),
		Damage,
		FString::Printf(TEXT("Phys=%.4f Expl=%.4f Rad=%.4f Chem=%.4f Eng=%.4f"),
			PhysicalDamage, ExplosiveDamage, RadiationDamage, ChemicalDamage, EnergyDamage));

	if (Damage <= 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			UFP_AttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, 0.f));
		return;
	}

	// -------------------------------------------------------
	// 3) ARMOUR
	// -------------------------------------------------------
	float Armour = FMath::Max(GetCaptured(DotStatics().ArmourDef), 0.f);
	Armour *= (1.f + GetCaptured(DotStatics().IncreasedArmourDef));

	const float DamageBeforeArmour = Damage;
	if (Armour > 0.f)
	{
		const float Denom = Armour + 5.f * Damage;
		if (Denom > 0.f)
			Damage = (5.f * Damage * Damage) / Denom;
	}

	LogStep(TEXT("3) Armour"),
		Damage,
		FString::Printf(TEXT("Armour=%.4f (%.4f->%.4f)"), Armour, DamageBeforeArmour, Damage));

	// -------------------------------------------------------
	// 4) DIVIDE BY TICK RATE — convert per-second to per-tick
	// -------------------------------------------------------
	const float TickRate = FMath::Max(DoTTickRate, 1.f);
	Damage /= TickRate;

	LogStep(TEXT("4) PerTick"), Damage,
		FString::Printf(TEXT("TickRate=%.1f PerTickDamage=%.4f"), TickRate, Damage));

	// -------------------------------------------------------
	// 5) OUTPUT
	// -------------------------------------------------------
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		UFP_AttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage));
}
