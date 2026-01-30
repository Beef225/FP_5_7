// Copyright JG


#include "AbilitySystem/FP_AttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Skills/FP_AbilityTypes.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "FP_GameplayTags.h"
#include "Libraries/FP_AbilitySystemLibrary.h"
#include "Interaction/FP_CombatInterface.h"
#include "Interaction/FP_PlayerInterface.h"
#include "Player/FP_PlayerController.h"
#include "Interaction/FP_CombatInterface.h"

UFP_AttributeSet::UFP_AttributeSet()
{
	const FFP_GameplayTags& GameplayTags = FFP_GameplayTags::Get();


		
	//Vital Attributes
TagsToAttributes.Add(GameplayTags.Attributes_Vital_HitPoints, GetHitPointsAttribute);
TagsToAttributes.Add(GameplayTags.Attributes_Vital_MaxHitPoints, GetMaxHitPointsAttribute);
TagsToAttributes.Add(GameplayTags.Attributes_Vital_Heat, GetHeatAttribute);
TagsToAttributes.Add(GameplayTags.Attributes_Vital_MaxHeatThreshold, GetMaxHeatThresholdAttribute);
TagsToAttributes.Add(GameplayTags.Attributees_VitalMinHeatThreshold, GetMinHeatThresholdAttribute);


//Primary Attributes
TagsToAttributes.Add(GameplayTags.Attributes_Primary_Might, GetMightAttribute);
TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resonance, GetResonanceAttribute);
TagsToAttributes.Add(GameplayTags.Attributes_Primary_Agility, GetAgilityAttribute);
TagsToAttributes.Add(GameplayTags.Attributes_Primary_Fortitude, GetFortitudeAttribute);


//Secondary Attributes
//Damage
TagsToAttributes.Add(GameplayTags.Damage_Increased_Generic, GetIncreasedDamageAttribute);
TagsToAttributes.Add(GameplayTags.Damage_More_Generic, GetMoreDamageAttribute);
TagsToAttributes.Add(GameplayTags.Damage_Added_Physical, GetAddedPhysicalDamageAttribute);
TagsToAttributes.Add(GameplayTags.Damage_Increased_Physical, GetIncreasedPhysicalDamageAttribute);
TagsToAttributes.Add(GameplayTags.Damage_More_Physical, GetMorePhysicalDamageAttribute);
TagsToAttributes.Add(GameplayTags.Damage_Added_Explosive, GetAddedExplosiveDamageAttribute);
TagsToAttributes.Add(GameplayTags.Damage_Increased_Explosive, GetIncreasedExplosiveDamageAttribute);
TagsToAttributes.Add(GameplayTags.Damage_More_Explosive, GetMoreExplosiveDamageAttribute);
TagsToAttributes.Add(GameplayTags.Damage_Added_Radiation, GetAddedRadiationDamageAttribute);
TagsToAttributes.Add(GameplayTags.Damage_Increased_Radiation, GetIncreasedRadiationDamageAttribute);
TagsToAttributes.Add(GameplayTags.Damage_More_Radiation, GetMoreRadiationDamageAttribute);
TagsToAttributes.Add(GameplayTags.Damage_Added_Chemical, GetAddedChemicalDamageAttribute);
TagsToAttributes.Add(GameplayTags.Damage_Increased_Chemical, GetIncreasedChemicalDamageAttribute);
TagsToAttributes.Add(GameplayTags.Damage_More_Chemical, GetMoreChemicalDamageAttribute);
TagsToAttributes.Add(GameplayTags.Damage_Added_Energy, GetAddedEnergyDamageAttribute);
TagsToAttributes.Add(GameplayTags.Damage_Increased_Energy, GetIncreasedEnergyDamageAttribute);
TagsToAttributes.Add(GameplayTags.Damage_More_Energy, GetMoreEnergyDamageAttribute);

//Resistance
TagsToAttributes.Add(GameplayTags.Resistance_Damage_Physical, GetPhysicalDamageResistanceAttribute);
TagsToAttributes.Add(GameplayTags.Resistance_Max_Physical, GetPhysicalMaxResistanceAttribute);
TagsToAttributes.Add(GameplayTags.Resistance_Damage_Explosive, GetExplosiveDamageResistanceAttribute);
TagsToAttributes.Add(GameplayTags.Resistance_Max_Explosive, GetExplosiveMaxResistanceAttribute);
TagsToAttributes.Add(GameplayTags.Resistance_Damage_Radiation, GetRadiationDamageResistanceAttribute);
TagsToAttributes.Add(GameplayTags.Resistance_Max_Radiation, GetRadiationMaxResistanceAttribute);
TagsToAttributes.Add(GameplayTags.Resistance_Damage_Chemical, GetChemicalDamageResistanceAttribute);
TagsToAttributes.Add(GameplayTags.Resistance_Max_Chemical, GetChemicalMaxResistanceAttribute);
TagsToAttributes.Add(GameplayTags.Resistance_Damage_Energy, GetEnergyDamageResistanceAttribute);
TagsToAttributes.Add(GameplayTags.Resistance_Max_Energy, GetEnergyMaxResistanceAttribute);
TagsToAttributes.Add(GameplayTags.Resistance_Damage_Overheat, GetOverheatDamageResistanceAttribute);
TagsToAttributes.Add(GameplayTags.Resistance_Max_Overheat, GetOverheatMaxResistanceAttribute);

