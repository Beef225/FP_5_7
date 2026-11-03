// Copyright JG

#include "AbilitySystem/Effects/FP_GE_EndothermicRegen.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "AbilitySystem/ModMagCalcs/FP_MMC_EndothermicRegen.h"

UFP_GE_EndothermicRegen::UFP_GE_EndothermicRegen()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	Period = FScalableFloat(RegenPeriod);
	bExecutePeriodicEffectOnApplication = false;

	FCustomCalculationBasedFloat Calc;
	Calc.CalculationClassMagnitude = UFP_MMC_EndothermicRegen::StaticClass();

	FGameplayModifierInfo Mod;
	Mod.Attribute = UFP_AttributeSet::GetHeatAttribute();
	Mod.ModifierOp = EGameplayModOp::AddBase;
	Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(Calc);
	Modifiers.Add(Mod);
}