//Critical
TagsToAttributes.Add(GameplayTags.Critical_StrikeChance_Base, GetCriticalStrikeChanceAttribute);
TagsToAttributes.Add(GameplayTags.Critical_StrikeChance_Increased, GetIncreasedCriticalStrikeChanceAttribute);
TagsToAttributes.Add(GameplayTags.Critical_StrikeMultiplier, GetCriticalStrikeMultiplierAttribute);

//Defence
TagsToAttributes.Add(GameplayTags.Defence_Armour, GetArmourAttribute);
TagsToAttributes.Add(GameplayTags.Defence_DodgeChance, GetChanceToDodgeAttribute);
TagsToAttributes.Add(GameplayTags.Defence_BlockChance, GetBlockChanceAttribute);
TagsToAttributes.Add(GameplayTags.Defence_BlockedDamage, GetBlockedDamageAttribute);
TagsToAttributes.Add(GameplayTags.Defence_DeferredDamageAmount, GetDeferredDamageAmountAttribute);
TagsToAttributes.Add(GameplayTags.Defence_DeferredDamageTime, GetDeferredDamageTimeAttribute);
TagsToAttributes.Add(GameplayTags.Defence_HealthRegeneration, GetHealthRegenerationAttribute);
TagsToAttributes.Add(GameplayTags.Defense_CriticalHitResistance, GetCriticalHitResistanceAttribute);
TagsToAttributes.Add(GameplayTags.Defense_ReducedDamage_Projectiles, GetReducedDamageFromProjectilesAttribute);

//Defense
TagsToAttributes.Add(GameplayTags.Defense_IncreasedArmour, GetIncreasedArmourAttribute);
TagsToAttributes.Add(GameplayTags.Defense_ReducedDamage_AoE, GetReducedDamageFromAoEAttribute);
TagsToAttributes.Add(GameplayTags.Defense_ReduceeDamage_Melee, GetReducedDamageFromMeleeAttribute);

//Heat
TagsToAttributes.Add(GameplayTags.Heat_AmbientTemperature, GetAmbientTemperatureAttribute);
TagsToAttributes.Add(GameplayTags.Heat_ThermalMass, GetThermalMassAttribute);
TagsToAttributes.Add(GameplayTags.Heat_CoolingCoefficient, GetCoolingCoefficientAttribute);

//Debuff
TagsToAttributes.Add(GameplayTags.Debuff_Effectiveness_Bruised, GetBruisedEffectivenessAttribute);
TagsToAttributes.Add(GameplayTags.Debuff_Effectiveness_Concussed, GetConcussedEffectivenessAttribute);
TagsToAttributes.Add(GameplayTags.Debuff_Effectiveness_Irradiated, GetIrradiatedEffectivenessAttribute);
TagsToAttributes.Add(GameplayTags.Debuff_Effectiveness_Corroded, GetCorrodedEffectivenessAttribute);
TagsToAttributes.Add(GameplayTags.Debuff_Effectiveness_ThermalDestabalization, GetThermalDestabalizationEffectivenessAttribute);
TagsToAttributes.Add(GameplayTags.Debuff_Threshold_Bruised, GetBruisedThresholdAttribute);
TagsToAttributes.Add(GameplayTags.Debuff_Threshold_Concussed, GetConcussedThresholdAttribute);
TagsToAttributes.Add(GameplayTags.Debuff_Threshold_Irradiated, GetIrradiatedThresholdAttribute);
TagsToAttributes.Add(GameplayTags.Debuff_Threshold_Corroded, GetCorrodedThresholdAttribute);
TagsToAttributes.Add(GameplayTags.Debuff_Duration_Bruised, GetBruisedDurationAttribute);
TagsToAttributes.Add(GameplayTags.Debuff_Duration_Concussed, GetConcussedDurationAttribute);
TagsToAttributes.Add(GameplayTags.Debuff_Duration_Irradiated, GetIrradiatedDurationAttribute);
TagsToAttributes.Add(GameplayTags.Debuff_Duration_Corroded, GetCorrodedDurationAttribute);
TagsToAttributes.Add(GameplayTags.Debuff_ThermalInsulation, GetThermalInsulationAttribute);

//AoE
TagsToAttributes.Add(GameplayTags.AoE_Multiplier, GetAreaOfEffectMulitplierAttribute);
TagsToAttributes.Add(GameplayTags.AoE_AdditionalRadius, GetAreaOfEffectAdditionalRadiusAttribute);

//Speed
TagsToAttributes.Add(GameplayTags.Speed_Movement, GetMovementSpeedAttribute);
TagsToAttributes.Add(GameplayTags.Speed_Skill, GetSkillSpeedAttribute);
TagsToAttributes.Add(GameplayTags.Speed_Movement_DuringSkill, GetSkillMoveSpeedModifierAttribute);
TagsToAttributes.Add(GameplayTags.Speed_Projectile, GetProjectileSpeedAttribute);

//Item
TagsToAttributes.Add(GameplayTags.Item_Quantity, GetItemQuantityAttribute);
TagsToAttributes.Add(GameplayTags.Item_Rarity, GetItemRarityAttribute);
TagsToAttributes.Add(GameplayTags.Item_WattsQuantity, GetWattsQuantityAttribute);

//DoT
TagsToAttributes.Add(GameplayTags.DoT_Increased, GetIncreasedDamageOverTimeAttribute);
TagsToAttributes.Add(GameplayTags.DoT_Multiplier, GetDamageOverTimeMultiplierAttribute);

//Penetration
TagsToAttributes.Add(GameplayTags.Penetration_Armour, GetArmourPenetrationAttribute);
TagsToAttributes.Add(GameplayTags.Penetration_Physical, GetPhysicalResistancePenetrationAttribute);
TagsToAttributes.Add(GameplayTags.Penetration_Explosive, GetExplosiveResistancePenetrationAttribute);
TagsToAttributes.Add(GameplayTags.Penetration_Radiation, GetRadiationResistancePenetrationAttribute);
TagsToAttributes.Add(GameplayTags.Penetration_Chemical, GetChemicalResistancePenetrationAttribute);
TagsToAttributes.Add(GameplayTags.Penetration_Energy, GetEnergyResistancePenetrationAttribute);

//Projectile
TagsToAttributes.Add(GameplayTags.Projectile_Pierce, GetProjectileTargetsPiercedAttribute);
TagsToAttributes.Add(GameplayTags.Projectile_Bounce, GetProjectileTargetsBouncedAttribute);
TagsToAttributes.Add(GameplayTags.Projectile_Count, GetProjectileCountAttribute);

//Conversion
TagsToAttributes.Add(GameplayTags.Conversion_Damage_PhysicalToEnergy, GetPhysicalDamageToEnergyAttribute);
TagsToAttributes.Add(GameplayTags.Conversion_Damage_ChemicalToRadiation, GetChemicalDamageToRadiationAttribute);
TagsToAttributes.Add(GameplayTags.Conversion_Damage_ExplosiveToPhysical, GetExplosiveDamageToPhysicalAttribute);
TagsToAttributes.Add(GameplayTags.Conversion_Defense_EnergyToRadiation, GetEnergyTakenAsRadiationAttribute);
TagsToAttributes.Add(GameplayTags.Conversion_Defense_ChemicalToExplosive, GetChemicalTakenAsExplosiveAttribute);

//Conditional
TagsToAttributes.Add(GameplayTags.Conditional_Damage_IncreasedAgainstBruised, GetIncreasedDamageAgainstBruisedAttribute);
TagsToAttributes.Add(GameplayTags.Conditional_Damage_IncreasedAgainstConcussed, GetIncreasedDamageAgainstConcussedAttribute);
TagsToAttributes.Add(GameplayTags.Conditional_Damage_IncreasedAgainstIrradiated, GetIncreasedDamageAgainstIrradiatedAttribute);
TagsToAttributes.Add(GameplayTags.Conditional_Damage_IncreasedAgainstCorroded, GetIncreasedDamageAgainstCorrodedAttribute);
TagsToAttributes.Add(GameplayTags.Conditional_Damage_IncreasedAgainstOverheated, GetIncreasedDamageAgainstOverheatedAttribute);
TagsToAttributes.Add(GameplayTags.Conditional_Damage_IncreasedAgainstFrozen, GetIncreasedDamageAgainstFrozenAttribute);
TagsToAttributes.Add(GameplayTags.Conditional_Damage_PerHeat, GetIncreasedDamagePerHeatFromEquilibriumAttribute);

//Recovery
TagsToAttributes.Add(GameplayTags.Recovery_Life_OnHit, GetLifeGainedOnHitAttribute);
TagsToAttributes.Add(GameplayTags.Recovery_Life_OnKill, GetLifeGainedOnKillAttribute);


//Meta Attributes
//Damage
TagsToAttributes.Add(GameplayTags.MetaAttribute_IncomingDamage, GetIncomingDamageAttribute);

//XP
TagsToAttributes.Add(GameplayTags.MetaAttribute_IncomingXP, GetIncomingXPAttribute);
	
}

void UFP_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	//Vital Attributes
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, HitPoints, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, MaxHitPoints, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, Heat, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, MaxHeatThreshold, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, MinHeatThreshold, COND_None, REPNOTIFY_Always)


//Primary Attributes
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, Might, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, Resonance, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, Agility, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, Fortitude, COND_None, REPNOTIFY_Always)


//Secondary Attributes
//Damage
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncreasedDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, MoreDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, AddedPhysicalDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncreasedPhysicalDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, MorePhysicalDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, AddedExplosiveDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncreasedExplosiveDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, MoreExplosiveDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, AddedRadiationDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncreasedRadiationDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, MoreRadiationDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, AddedChemicalDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncreasedChemicalDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, MoreChemicalDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, AddedEnergyDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncreasedEnergyDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, MoreEnergyDamage, COND_None, REPNOTIFY_Always)

//Resistance
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, PhysicalDamageResistance, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, PhysicalMaxResistance, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ExplosiveDamageResistance, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ExplosiveMaxResistance, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, RadiationDamageResistance, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, RadiationMaxResistance, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ChemicalDamageResistance, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ChemicalMaxResistance, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, EnergyDamageResistance, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, EnergyMaxResistance, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, OverheatDamageResistance, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, OverheatMaxResistance, COND_None, REPNOTIFY_Always)

//Critical
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, CriticalStrikeChance, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncreasedCriticalStrikeChance, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, CriticalStrikeMultiplier, COND_None, REPNOTIFY_Always)

//Defence
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, Armour, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ChanceToDodge, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, BlockChance, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, BlockedDamage, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, DeferredDamageAmount, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, DeferredDamageTime, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ReducedDamageFromProjectiles, COND_None, REPNOTIFY_Always)

//Defense
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncreasedArmour, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ReducedDamageFromAoE, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ReducedDamageFromMelee, COND_None, REPNOTIFY_Always)

//Heat
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, AmbientTemperature, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ThermalMass, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, CoolingCoefficient, COND_None, REPNOTIFY_Always)

//Debuff
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, BruisedEffectiveness, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ConcussedEffectiveness, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IrradiatedEffectiveness, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, CorrodedEffectiveness, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ThermalDestabalizationEffectiveness, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, BruisedThreshold, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ConcussedThreshold, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IrradiatedThreshold, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, CorrodedThreshold, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, BruisedDuration, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ConcussedDuration, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IrradiatedDuration, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, CorrodedDuration, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ThermalInsulation, COND_None, REPNOTIFY_Always)

//AoE
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, AreaOfEffectMulitplier, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, AreaOfEffectAdditionalRadius, COND_None, REPNOTIFY_Always)

//Speed
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, SkillSpeed, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, SkillMoveSpeedModifier, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ProjectileSpeed, COND_None, REPNOTIFY_Always)

//Item
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ItemQuantity, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ItemRarity, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, WattsQuantity, COND_None, REPNOTIFY_Always)

//DoT
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncreasedDamageOverTime, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, DamageOverTimeMultiplier, COND_None, REPNOTIFY_Always)

//Penetration
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ArmourPenetration, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, PhysicalResistancePenetration, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ExplosiveResistancePenetration, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, RadiationResistancePenetration, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ChemicalResistancePenetration, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, EnergyResistancePenetration, COND_None, REPNOTIFY_Always)

//Projectile
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ProjectileTargetsPierced, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ProjectileTargetsBounced, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ProjectileCount, COND_None, REPNOTIFY_Always)

//Conversion
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, PhysicalDamageToEnergy, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ChemicalDamageToRadiation, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ExplosiveDamageToPhysical, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, EnergyTakenAsRadiation, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, ChemicalTakenAsExplosive, COND_None, REPNOTIFY_Always)

//Conditional
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncreasedDamageAgainstBruised, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncreasedDamageAgainstConcussed, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncreasedDamageAgainstIrradiated, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncreasedDamageAgainstCorroded, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncreasedDamageAgainstOverheated, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncreasedDamageAgainstFrozen, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncreasedDamagePerHeatFromEquilibrium, COND_None, REPNOTIFY_Always)

//Recovery
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, LifeGainedOnHit, COND_None, REPNOTIFY_Always)

DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, LifeGainedOnKill, COND_None, REPNOTIFY_Always)


//Meta Attributes
//Damage
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncomingDamage, COND_None, REPNOTIFY_Always)

//XP
DOREPLIFETIME_CONDITION_NOTIFY(UFP_AttributeSet, IncomingXP, COND_None, REPNOTIFY_Always)

}

void UFP_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetHitPointsAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHitPoints());
	}
}

void UFP_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	FEffectProperties Props;
	SetEffectProperties(Data, Props);
	
	if (Data.EvaluatedData.Attribute == GetHitPointsAttribute())
	{
		SetHitPoints(FMath::Clamp(GetHitPoints(), 0.f, GetMaxHitPoints()));
	}
	
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);
		if (LocalIncomingDamage > 0.f)
		{
			const float NewHealth = GetHitPoints() - LocalIncomingDamage;
			SetHitPoints(FMath::Clamp(NewHealth, 0.f, GetMaxHitPoints()));

			const bool bFatal = NewHealth <= 0.f;
			if (bFatal)
			{
				if (Props.TargetAvatarActor && Props.TargetAvatarActor->HasAuthority())
				{
					if (IFP_CombatInterface* CombatInterface = Cast<IFP_CombatInterface>(Props.TargetAvatarActor))
					{
						CombatInterface->Die();
					}
				}
			}
			else
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FFP_GameplayTags::Get().Effects_HitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			
			}

		}
	}
	
}

void UFP_AttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UFP_AttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
}

void UFP_AttributeSet::HandleIncomingXP(const FEffectProperties& Props)
{
}

void UFP_AttributeSet::Debuff(const FEffectProperties& Props)
{
}

void UFP_AttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	// Source = causer of the effect, Target = target of the effect (owner of this AS)

	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UFP_AttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit,
	bool bCriticalHit) const
{
}

void UFP_AttributeSet::SendXPEvent(const FEffectProperties& Props)
{
}


//Vital Attributes
void UFP_AttributeSet::OnRep_HitPoints(const FGameplayAttributeData& OldHitPoints) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, HitPoints, OldHitPoints);
}

void UFP_AttributeSet::OnRep_MaxHitPoints(const FGameplayAttributeData& OldMaxHitPoints) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, MaxHitPoints, OldMaxHitPoints);
}

void UFP_AttributeSet::OnRep_Heat(const FGameplayAttributeData& OldHeat) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, Heat, OldHeat);
}

void UFP_AttributeSet::OnRep_MaxHeatThreshold(const FGameplayAttributeData& OldMaxHeatThreshold) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, MaxHeatThreshold, OldMaxHeatThreshold);
}

void UFP_AttributeSet::OnRep_MinHeatThreshold(const FGameplayAttributeData& OldMinHeatThreshold) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, MinHeatThreshold, OldMinHeatThreshold);
}


//Primary Attributes
void UFP_AttributeSet::OnRep_Might(const FGameplayAttributeData& OldMight) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, Might, OldMight);
}

void UFP_AttributeSet::OnRep_Resonance(const FGameplayAttributeData& OldResonance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, Resonance, OldResonance);
}

void UFP_AttributeSet::OnRep_Agility(const FGameplayAttributeData& OldAgility) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, Agility, OldAgility);
}

void UFP_AttributeSet::OnRep_Fortitude(const FGameplayAttributeData& OldFortitude) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, Fortitude, OldFortitude);
}


//Secondary Attributes
//Damage
void UFP_AttributeSet::OnRep_IncreasedDamage(const FGameplayAttributeData& OldIncreasedDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncreasedDamage, OldIncreasedDamage);
}

void UFP_AttributeSet::OnRep_MoreDamage(const FGameplayAttributeData& OldMoreDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, MoreDamage, OldMoreDamage);
}

void UFP_AttributeSet::OnRep_AddedPhysicalDamage(const FGameplayAttributeData& OldAddedPhysicalDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, AddedPhysicalDamage, OldAddedPhysicalDamage);
}

void UFP_AttributeSet::OnRep_IncreasedPhysicalDamage(const FGameplayAttributeData& OldIncreasedPhysicalDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncreasedPhysicalDamage, OldIncreasedPhysicalDamage);
}

void UFP_AttributeSet::OnRep_MorePhysicalDamage(const FGameplayAttributeData& OldMorePhysicalDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, MorePhysicalDamage, OldMorePhysicalDamage);
}

void UFP_AttributeSet::OnRep_AddedExplosiveDamage(const FGameplayAttributeData& OldAddedExplosiveDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, AddedExplosiveDamage, OldAddedExplosiveDamage);
}

void UFP_AttributeSet::OnRep_IncreasedExplosiveDamage(const FGameplayAttributeData& OldIncreasedExplosiveDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncreasedExplosiveDamage, OldIncreasedExplosiveDamage);
}

void UFP_AttributeSet::OnRep_MoreExplosiveDamage(const FGameplayAttributeData& OldMoreExplosiveDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, MoreExplosiveDamage, OldMoreExplosiveDamage);
}

void UFP_AttributeSet::OnRep_AddedRadiationDamage(const FGameplayAttributeData& OldAddedRadiationDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, AddedRadiationDamage, OldAddedRadiationDamage);
}

void UFP_AttributeSet::OnRep_IncreasedRadiationDamage(const FGameplayAttributeData& OldIncreasedRadiationDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncreasedRadiationDamage, OldIncreasedRadiationDamage);
}

void UFP_AttributeSet::OnRep_MoreRadiationDamage(const FGameplayAttributeData& OldMoreRadiationDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, MoreRadiationDamage, OldMoreRadiationDamage);
}

void UFP_AttributeSet::OnRep_AddedChemicalDamage(const FGameplayAttributeData& OldAddedChemicalDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, AddedChemicalDamage, OldAddedChemicalDamage);
}

void UFP_AttributeSet::OnRep_IncreasedChemicalDamage(const FGameplayAttributeData& OldIncreasedChemicalDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncreasedChemicalDamage, OldIncreasedChemicalDamage);
}

void UFP_AttributeSet::OnRep_MoreChemicalDamage(const FGameplayAttributeData& OldMoreChemicalDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, MoreChemicalDamage, OldMoreChemicalDamage);
}

void UFP_AttributeSet::OnRep_AddedEnergyDamage(const FGameplayAttributeData& OldAddedEnergyDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, AddedEnergyDamage, OldAddedEnergyDamage);
}

void UFP_AttributeSet::OnRep_IncreasedEnergyDamage(const FGameplayAttributeData& OldIncreasedEnergyDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncreasedEnergyDamage, OldIncreasedEnergyDamage);
}

void UFP_AttributeSet::OnRep_MoreEnergyDamage(const FGameplayAttributeData& OldMoreEnergyDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, MoreEnergyDamage, OldMoreEnergyDamage);
}

//Resistance
void UFP_AttributeSet::OnRep_PhysicalDamageResistance(const FGameplayAttributeData& OldPhysicalDamageResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, PhysicalDamageResistance, OldPhysicalDamageResistance);
}

void UFP_AttributeSet::OnRep_PhysicalMaxResistance(const FGameplayAttributeData& OldPhysicalMaxResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, PhysicalMaxResistance, OldPhysicalMaxResistance);
}

void UFP_AttributeSet::OnRep_ExplosiveDamageResistance(const FGameplayAttributeData& OldExplosiveDamageResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ExplosiveDamageResistance, OldExplosiveDamageResistance);
}

void UFP_AttributeSet::OnRep_ExplosiveMaxResistance(const FGameplayAttributeData& OldExplosiveMaxResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ExplosiveMaxResistance, OldExplosiveMaxResistance);
}

void UFP_AttributeSet::OnRep_RadiationDamageResistance(const FGameplayAttributeData& OldRadiationDamageResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, RadiationDamageResistance, OldRadiationDamageResistance);
}

void UFP_AttributeSet::OnRep_RadiationMaxResistance(const FGameplayAttributeData& OldRadiationMaxResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, RadiationMaxResistance, OldRadiationMaxResistance);
}

void UFP_AttributeSet::OnRep_ChemicalDamageResistance(const FGameplayAttributeData& OldChemicalDamageResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ChemicalDamageResistance, OldChemicalDamageResistance);
}

void UFP_AttributeSet::OnRep_ChemicalMaxResistance(const FGameplayAttributeData& OldChemicalMaxResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ChemicalMaxResistance, OldChemicalMaxResistance);
}

void UFP_AttributeSet::OnRep_EnergyDamageResistance(const FGameplayAttributeData& OldEnergyDamageResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, EnergyDamageResistance, OldEnergyDamageResistance);
}

void UFP_AttributeSet::OnRep_EnergyMaxResistance(const FGameplayAttributeData& OldEnergyMaxResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, EnergyMaxResistance, OldEnergyMaxResistance);
}

void UFP_AttributeSet::OnRep_OverheatDamageResistance(const FGameplayAttributeData& OldOverheatDamageResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, OverheatDamageResistance, OldOverheatDamageResistance);
}

void UFP_AttributeSet::OnRep_OverheatMaxResistance(const FGameplayAttributeData& OldOverheatMaxResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, OverheatMaxResistance, OldOverheatMaxResistance);
}

//Critical
void UFP_AttributeSet::OnRep_CriticalStrikeChance(const FGameplayAttributeData& OldCriticalStrikeChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, CriticalStrikeChance, OldCriticalStrikeChance);
}

void UFP_AttributeSet::OnRep_IncreasedCriticalStrikeChance(const FGameplayAttributeData& OldIncreasedCriticalStrikeChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncreasedCriticalStrikeChance, OldIncreasedCriticalStrikeChance);
}

void UFP_AttributeSet::OnRep_CriticalStrikeMultiplier(const FGameplayAttributeData& OldCriticalStrikeMultiplier) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, CriticalStrikeMultiplier, OldCriticalStrikeMultiplier);
}

//Defence
void UFP_AttributeSet::OnRep_Armour(const FGameplayAttributeData& OldArmour) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, Armour, OldArmour);
}

void UFP_AttributeSet::OnRep_ChanceToDodge(const FGameplayAttributeData& OldChanceToDodge) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ChanceToDodge, OldChanceToDodge);
}

void UFP_AttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, BlockChance, OldBlockChance);
}

void UFP_AttributeSet::OnRep_BlockedDamage(const FGameplayAttributeData& OldBlockedDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, BlockedDamage, OldBlockedDamage);
}

void UFP_AttributeSet::OnRep_DeferredDamageAmount(const FGameplayAttributeData& OldDeferredDamageAmount) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, DeferredDamageAmount, OldDeferredDamageAmount);
}

void UFP_AttributeSet::OnRep_DeferredDamageTime(const FGameplayAttributeData& OldDeferredDamageTime) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, DeferredDamageTime, OldDeferredDamageTime);
}

void UFP_AttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UFP_AttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UFP_AttributeSet::OnRep_ReducedDamageFromProjectiles(const FGameplayAttributeData& OldReducedDamageFromProjectiles) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ReducedDamageFromProjectiles, OldReducedDamageFromProjectiles);
}

//Defense
void UFP_AttributeSet::OnRep_IncreasedArmour(const FGameplayAttributeData& OldIncreasedArmour) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncreasedArmour, OldIncreasedArmour);
}

void UFP_AttributeSet::OnRep_ReducedDamageFromAoE(const FGameplayAttributeData& OldReducedDamageFromAoE) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ReducedDamageFromAoE, OldReducedDamageFromAoE);
}

void UFP_AttributeSet::OnRep_ReducedDamageFromMelee(const FGameplayAttributeData& OldReducedDamageFromMelee) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ReducedDamageFromMelee, OldReducedDamageFromMelee);
}

//Heat
void UFP_AttributeSet::OnRep_AmbientTemperature(const FGameplayAttributeData& OldAmbientTemperature) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, AmbientTemperature, OldAmbientTemperature);
}

void UFP_AttributeSet::OnRep_ThermalMass(const FGameplayAttributeData& OldThermalMass) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ThermalMass, OldThermalMass);
}

void UFP_AttributeSet::OnRep_CoolingCoefficient(const FGameplayAttributeData& OldCoolingCoefficient) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, CoolingCoefficient, OldCoolingCoefficient);
}

//Debuff
void UFP_AttributeSet::OnRep_BruisedEffectiveness(const FGameplayAttributeData& OldBruisedEffectiveness) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, BruisedEffectiveness, OldBruisedEffectiveness);
}

void UFP_AttributeSet::OnRep_ConcussedEffectiveness(const FGameplayAttributeData& OldConcussedEffectiveness) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ConcussedEffectiveness, OldConcussedEffectiveness);
}

void UFP_AttributeSet::OnRep_IrradiatedEffectiveness(const FGameplayAttributeData& OldIrradiatedEffectiveness) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IrradiatedEffectiveness, OldIrradiatedEffectiveness);
}

void UFP_AttributeSet::OnRep_CorrodedEffectiveness(const FGameplayAttributeData& OldCorrodedEffectiveness) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, CorrodedEffectiveness, OldCorrodedEffectiveness);
}

void UFP_AttributeSet::OnRep_ThermalDestabalizationEffectiveness(const FGameplayAttributeData& OldThermalDestabalizationEffectiveness) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ThermalDestabalizationEffectiveness, OldThermalDestabalizationEffectiveness);
}

void UFP_AttributeSet::OnRep_BruisedThreshold(const FGameplayAttributeData& OldBruisedThreshold) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, BruisedThreshold, OldBruisedThreshold);
}

void UFP_AttributeSet::OnRep_ConcussedThreshold(const FGameplayAttributeData& OldConcussedThreshold) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ConcussedThreshold, OldConcussedThreshold);
}

void UFP_AttributeSet::OnRep_IrradiatedThreshold(const FGameplayAttributeData& OldIrradiatedThreshold) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IrradiatedThreshold, OldIrradiatedThreshold);
}

void UFP_AttributeSet::OnRep_CorrodedThreshold(const FGameplayAttributeData& OldCorrodedThreshold) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, CorrodedThreshold, OldCorrodedThreshold);
}

void UFP_AttributeSet::OnRep_BruisedDuration(const FGameplayAttributeData& OldBruisedDuration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, BruisedDuration, OldBruisedDuration);
}

void UFP_AttributeSet::OnRep_ConcussedDuration(const FGameplayAttributeData& OldConcussedDuration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ConcussedDuration, OldConcussedDuration);
}

void UFP_AttributeSet::OnRep_IrradiatedDuration(const FGameplayAttributeData& OldIrradiatedDuration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IrradiatedDuration, OldIrradiatedDuration);
}

void UFP_AttributeSet::OnRep_CorrodedDuration(const FGameplayAttributeData& OldCorrodedDuration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, CorrodedDuration, OldCorrodedDuration);
}

void UFP_AttributeSet::OnRep_ThermalInsulation(const FGameplayAttributeData& OldThermalInsulation) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ThermalInsulation, OldThermalInsulation);
}

//AoE
void UFP_AttributeSet::OnRep_AreaOfEffectMulitplier(const FGameplayAttributeData& OldAreaOfEffectMulitplier) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, AreaOfEffectMulitplier, OldAreaOfEffectMulitplier);
}

void UFP_AttributeSet::OnRep_AreaOfEffectAdditionalRadius(const FGameplayAttributeData& OldAreaOfEffectAdditionalRadius) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, AreaOfEffectAdditionalRadius, OldAreaOfEffectAdditionalRadius);
}

//Speed
void UFP_AttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, MovementSpeed, OldMovementSpeed);
}

void UFP_AttributeSet::OnRep_SkillSpeed(const FGameplayAttributeData& OldSkillSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, SkillSpeed, OldSkillSpeed);
}

void UFP_AttributeSet::OnRep_SkillMoveSpeedModifier(const FGameplayAttributeData& OldSkillMoveSpeedModifier) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, SkillMoveSpeedModifier, OldSkillMoveSpeedModifier);
}

void UFP_AttributeSet::OnRep_ProjectileSpeed(const FGameplayAttributeData& OldProjectileSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ProjectileSpeed, OldProjectileSpeed);
}

//Item
void UFP_AttributeSet::OnRep_ItemQuantity(const FGameplayAttributeData& OldItemQuantity) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ItemQuantity, OldItemQuantity);
}

void UFP_AttributeSet::OnRep_ItemRarity(const FGameplayAttributeData& OldItemRarity) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ItemRarity, OldItemRarity);
}

void UFP_AttributeSet::OnRep_WattsQuantity(const FGameplayAttributeData& OldWattsQuantity) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, WattsQuantity, OldWattsQuantity);
}

//DoT
void UFP_AttributeSet::OnRep_IncreasedDamageOverTime(const FGameplayAttributeData& OldIncreasedDamageOverTime) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncreasedDamageOverTime, OldIncreasedDamageOverTime);
}

void UFP_AttributeSet::OnRep_DamageOverTimeMultiplier(const FGameplayAttributeData& OldDamageOverTimeMultiplier) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, DamageOverTimeMultiplier, OldDamageOverTimeMultiplier);
}

//Penetration
void UFP_AttributeSet::OnRep_ArmourPenetration(const FGameplayAttributeData& OldArmourPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ArmourPenetration, OldArmourPenetration);
}

void UFP_AttributeSet::OnRep_PhysicalResistancePenetration(const FGameplayAttributeData& OldPhysicalResistancePenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, PhysicalResistancePenetration, OldPhysicalResistancePenetration);
}

void UFP_AttributeSet::OnRep_ExplosiveResistancePenetration(const FGameplayAttributeData& OldExplosiveResistancePenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ExplosiveResistancePenetration, OldExplosiveResistancePenetration);
}

void UFP_AttributeSet::OnRep_RadiationResistancePenetration(const FGameplayAttributeData& OldRadiationResistancePenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, RadiationResistancePenetration, OldRadiationResistancePenetration);
}

void UFP_AttributeSet::OnRep_ChemicalResistancePenetration(const FGameplayAttributeData& OldChemicalResistancePenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ChemicalResistancePenetration, OldChemicalResistancePenetration);
}

void UFP_AttributeSet::OnRep_EnergyResistancePenetration(const FGameplayAttributeData& OldEnergyResistancePenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, EnergyResistancePenetration, OldEnergyResistancePenetration);
}

//Projectile
void UFP_AttributeSet::OnRep_ProjectileTargetsPierced(const FGameplayAttributeData& OldProjectileTargetsPierced) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ProjectileTargetsPierced, OldProjectileTargetsPierced);
}

void UFP_AttributeSet::OnRep_ProjectileTargetsBounced(const FGameplayAttributeData& OldProjectileTargetsBounced) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ProjectileTargetsBounced, OldProjectileTargetsBounced);
}

void UFP_AttributeSet::OnRep_ProjectileCount(const FGameplayAttributeData& OldProjectileCount) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ProjectileCount, OldProjectileCount);
}

//Conversion
void UFP_AttributeSet::OnRep_PhysicalDamageToEnergy(const FGameplayAttributeData& OldPhysicalDamageToEnergy) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, PhysicalDamageToEnergy, OldPhysicalDamageToEnergy);
}

void UFP_AttributeSet::OnRep_ChemicalDamageToRadiation(const FGameplayAttributeData& OldChemicalDamageToRadiation) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ChemicalDamageToRadiation, OldChemicalDamageToRadiation);
}

void UFP_AttributeSet::OnRep_ExplosiveDamageToPhysical(const FGameplayAttributeData& OldExplosiveDamageToPhysical) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ExplosiveDamageToPhysical, OldExplosiveDamageToPhysical);
}

void UFP_AttributeSet::OnRep_EnergyTakenAsRadiation(const FGameplayAttributeData& OldEnergyTakenAsRadiation) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, EnergyTakenAsRadiation, OldEnergyTakenAsRadiation);
}

void UFP_AttributeSet::OnRep_ChemicalTakenAsExplosive(const FGameplayAttributeData& OldChemicalTakenAsExplosive) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, ChemicalTakenAsExplosive, OldChemicalTakenAsExplosive);
}

//Conditional
void UFP_AttributeSet::OnRep_IncreasedDamageAgainstBruised(const FGameplayAttributeData& OldIncreasedDamageAgainstBruised) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncreasedDamageAgainstBruised, OldIncreasedDamageAgainstBruised);
}

void UFP_AttributeSet::OnRep_IncreasedDamageAgainstConcussed(const FGameplayAttributeData& OldIncreasedDamageAgainstConcussed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncreasedDamageAgainstConcussed, OldIncreasedDamageAgainstConcussed);
}

void UFP_AttributeSet::OnRep_IncreasedDamageAgainstIrradiated(const FGameplayAttributeData& OldIncreasedDamageAgainstIrradiated) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncreasedDamageAgainstIrradiated, OldIncreasedDamageAgainstIrradiated);
}

void UFP_AttributeSet::OnRep_IncreasedDamageAgainstCorroded(const FGameplayAttributeData& OldIncreasedDamageAgainstCorroded) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncreasedDamageAgainstCorroded, OldIncreasedDamageAgainstCorroded);
}

void UFP_AttributeSet::OnRep_IncreasedDamageAgainstOverheated(const FGameplayAttributeData& OldIncreasedDamageAgainstOverheated) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncreasedDamageAgainstOverheated, OldIncreasedDamageAgainstOverheated);
}

void UFP_AttributeSet::OnRep_IncreasedDamageAgainstFrozen(const FGameplayAttributeData& OldIncreasedDamageAgainstFrozen) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncreasedDamageAgainstFrozen, OldIncreasedDamageAgainstFrozen);
}

void UFP_AttributeSet::OnRep_IncreasedDamagePerHeatFromEquilibrium(const FGameplayAttributeData& OldIncreasedDamagePerHeatFromEquilibrium) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncreasedDamagePerHeatFromEquilibrium, OldIncreasedDamagePerHeatFromEquilibrium);
}

//Recovery
void UFP_AttributeSet::OnRep_LifeGainedOnHit(const FGameplayAttributeData& OldLifeGainedOnHit) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, LifeGainedOnHit, OldLifeGainedOnHit);
}

void UFP_AttributeSet::OnRep_LifeGainedOnKill(const FGameplayAttributeData& OldLifeGainedOnKill) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, LifeGainedOnKill, OldLifeGainedOnKill);
}


//Meta Attributes
//Damage
void UFP_AttributeSet::OnRep_IncomingDamage(const FGameplayAttributeData& OldIncomingDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncomingDamage, OldIncomingDamage);
}

//XP
void UFP_AttributeSet::OnRep_IncomingXP(const FGameplayAttributeData& OldIncomingXP) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFP_AttributeSet, IncomingXP, OldIncomingXP);
}