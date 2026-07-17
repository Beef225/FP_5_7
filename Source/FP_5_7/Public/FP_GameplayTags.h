// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**

Gameplay Tags Singleton containing native GameplayTags

*/

struct FFP_GameplayTags
{
public:
	static const FFP_GameplayTags& Get() {return GameplayTags;}
	static void InitializeGameplayTags();
	
	//Vital Attributes
FGameplayTag Attributes_Vital_HitPoints;
FGameplayTag Attributes_Vital_MaxHitPoints;
FGameplayTag Attributes_Vital_Heat;
FGameplayTag Attributes_Vital_MaxHeatThreshold;
FGameplayTag Attributees_VitalMinHeatThreshold;


//Primary Attributes
FGameplayTag Attributes_Primary_Might;
FGameplayTag Attributes_Primary_Resonance;
FGameplayTag Attributes_Primary_Agility;
FGameplayTag Attributes_Primary_Fortitude;


//Secondary Attributes
//Health
FGameplayTag Health_Increased;
FGameplayTag Health_More;

//Damage
FGameplayTag Damage_Increased_Generic;
FGameplayTag Damage_More_Generic;
FGameplayTag Damage_Added_Physical;
FGameplayTag Damage_Increased_Physical;
FGameplayTag Damage_More_Physical;
FGameplayTag Damage_Added_Explosive;
FGameplayTag Damage_Increased_Explosive;
FGameplayTag Damage_More_Explosive;
FGameplayTag Damage_Added_Radiation;
FGameplayTag Damage_Increased_Radiation;
FGameplayTag Damage_More_Radiation;
FGameplayTag Damage_Added_Chemical;
FGameplayTag Damage_Increased_Chemical;
FGameplayTag Damage_More_Chemical;
FGameplayTag Damage_Added_Energy;
FGameplayTag Damage_Increased_Energy;
FGameplayTag Damage_More_Energy;
FGameplayTag Damage_Increased_Pistol;
FGameplayTag Damage_More_Pistol;
FGameplayTag Damage_Increased_Rifle;
FGameplayTag Damage_More_Rifle;
FGameplayTag Damage_Increased_Shotgun;
FGameplayTag Damage_More_Shotgun;
FGameplayTag Damage_Increased_DroneController;
FGameplayTag Damage_More_DroneController;
FGameplayTag Damage_Increased_Axe;
FGameplayTag Damage_More_Axe;
FGameplayTag Damage_Increased_Sword;
FGameplayTag Damage_More_Sword;
FGameplayTag Damage_Increased_Mace;
FGameplayTag Damage_More_Mace;
FGameplayTag Damage_Increased_Gauntlets;
FGameplayTag Damage_More_Gauntlets;
FGameplayTag Damage_Increased_Ordnance;
FGameplayTag Damage_More_Ordnance;
FGameplayTag Damage_Increased_Conduit;
FGameplayTag Damage_More_Conduit;
FGameplayTag Damage_Increased_Accelerator;
FGameplayTag Damage_More_Accelerator;
FGameplayTag Damage_Increased_Spear;
FGameplayTag Damage_More_Spear;
FGameplayTag Damage_Increased_Precision;
FGameplayTag Damage_More_Precision;
FGameplayTag Damage_Increased_Projector;
FGameplayTag Damage_More_Projector;
FGameplayTag Damage_Increased_Melee;
FGameplayTag Damage_More_Melee;
FGameplayTag Damage_Increased_Projectile;
FGameplayTag Damage_More_Projectile;
FGameplayTag Damage_Increased_AoE;
FGameplayTag Damage_More_AoE;

//Resistance
FGameplayTag Resistance_Damage_Physical;
FGameplayTag Resistance_Max_Physical;
FGameplayTag Resistance_Damage_Explosive;
FGameplayTag Resistance_Max_Explosive;
FGameplayTag Resistance_Damage_Radiation;
FGameplayTag Resistance_Max_Radiation;
FGameplayTag Resistance_Damage_Chemical;
FGameplayTag Resistance_Max_Chemical;
FGameplayTag Resistance_Damage_Energy;
FGameplayTag Resistance_Max_Energy;
FGameplayTag Resistance_Damage_Overheat;
FGameplayTag Resistance_Max_Overheat;

//Critical
FGameplayTag Critical_StrikeChance_Base;
FGameplayTag Critical_StrikeChance_Increased;
FGameplayTag Critical_StrikeMultiplier;

//Defence
FGameplayTag Defence_Armour;
FGameplayTag Defence_DodgeChance;
FGameplayTag Defence_BlockChance;
FGameplayTag Defence_BlockedDamage;
FGameplayTag Defence_DeferredDamageAmount;
FGameplayTag Defence_DeferredDamageTime;
FGameplayTag Defence_HealthRegeneration;
FGameplayTag Defense_CriticalHitResistance;
FGameplayTag Defense_ReducedDamage_Projectiles;

//Defense
FGameplayTag Defense_IncreasedArmour;
FGameplayTag Defense_ReducedDamage_AoE;
FGameplayTag Defense_ReduceeDamage_Melee;

//Heat
FGameplayTag Heat_AmbientTemperature;
FGameplayTag Heat_ThermalMass;
FGameplayTag Heat_CoolingCoefficient;
// Modifies the Heat cost of activating skills — decimal fraction, BIDIRECTIONAL:
// negative = reduced cost, positive = increased cost (supports a high-heat
// "sustain it and turn it into damage" build that WANTS higher heat costs on
// purpose, not just cost-reduction builds). Sign convention matches every
// other Increased/Reduced-style attribute (see standard note above): positive
// = more, negative = less. FinalCost = BaseCost * (1 + sum) — a -0.10 node
// ("10% Reduced Heat Cost") gives 0.9x cost, a +0.10 node gives 1.1x cost.
// Gated variants follow the standard <Type> suffix, e.g. Heat.SkillCost.Mace
// for "Reduced Heat Cost with Maces" (magnitude authored as -0.10). Attribute
// only — dynamic heat costs (the GameplayEffect Cost that actually reads
// this) are not implemented yet.
FGameplayTag Heat_SkillCost;

//Debuff
FGameplayTag Debuff_Effectiveness_Bruised;
FGameplayTag Debuff_Effectiveness_Concussed;
FGameplayTag Debuff_Effectiveness_Irradiated;
FGameplayTag Debuff_Effectiveness_Corroded;
FGameplayTag Debuff_Effectiveness_ThermalDestabalization;
FGameplayTag Debuff_Threshold_Bruised;
FGameplayTag Debuff_Threshold_Concussed;
FGameplayTag Debuff_Threshold_Irradiated;
FGameplayTag Debuff_Threshold_Corroded;
FGameplayTag Debuff_Duration_Bruised;
FGameplayTag Debuff_Duration_Concussed;
FGameplayTag Debuff_Duration_Irradiated;
FGameplayTag Debuff_Duration_Corroded;
FGameplayTag Debuff_ThermalInsulation;

//AoE
FGameplayTag AoE_Multiplier;
FGameplayTag AoE_AdditionalRadius;

//Speed
FGameplayTag Speed_Movement;
FGameplayTag Speed_Skill;
FGameplayTag Speed_Movement_DuringSkill;
FGameplayTag Speed_Projectile;

//Duration
FGameplayTag Duration_Skill;

//Item
FGameplayTag Item_Quantity;
FGameplayTag Item_Rarity;
FGameplayTag Item_WattsQuantity;

//DoT
FGameplayTag DoT_Increased;
FGameplayTag DoT_Multiplier;

//Penetration
FGameplayTag Penetration_Armour;
FGameplayTag Penetration_Physical;
FGameplayTag Penetration_Explosive;
FGameplayTag Penetration_Radiation;
FGameplayTag Penetration_Chemical;
FGameplayTag Penetration_Energy;

//Projectile
FGameplayTag Projectile_Pierce;
FGameplayTag Projectile_Bounce;
FGameplayTag Projectile_Count;

//Conversion
FGameplayTag Conversion_Damage_PhysicalToEnergy;
FGameplayTag Conversion_Damage_ChemicalToRadiation;
FGameplayTag Conversion_Damage_ExplosiveToPhysical;
FGameplayTag Conversion_Defense_EnergyToRadiation;
FGameplayTag Conversion_Defense_ChemicalToExplosive;

//Conditional
FGameplayTag Conditional_Damage_IncreasedAgainstBruised;
FGameplayTag Conditional_Damage_IncreasedAgainstConcussed;
FGameplayTag Conditional_Damage_IncreasedAgainstIrradiated;
FGameplayTag Conditional_Damage_IncreasedAgainstCorroded;
FGameplayTag Conditional_Damage_IncreasedAgainstOverheated;
FGameplayTag Conditional_Damage_IncreasedAgainstFrozen;
FGameplayTag Conditional_Damage_PerHeat;

//Recovery
FGameplayTag Recovery_Life_OnHit;
FGameplayTag Recovery_Life_OnKill;


//Meta Attributes
//Damage
FGameplayTag MetaAttribute_IncomingDamage;

//XP
FGameplayTag MetaAttribute_IncomingXP;

//DoT duration (SetByCaller — seconds)
FGameplayTag SetByCaller_DotDuration;

//Deferred damage per tick (SetByCaller for the deferred damage periodic GE)
FGameplayTag SetByCaller_DeferredDamagePerTick;

//Gameplay event sent by ExecCalc_Damage to trigger the deferred damage passive
FGameplayTag GameplayEvent_DeferredDamage;


//Misc Gameplay Tags
//Player Tags
FGameplayTag Player_Block_CursorTrace;
FGameplayTag Player_Block_InputHeld;
FGameplayTag Player_Block_InputReleased;

//Input Tags
FGameplayTag InputTag_LMB;
FGameplayTag InputTag_RMB;
FGameplayTag InputTag_Space;
FGameplayTag InputTag_1;
FGameplayTag InputTag_2;
FGameplayTag InputTag_3;
FGameplayTag InputTag_4;
FGameplayTag InputTag_5;
FGameplayTag InputTag_6;
FGameplayTag InputTag_7;
FGameplayTag InputTag_8;
FGameplayTag InputTag_9;
FGameplayTag InputTag_10;

//InputTags
FGameplayTag InputTag_MMB;
FGameplayTag InputTag_Menu_Character;
FGameplayTag InputTag_Menu_Skills;
FGameplayTag InputTag_Menu_Options;
FGameplayTag InputTag_Menu_Inventory;
FGameplayTag InputTag_Menu_PassiveTree;
FGameplayTag InputTag_Menu_WorldMap;
FGameplayTag InputTag_Menu_LocalMap;
FGameplayTag InputTag_Menu_Stash;

//Events
FGameplayTag Event_Montage_1;
FGameplayTag Event_Montage_2;
FGameplayTag Event_Montage_3;
FGameplayTag Event_Montage_4;
FGameplayTag Event_Montage_5;

//Damage
FGameplayTag Damage;
FGameplayTag Damage_Physical;
FGameplayTag Damage_Explosive;
FGameplayTag Damage_Radiation;
FGameplayTag Damage_Chemical;
FGameplayTag Damage_Energy;
FGameplayTag Damage_Overheat;

//Effects
FGameplayTag Effects_HitReact;
FGameplayTag Effects_DeathReact;

//Skills
FGameplayTag Skills_Melee;
FGameplayTag Skill_Ranged;
FGameplayTag Skill_Summon;
FGameplayTag Skill_Movement;
FGameplayTag Skills_Movement_DodgeRoll;
	FGameplayTag Skills_Movement_Leap;
FGameplayTag Skills_Rotation_Cancelled;
FGameplayTag Skill_MoveSpeed_Diminished;
FGameplayTag Skill_MoveSpeed_None;
FGameplayTag Skills_Status_Equipped;

//Montage
FGameplayTag Montage_Skill_Weapon;
FGameplayTag Montage_Skill_LeftHand;
FGameplayTag Montage_Skill_RightHand;
FGameplayTag Montage_Skill_Tail;
FGameplayTag Montage_Skill_Mouth;

//State
FGameplayTag State_Frozen;
FGameplayTag State_Chilled;
FGameplayTag State_Overheat;
FGameplayTag State_Bruised;
FGameplayTag State_Concussed;
FGameplayTag State_Irradiated;
FGameplayTag State_Corroded;

// Debuff buildup events (sent to target's ASC by skills/effects)
FGameplayTag GameplayEvent_Debuff_Buildup_Bruised;
FGameplayTag GameplayEvent_Debuff_Buildup_Concussed;
FGameplayTag GameplayEvent_Debuff_Buildup_Irradiated;
FGameplayTag GameplayEvent_Debuff_Buildup_Corroded;

//SetByCaller
FGameplayTag SetByCaller_Attribute_IncomingXP;
FGameplayTag SetByCaller_DodgeRoll_MoveSpeed;

// Skill-passive bonus buckets — stamped onto the damage spec by the ability at cast time.
// One fixed tag per modifier category; the ExecCalc reads these generically.
FGameplayTag SetByCaller_SkillBonus_IncreasedDamage;
FGameplayTag SetByCaller_SkillBonus_MoreDamage;
FGameplayTag SetByCaller_SkillBonus_IncreasedChemicalDamage;
FGameplayTag SetByCaller_SkillBonus_MoreChemicalDamage;
FGameplayTag SetByCaller_SkillBonus_CritChance;
FGameplayTag SetByCaller_SkillBonus_IncreasedCritChance;
FGameplayTag SetByCaller_SkillBonus_CritMultiplier;


// ---------------------------------------------------------------------------
// TODO(SkillTagMod / gated-stat tag standard) — pick this back up here.
//
// THE STANDARD (applies to any stat, not just Damage):
//   To make a stat apply only when a skill matches specific condition(s)
//   (a weapon type, a weapon class, a hit type, or any combination), take
//   the stat's existing attribute tag and APPEND one segment per required
//   condition, each segment matching the leaf name of an existing
//   SkillTagMod.<Leaf> tag:
//     Speed.Skill              -> Speed.Skill.Rifle                 (gated: Rifle)
//     Damage.Increased.AoE     -> Damage.Increased.AoE.Mace         (gated: AoE AND Mace)
//     Critical.StrikeMultiplier -> Critical.StrikeMultiplier.Rifle  (gated: Rifle)
//   One segment = one required SkillTagMod tag; multiple segments = ALL of
//   them required (AND, not OR). The gated tag is a NEW, SEPARATE attribute
//   from the ungated/generic one — both get registered and both can be
//   summed into the total independently (a Rifle+AoE skill can pull from
//   the generic bucket, the Rifle bucket, the AoE bucket, AND an AoE.Rifle
//   compound bucket all at once, if all of those attributes exist and have
//   points invested).
//
//   Implementing a gated attribute is always the same two steps, regardless
//   of which stat family it's on:
//     1. Register the attribute (FP_AttributeSet.h/.cpp) and its tag
//        (FP_GameplayTags.h/.cpp + TagsToAttributes), same pattern as every
//        existing IncreasedDamage<Type> attribute.
//     2. Wherever that stat gets READ at runtime, resolve the skill's
//        SkillModifierTags (via Spec.GetAllAssetTags() if going through a
//        GameplayEffect, or directly off the resolved FFP_AbilityEntry
//        otherwise) and, for every gated variant that exists, check
//        HasTagExact(SkillTagMod_X) for EACH of its condition segments —
//        only fold the attribute into the running total if ALL are present.
//   No stat family needs bespoke gating logic beyond "where do I read
//   SkillModifierTags from here, and what's my running total" — the
//   tag-matching shape is identical everywhere.
//
// SIGN CONVENTION (any stat, bidirectional or not): positive = increased /
// more of the thing, negative = reduced / less of the thing. This already
// matches every "Increased X" attribute (positive magnitude = more X via
// `1 + sum`) — the same convention extends to attributes that can go either
// way (e.g. Heat.SkillCost below: author a "Reduced Heat Cost" node with a
// NEGATIVE magnitude, an "Increased Heat Cost" node with a positive one).
// Never flip this per-attribute — a node reading "10% Reduced X" always gets
// authored as -0.10, full stop, regardless of which attribute it targets.
//
// CURRENT STATE:
//   - Damage IS gated this way today: ExecCalc_Damage.cpp reads Spec asset
//     tags and sums IncreasedDamage<Type>/MoreDamage<Type> for the 17
//     single-segment weapon/hit-type tags it checks (Pistol, Rifle, ...,
//     Melee, Projectile, AoE). No COMPOUND (multi-segment) checks exist yet
//     anywhere — every existing check is single-tag. The other 18 registered
//     SkillTagMod_* tags below (DamageOverTime, Bomb, element types, debuff
//     mechanic tags, Piercing, Channeling, Duration, Cooldown, Trigger,
//     Movement, Buff) are declared/registered but consumed nowhere in C++.
//   - Skill Speed (UFP_DamageGameplayAbility::GetSkillSpeedAttributeModifier /
//     GetEffectiveSkillRate), Crit (not yet touched at all), and AoE
//     (GetAoEAttributeModifier / GetEffectiveSphereRadius /
//     GetEffectiveConeParams / GetEffectiveRectParams) are NOT gated at
//     all — they always apply their generic attribute to every ability
//     regardless of SkillModifierTags. This is the next implementation pass:
//     go through each of these call sites and apply the standard above.
//   - AppendSkillModifierTagsToDamageSpec() (FP_DamageGameplayAbility.cpp)
//     only runs when AssignRolledDamageMagnitudes() is explicitly called —
//     verify that's actually happening for the abilities being gated before
//     assuming Damage's existing SkillTagMod bonuses are live end-to-end.
//
// WORKED EXAMPLES already authored into the Rifle nodes in MightPassiveTree
// (NodeDataTable) — none of these are registered/consumed in C++ yet, they
// are EffectStatModAttributeType strings sitting in the table waiting for
// the implementation pass above:
//   Speed.Skill.Rifle                     (gated: Rifle)
//   Critical.StrikeChance.Increased.Rifle (gated: Rifle)
//   Critical.StrikeChance.Base.Rifle      (gated: Rifle)
//   Critical.StrikeMultiplier.Rifle       (gated: Rifle)
//   Projectile.Pierce.Rifle               (gated: Rifle)
//   Projectile.Count.Rifle                (gated: Rifle — "Rifles Fire an
//                                          Extra Projectile"; no base Rifle
//                                          skill exists yet either)
//   AoE.Multiplier.Rifle                  (gated: Rifle)
// COMPOUND examples now actually authored (Mace/Ordnance/Axe nodes):
//   Damage.Increased.AoE.Mace / .Ordnance   AoE.Multiplier.Axe
//   AoE.AdditionalRadius.Mace               DoT.Increased.Axe
//   Debuff.Effectiveness.Bruised.Axe        (gated: hit-type/mechanic AND weapon)
// Same single-segment suffix pattern also now used for non-Damage families
// per the standard above: Critical.StrikeChance.Increased/Base.<Type>,
// Critical.StrikeMultiplier.<Type>, Heat.SkillCost.<Type> (see Heat section).
// Only build attributes/gating for combinations actually being authored —
// this is combinatorial (e.g. 3 hit-types x 14 weapon types = up to 42
// possible Damage compound attributes alone), do not pre-build the matrix.
//
// SkillTagMod_MeleeWeapon / SkillTagMod_RangedWeapon (added below) are a
// WEAPON-CLASS categorization — "this skill belongs to an inherently
// close-range vs. long-range weapon" — deliberately kept distinct from the
// existing SkillTagMod_Melee/SkillTagMod_Projectile/SkillTagMod_AoE tags,
// which are HIT-TYPE tags (what kind of hit a specific damage instance was,
// already consumed by ExecCalc_Damage). A weapon can be MeleeWeapon-class and
// still land a Melee-hit-type attack; the two are correlated but not the same
// tag, since a weapon could plausibly have an off-class attack (a thrown axe,
// a shotgun bash) without this getting confused. Both are valid single
// condition segments under the standard above (e.g. Damage.Increased.MeleeWeapon).
// ---------------------------------------------------------------------------

//SkillTagMods
//SkillTagMod
FGameplayTag SkillTagMod_Pistol;
FGameplayTag SkillTagMod_Rifle;
FGameplayTag SkillTagMod_Shotgun;
FGameplayTag SkillTagMod_DroneController;
FGameplayTag SkillTagMod_Axe;
FGameplayTag SkillTagMod_Sword;
FGameplayTag SkillTagMod_Mace;
FGameplayTag SkillTagMod_Gauntlets;
FGameplayTag SkillTagMod_Ordnance;
FGameplayTag SkillTagMod_Conduit;
FGameplayTag SkillTagMod_Accelerator;
FGameplayTag SkillTagMod_Spear;
FGameplayTag SkillTagMod_Precision;
FGameplayTag SkillTagMod_Projector;
FGameplayTag SkillTagMod_MeleeWeapon;
FGameplayTag SkillTagMod_RangedWeapon;
FGameplayTag SkillTagMod_Projectile;
FGameplayTag SkillTagMod_Melee;
FGameplayTag SkillTagMod_AoE;
FGameplayTag SkillTagMod_DamageOverTime;
FGameplayTag SkillTagMod_Bomb;
FGameplayTag SkillTagMod_Physical;
FGameplayTag SkillTagMod_Explosive;
FGameplayTag SkillTagMod_Chemical;
FGameplayTag SkillTagMod_Radiation;
FGameplayTag SkillTagMod_Energy;
FGameplayTag SkillTagMod_Bruising;
FGameplayTag SkillTagMod_Concussing;
FGameplayTag SkillTagMod_Irradiating;
FGameplayTag SkillTagMod_Corroding;
FGameplayTag SkillTagMod_ThermalDestabilizing;
FGameplayTag SkillTagMod_Piercing;
FGameplayTag SkillTagMod_Channeling;
FGameplayTag SkillTagMod_Duration;
FGameplayTag SkillTagMod_Cooldown;
FGameplayTag SkillTagMod_Trigger;
FGameplayTag SkillTagMod_Movement;
FGameplayTag SkillTagMod_Buff;


// -------------------------------------------------------------------------
// Inventory
// -------------------------------------------------------------------------

// Consumable
FGameplayTag Inventory_Consumable_Potion;

// Craftable
FGameplayTag Inventory_Craftable_Material;

// Quest
FGameplayTag Inventory_Quest_Item;

// Misc
FGameplayTag Inventory_Misc_Junk;

// -------------------------------------------------------------------------
// Inventory — Fragments
// -------------------------------------------------------------------------

/** Identifies FFP_GridFragment: defines the tile footprint of an item on the grid. */
FGameplayTag Fragment_Grid;

/** Identifies FFP_ImageFragment: stores the item's icon texture and display dimensions. */
FGameplayTag Fragment_Icon;

/** Identifies FFP_StackableFragment: defines max stack size and current stack count. */
FGameplayTag Fragment_Stackable;

/** Identifies FFP_ConsumableFragment: defines consume behaviour per item type. */
FGameplayTag Fragment_Consumable;

/** Identifies FFP_TextFragment: stores a text string for display in the item description (e.g. item name). */
FGameplayTag Fragment_ItemName;

/** Identifies FFP_LabeledNumberFragment: stores a labeled numeric stat (e.g. damage, armour). */
FGameplayTag Fragment_PrimaryStat;

/** Identifies a text leaf displaying the item's type/category (e.g. "Weapon", "Consumable"). */
FGameplayTag Fragment_ItemType;

/** Identifies a text leaf displaying flavour/lore text for the item. */
FGameplayTag Fragment_FlavorText;

/** Identifies a labeled-value leaf displaying the item's sell value. */
FGameplayTag Fragment_SellValue;

/** Identifies a labeled-value leaf displaying the item's required level. */
FGameplayTag Fragment_RequiredLevel;

/** Identifies FFP_MeshFragment: defines mesh replacement/attachment operations applied on equip. */
FGameplayTag Fragment_Mesh;

/** Identifies FFP_RarityFragment: holds the item's rarity and its drop-weight table. */
FGameplayTag Fragment_Rarity;
FGameplayTag Fragment_ItemLevel;
FGameplayTag Fragment_AttributeRequirements;

/** Identifies FFP_AffixFragment: holds all rolled affixes for an equippable item. */
FGameplayTag Fragment_Affixes;

/** Identifies FFP_ImplicitFragment: fixed implicit modifiers present on all rarities. */
FGameplayTag Fragment_Implicits;

/** Identifies FFP_SkillFragment: skills rolled from a pool and granted to the player on equip. */
FGameplayTag Fragment_Skills;

/** Identifies FFP_GripStanceFragment: applies a weapon grip style to the player on equip. */
FGameplayTag Fragment_GripStance;


// -------------------------------------------------------------------------
// Equipment Slot Types  (parent tags — items use child tags under these)
// -------------------------------------------------------------------------
FGameplayTag Inventory_Equippable_BodyArmour;
FGameplayTag Inventory_Equippable_Headwear;
FGameplayTag Inventory_Equippable_Gloves;
FGameplayTag Inventory_Equippable_Boots;
FGameplayTag Inventory_Equippable_Weapon;
FGameplayTag Inventory_Equippable_Injector;
FGameplayTag Inventory_Equippable_Belt;
FGameplayTag Inventory_Equippable_Ring;
FGameplayTag Inventory_Equippable_Amulet;

// -------------------------------------------------------------------------
// Item Types
// -------------------------------------------------------------------------
FGameplayTag Inventory_Equippable_Headwear_CombatHelm;
FGameplayTag Inventory_Equippable_Gloves_CombatGloves;
FGameplayTag Inventory_Equippable_BodyArmour_CombatArmour;
	FGameplayTag Inventory_Equippable_BodyArmour_DusterCoat;
FGameplayTag Inventory_Equippable_Boots_CombatBoots;

// -------------------------------------------------------------------------
// Weapons
// -------------------------------------------------------------------------
FGameplayTag Inventory_Equippable_Weapon_ShockRail;
	
	
	FGameplayTag Inventory_Equippable_Weapon_ImpactorMace;
	

// -------------------------------------------------------------------------
//Locations
FGameplayTag Location_Startup;
FGameplayTag Location_The_Aurora;
FGameplayTag Location_Colony;
FGameplayTag Location_Lunar_Base;
FGameplayTag Location_Void_Domain;
FGameplayTag Location_Derelict_Station;
FGameplayTag Location_Crystalline_Depths;
FGameplayTag Location_Frozen_Orbit;
FGameplayTag Location_Ash_Fields;
FGameplayTag Location_Titans_Hollow;
FGameplayTag Location_Shattered_Reach;
FGameplayTag Location_SafeZone;
FGameplayTag Location_EndlessSimulation;

//Skills
//Skills
FGameplayTag Skills_Energy_Firebolt;
FGameplayTag Skills_Radiation_Microwave;
FGameplayTag Skills_Chemical_LightningCoil;
FGameplayTag Skills_Physical_Impact;

	TArray<FGameplayTag> DamageTypes;
	

//PassiveTree
FGameplayTag PassiveTree_Might;
FGameplayTag PassiveTree_Resonance;
FGameplayTag PassiveTree_Agility;
FGameplayTag PassiveTree_Fortitude;
FGameplayTag PassiveTree_IncreasedArmour_1;
FGameplayTag PassiveTree_IncreasedArmour_2;
FGameplayTag PassiveTree_IncreasedArmour_3;
FGameplayTag PassiveTree_IncreasedArmour_4;
FGameplayTag PassiveTree_IncreasedMelee_1;
FGameplayTag PassiveTree_IncreasedMelee_2;
FGameplayTag PassiveTree_IncreasedMelee_3;
FGameplayTag PassiveTree_IncreasedMelee_4;
FGameplayTag PassiveTree_IncreasedMelee_5;
FGameplayTag PassiveTree_IncreasedMelee_6;
FGameplayTag PassiveTree_BruisedConcussedThreshold_1;
FGameplayTag PassiveTree_BruisedConcussedThreshold_2;
FGameplayTag PassiveTree_BruisedConcussedThreshold_3;
FGameplayTag PassiveTree_BruisedConcussedThreshold_4;
FGameplayTag PassiveTree_BruisedConcussedThreshold_5;
FGameplayTag PassiveTree_BruisedConcussedThreshold_6;
FGameplayTag PassiveTree_Might_3;
FGameplayTag PassiveTree_Might_1;
FGameplayTag PassiveTree_Might_2;
FGameplayTag PassiveTree_Might_4;
FGameplayTag PassiveTree_Might_5;
FGameplayTag PassiveTree_Might_6;
FGameplayTag PassiveTree_DeferredDamage_1;
FGameplayTag PassiveTree_DeferredDamage_2;
FGameplayTag PassiveTree_DeferredDamage_3;
FGameplayTag PassiveTree_DeferredDamage_4;
FGameplayTag PassiveTree_DeferredDamage_5;
FGameplayTag PassiveTree_DeferredDamage_6;
FGameplayTag PassiveTree_IncreasedMelee_7;
FGameplayTag PassiveTree_RelentlessStrength_1;
FGameplayTag PassiveTree_IncreasedMelee_8;
FGameplayTag PassiveTree_IncreasedMelee_9;
FGameplayTag PassiveTree_Outlasting_1;
FGameplayTag PassiveTree_BruisedConcussedThreshold_7;
FGameplayTag PassiveTree_BruisedConcussedThreshold_8;
FGameplayTag PassiveTree_Might_7;
FGameplayTag PassiveTree_Might_8;
FGameplayTag PassiveTree_DeferredDamage_7;
FGameplayTag PassiveTree_DeferredDamage_8;
FGameplayTag PassiveTree_IncreasedMelee_10;
FGameplayTag PassiveTree_IncreasedMelee_11;
FGameplayTag PassiveTree_IncreasedMelee_12;
FGameplayTag PassiveTree_IncreasedMelee_13;
FGameplayTag PassiveTree_BruisedConcussedThreshold_9;
FGameplayTag PassiveTree_BruisedConcussedThreshold_10;
FGameplayTag PassiveTree_BruisedConcussedThreshold_11;
FGameplayTag PassiveTree_BruisedConcussedThreshold_12;
FGameplayTag PassiveTree_BruisedConcussedThreshold_13;
FGameplayTag PassiveTree_Might_9;
FGameplayTag PassiveTree_Might_10;
FGameplayTag PassiveTree_Might_11;
FGameplayTag PassiveTree_Might_12;
FGameplayTag PassiveTree_Might_13;
FGameplayTag PassiveTree_DeferredDamage_9;
FGameplayTag PassiveTree_DeferredDamage_10;
FGameplayTag PassiveTree_DeferredDamage_11;
FGameplayTag PassiveTree_DeferredDamage_12;
FGameplayTag PassiveTree_DeferredDamage_13;
FGameplayTag PassiveTree_IncreasedHitPoints_1;
FGameplayTag PassiveTree_Fortitude_3;
FGameplayTag PassiveTree_Fortitude_1;
FGameplayTag PassiveTree_Fortitude_2;
FGameplayTag PassiveTree_Fortitude_4;
FGameplayTag PassiveTree_Fortitude_5;
FGameplayTag PassiveTree_Fortitude_6;
FGameplayTag PassiveTree_AllThreshold_2;
FGameplayTag PassiveTree_Bulwark_1;
FGameplayTag PassiveTree_NeverFall_1;
FGameplayTag PassiveTree_Fortitude_7;
FGameplayTag PassiveTree_Fortitude_8;
FGameplayTag PassiveTree_Fortitude_9;
FGameplayTag PassiveTree_Fortitude_10;
FGameplayTag PassiveTree_Fortitude_11;
FGameplayTag PassiveTree_Fortitude_12;
FGameplayTag PassiveTree_Fortitude_13;
FGameplayTag PassiveTree_IncreasedHitPoints_2;
FGameplayTag PassiveTree_IncreasedHitPoints_3;
FGameplayTag PassiveTree_IncreasedHitPoints_4;
FGameplayTag PassiveTree_IncreasedHitPoints_5;
FGameplayTag PassiveTree_IncreasedHitPoints_6;
FGameplayTag PassiveTree_IncreasedArmour_5;
FGameplayTag PassiveTree_IncreasedArmour_6;
FGameplayTag PassiveTree_AllThreshold_1;
FGameplayTag PassiveTree_AllThreshold_3;
FGameplayTag PassiveTree_AllThreshold_4;
FGameplayTag PassiveTree_AllThreshold_5;
FGameplayTag PassiveTree_AllThreshold_6;
FGameplayTag PassiveTree_IncreasedHitPoints_7;
FGameplayTag PassiveTree_IncreasedHitPoints_8;
FGameplayTag PassiveTree_IncreasedHitPoints_9;
FGameplayTag PassiveTree_IncreasedArmour_7;
FGameplayTag PassiveTree_IncreasedArmour_8;
FGameplayTag PassiveTree_AllThreshold_7;
FGameplayTag PassiveTree_AllThreshold_8;
FGameplayTag PassiveTree_IncreasedHitPoints_10;
FGameplayTag PassiveTree_IncreasedHitPoints_11;
FGameplayTag PassiveTree_IncreasedHitPoints_12;
FGameplayTag PassiveTree_IncreasedHitPoints_13;
FGameplayTag PassiveTree_IncreasedArmour_9;
FGameplayTag PassiveTree_IncreasedArmour_10;
FGameplayTag PassiveTree_IncreasedArmour_11;
FGameplayTag PassiveTree_IncreasedArmour_12;
FGameplayTag PassiveTree_IncreasedArmour_13;
FGameplayTag PassiveTree_AllThreshold_9;
FGameplayTag PassiveTree_AllThreshold_10;
FGameplayTag PassiveTree_AllThreshold_11;
FGameplayTag PassiveTree_AllThreshold_12;
FGameplayTag PassiveTree_AllThreshold_13;
FGameplayTag PassiveTree_Accelerator_AcceleratorDamage_1;
FGameplayTag PassiveTree_Accelerator_AcceleratorDamage_2;
FGameplayTag PassiveTree_Accelerator_AcceleratorDamage_3;
FGameplayTag PassiveTree_Accelerator_AcceleratorDamage_4;
FGameplayTag PassiveTree_Accelerator_AcceleratorDamage_5;
FGameplayTag PassiveTree_Accelerator_AcceleratorDamage_6;
FGameplayTag PassiveTree_Accelerator_AcceleratorDamage_7;
FGameplayTag PassiveTree_Accelerator_ParticleCharge_1;
FGameplayTag PassiveTree_Accelerator_ParticleCharge_2;
FGameplayTag PassiveTree_Accelerator_ParticleCharge_3;
FGameplayTag PassiveTree_Accelerator_ParticleCharge_4;
FGameplayTag PassiveTree_Accelerator_ParticleCharge_5;
FGameplayTag PassiveTree_Accelerator_PenetratorWheel_1;
FGameplayTag PassiveTree_Accelerator_PenetratorWheel_2;
FGameplayTag PassiveTree_Accelerator_PenetratorWheel_3;
FGameplayTag PassiveTree_Accelerator_PenetratorWheel_4;
FGameplayTag PassiveTree_Accelerator_PenetratorWheel_5;
FGameplayTag PassiveTree_Accelerator_RailgunMastery_1;
FGameplayTag PassiveTree_Accelerator_RailgunMastery_2;
FGameplayTag PassiveTree_Accelerator_RailgunMastery_3;
FGameplayTag PassiveTree_Accelerator_RailgunMastery_4;
FGameplayTag PassiveTree_Accelerator_RailgunMastery_5;
FGameplayTag PassiveTree_Accelerator_VelocityRounds_1;
FGameplayTag PassiveTree_Accelerator_VelocityRounds_2;
FGameplayTag PassiveTree_Accelerator_VelocityRounds_3;
FGameplayTag PassiveTree_Accelerator_VelocityRounds_4;
FGameplayTag PassiveTree_Accelerator_VelocityRounds_5;
FGameplayTag PassiveTree_Axe_AxeDamage_1;
FGameplayTag PassiveTree_Axe_AxeDamage_2;
FGameplayTag PassiveTree_Axe_AxeDamage_3;
FGameplayTag PassiveTree_Axe_AxeDamage_4;
FGameplayTag PassiveTree_Axe_AxeDamage_5;
FGameplayTag PassiveTree_Axe_AxeDamage_6;
FGameplayTag PassiveTree_Axe_AxeDamage_7;
FGameplayTag PassiveTree_Axe_BerserkersAxework_1;
FGameplayTag PassiveTree_Axe_BerserkersAxework_2;
FGameplayTag PassiveTree_Axe_BerserkersAxework_3;
FGameplayTag PassiveTree_Axe_BerserkersAxework_4;
FGameplayTag PassiveTree_Axe_BerserkersAxework_5;
FGameplayTag PassiveTree_Axe_BleedWheel_1;
FGameplayTag PassiveTree_Axe_BleedWheel_2;
FGameplayTag PassiveTree_Axe_BleedWheel_3;
FGameplayTag PassiveTree_Axe_BleedWheel_4;
FGameplayTag PassiveTree_Axe_BleedWheel_5;
FGameplayTag PassiveTree_Axe_CleaveWheel_1;
FGameplayTag PassiveTree_Axe_CleaveWheel_2;
FGameplayTag PassiveTree_Axe_CleaveWheel_3;
FGameplayTag PassiveTree_Axe_CleaveWheel_4;
FGameplayTag PassiveTree_Axe_CleaveWheel_5;
FGameplayTag PassiveTree_Axe_Execute_1;
FGameplayTag PassiveTree_Axe_ExecutionersEdge_1;
FGameplayTag PassiveTree_Axe_ExecutionersEdge_2;
FGameplayTag PassiveTree_Axe_ExecutionersEdge_3;
FGameplayTag PassiveTree_Axe_ExecutionersEdge_4;
FGameplayTag PassiveTree_Bide_1;
FGameplayTag PassiveTree_Bide_2;
FGameplayTag PassiveTree_Bide_3;
FGameplayTag PassiveTree_Bide_4;
FGameplayTag PassiveTree_Endure_1;
FGameplayTag PassiveTree_Endure_2;
FGameplayTag PassiveTree_Endure_3;
FGameplayTag PassiveTree_Mace_ArmorBreak_1;
FGameplayTag PassiveTree_Mace_ArmorBreak_2;
FGameplayTag PassiveTree_Mace_ArmorBreak_3;
FGameplayTag PassiveTree_Mace_ArmorBreak_4;
FGameplayTag PassiveTree_Mace_ArmorBreak_5;
FGameplayTag PassiveTree_Mace_GroundSlam_1;
FGameplayTag PassiveTree_Mace_GroundSlam_2;
FGameplayTag PassiveTree_Mace_GroundSlam_3;
FGameplayTag PassiveTree_Mace_GroundSlam_4;
FGameplayTag PassiveTree_Mace_GroundSlam_5;
FGameplayTag PassiveTree_Mace_HeavySwing_1;
FGameplayTag PassiveTree_Mace_HeavySwing_2;
FGameplayTag PassiveTree_Mace_HeavySwing_3;
FGameplayTag PassiveTree_Mace_HeavySwing_4;
FGameplayTag PassiveTree_Mace_HeavySwing_5;
FGameplayTag PassiveTree_Mace_MaceDamageA_1;
FGameplayTag PassiveTree_Mace_MaceDamageA_2;
FGameplayTag PassiveTree_Mace_MaceDamageA_3;
FGameplayTag PassiveTree_Mace_MaceDamageA_4;
FGameplayTag PassiveTree_Mace_MaceDamageA_5;
FGameplayTag PassiveTree_Mace_MaceDamageA_6;
FGameplayTag PassiveTree_Mace_MaceDamageA_7;
FGameplayTag PassiveTree_Mace_MaceDamageB_1;
FGameplayTag PassiveTree_Mace_MaceDamageB_2;
FGameplayTag PassiveTree_Mace_MaceDamageB_3;
FGameplayTag PassiveTree_Mace_MaceDamageB_4;
FGameplayTag PassiveTree_Mace_MaceDamageB_5;
FGameplayTag PassiveTree_Mace_MaceDamageB_6;
FGameplayTag PassiveTree_Mace_SledgehammerMastery_1;
FGameplayTag PassiveTree_Mace_SledgehammerMastery_2;
FGameplayTag PassiveTree_Mace_SledgehammerMastery_3;
FGameplayTag PassiveTree_Mace_SledgehammerMastery_4;
FGameplayTag PassiveTree_Mace_SledgehammerMastery_5;
FGameplayTag PassiveTree_Mace_StaggerWheel_1;
FGameplayTag PassiveTree_Mace_StaggerWheel_2;
FGameplayTag PassiveTree_Mace_StaggerWheel_3;
FGameplayTag PassiveTree_Mace_StaggerWheel_4;
FGameplayTag PassiveTree_Mace_StaggerWheel_5;
FGameplayTag PassiveTree_Ordnance_BlastWheel_1;
FGameplayTag PassiveTree_Ordnance_BlastWheel_2;
FGameplayTag PassiveTree_Ordnance_BlastWheel_3;
FGameplayTag PassiveTree_Ordnance_BlastWheel_4;
FGameplayTag PassiveTree_Ordnance_DevastatingImpact_1;
FGameplayTag PassiveTree_Ordnance_DevastatingImpact_2;
FGameplayTag PassiveTree_Ordnance_DevastatingImpact_3;
FGameplayTag PassiveTree_Ordnance_DevastatingImpact_4;
FGameplayTag PassiveTree_Ordnance_DevastatingImpact_5;
FGameplayTag PassiveTree_Ordnance_ExplosivePenetration_1;
FGameplayTag PassiveTree_Ordnance_ExplosivePenetration_2;
FGameplayTag PassiveTree_Ordnance_ExplosivePenetration_3;
FGameplayTag PassiveTree_Ordnance_ExplosivePenetration_4;
FGameplayTag PassiveTree_Ordnance_ExplosivePenetration_5;
FGameplayTag PassiveTree_Ordnance_HighExplosive_1;
FGameplayTag PassiveTree_Ordnance_HighExplosive_2;
FGameplayTag PassiveTree_Ordnance_HighExplosive_3;
FGameplayTag PassiveTree_Ordnance_HighExplosive_4;
FGameplayTag PassiveTree_Ordnance_HighExplosive_5;
FGameplayTag PassiveTree_Ordnance_OrdnanceDamageA_1;
FGameplayTag PassiveTree_Ordnance_OrdnanceDamageA_2;
FGameplayTag PassiveTree_Ordnance_OrdnanceDamageA_3;
FGameplayTag PassiveTree_Ordnance_OrdnanceDamageA_4;
FGameplayTag PassiveTree_Ordnance_OrdnanceDamageA_5;
FGameplayTag PassiveTree_Ordnance_OrdnanceDamageA_6;
FGameplayTag PassiveTree_Ordnance_OrdnanceDamageA_7;
FGameplayTag PassiveTree_Ordnance_OrdnanceDamageB_1;
FGameplayTag PassiveTree_Ordnance_OrdnanceDamageB_2;
FGameplayTag PassiveTree_Ordnance_OrdnanceDamageB_3;
FGameplayTag PassiveTree_Ordnance_OrdnanceDamageB_4;
FGameplayTag PassiveTree_Ordnance_OrdnanceDamageB_5;
FGameplayTag PassiveTree_Ordnance_OrdnanceDamageB_6;
FGameplayTag PassiveTree_Ordnance_OrdnanceSpecialist_1;
FGameplayTag PassiveTree_Ordnance_OrdnanceSpecialist_2;
FGameplayTag PassiveTree_Ordnance_OrdnanceSpecialist_3;
FGameplayTag PassiveTree_Ordnance_OrdnanceSpecialist_4;
FGameplayTag PassiveTree_Ordnance_OrdnanceSpecialist_5;
FGameplayTag PassiveTree_Ordnance_OrdnanceSpecialist_6;
FGameplayTag PassiveTree_Rifle_MarksmanWheel_1;
FGameplayTag PassiveTree_Rifle_MarksmanWheel_2;
FGameplayTag PassiveTree_Rifle_MarksmanWheel_3;
FGameplayTag PassiveTree_Rifle_MarksmanWheel_4;
FGameplayTag PassiveTree_Rifle_MarksmanWheel_5;
FGameplayTag PassiveTree_Rifle_PenetratingRounds_1;
FGameplayTag PassiveTree_Rifle_PenetratingRounds_2;
FGameplayTag PassiveTree_Rifle_RapidCycling_1;
FGameplayTag PassiveTree_Rifle_RapidCycling_2;
FGameplayTag PassiveTree_Rifle_RapidCycling_3;
FGameplayTag PassiveTree_Rifle_RapidCycling_4;
FGameplayTag PassiveTree_Rifle_RapidCycling_5;
FGameplayTag PassiveTree_Rifle_RifleDamage_1;
FGameplayTag PassiveTree_Rifle_RifleDamage_2;
FGameplayTag PassiveTree_Rifle_RifleDamage_3;
FGameplayTag PassiveTree_Rifle_RifleDamage_4;
FGameplayTag PassiveTree_Rifle_RifleDamage_5;
FGameplayTag PassiveTree_Rifle_RifleDamage_6;
FGameplayTag PassiveTree_Rifle_RifleDamage_7;
FGameplayTag PassiveTree_Rifle_RiflemansMastery_1;
FGameplayTag PassiveTree_Rifle_RiflemansMastery_2;
FGameplayTag PassiveTree_Rifle_RiflemansMastery_3;
FGameplayTag PassiveTree_Rifle_RiflemansMastery_4;
FGameplayTag PassiveTree_Rifle_RiflemansMastery_5;
FGameplayTag PassiveTree_RunningHot_1;
FGameplayTag PassiveTree_RunningHot_2;
FGameplayTag PassiveTree_RunningHot_3;
FGameplayTag PassiveTree_RunningHot_4;
FGameplayTag PassiveTree_SkillSpeed_1;
FGameplayTag PassiveTree_SkillSpeed_2;
FGameplayTag PassiveTree_SkillSpeed_3;
FGameplayTag PassiveTree_SplicedMalacostraca_1;
FGameplayTag PassiveTree_SplicedMalacostraca_2;
FGameplayTag PassiveTree_SplicedMalacostraca_3;
FGameplayTag PassiveTree_CarryOn_1;
FGameplayTag PassiveTree_CarryOn_2;
FGameplayTag PassiveTree_CarryOn_3;
FGameplayTag PassiveTree_AblativeArmour_1;
FGameplayTag PassiveTree_AblativeArmour_2;
FGameplayTag PassiveTree_AblativeArmour_3;
FGameplayTag PassiveTree_SplicedPlanariidae_1;
FGameplayTag PassiveTree_SplicedPlanariidae_2;
FGameplayTag PassiveTree_SplicedPlanariidae_3;
FGameplayTag PassiveTree_SplicedPlanariidae_4;
FGameplayTag PassiveTree_LiquidCooledSystems_1;

//SkillTree
FGameplayTag SkillTree_LightningCoil_Damage_1;
FGameplayTag SkillTree_LightningCoil_Damage_2;
FGameplayTag SkillTree_LightningCoil_Damage_3;
FGameplayTag SkillTree_LightningCoil_Damage_4;
FGameplayTag SkillTree_LightningCoil_Damage_5;
FGameplayTag SkillTree_LightningCoil_Damage_6;
FGameplayTag SkillTree_LightningCoil_Damage_7;
FGameplayTag SkillTree_LightningCoil_IncreasedCritChance_1;
FGameplayTag SkillTree_LightningCoil_IncreasedCritChance_2;
FGameplayTag SkillTree_LightningCoil_IncreasedCritChance_3;
FGameplayTag SkillTree_LightningCoil_IncreasedCritChance_4;
FGameplayTag SkillTree_LightningCoil_IncreasedCritChance_5;
FGameplayTag SkillTree_LightningCoil_IncreasedCritChance_6;
FGameplayTag SkillTree_LightningCoil_IncreasedCritChance_7;
FGameplayTag SkillTree_LightningCoil_IncreasedCritChance_8;
FGameplayTag SkillTree_LightningCoil_BoltAoE_1;
FGameplayTag SkillTree_LightningCoil_BoltAoE_2;
FGameplayTag SkillTree_LightningCoil_BoltAoE_3;
FGameplayTag SkillTree_LightningCoil_BoltAoE_4;
FGameplayTag SkillTree_LightningCoil_BoltAoE_5;
FGameplayTag SkillTree_LightningCoil_BoltAoE_6;
FGameplayTag SkillTree_LightningCoil_CorrodedBuildup_1;
FGameplayTag SkillTree_LightningCoil_CorrodedBuildup_2;
FGameplayTag SkillTree_LightningCoil_CorrodedBuildup_3;
FGameplayTag SkillTree_LightningCoil_CorrodedBuildup_4;
FGameplayTag SkillTree_LightningCoil_CorrodedBuildup_5;
FGameplayTag SkillTree_LightningCoil_CorrodedBuildup_6;
FGameplayTag SkillTree_LightningCoil_CorrodedBuildup_7;
FGameplayTag SkillTree_LightningCoil_Convergence_1;
FGameplayTag SkillTree_LightningCoil_Convergence_2;
FGameplayTag SkillTree_LightningCoil_Convergence_3;
FGameplayTag SkillTree_LightningCoil_ExtraBolts_1;
FGameplayTag SkillTree_LightningCoil_ExtraBolts_2;
FGameplayTag SkillTree_LightningCoil_ExtraBolts_3;
FGameplayTag SkillTree_LightningCoil_ConeLength_1;
FGameplayTag SkillTree_LightningCoil_ConeLength_2;
FGameplayTag SkillTree_LightningCoil_ConeLength_3;
FGameplayTag SkillTree_LightningCoil_ConeLength_4;
FGameplayTag SkillTree_LightningCoil_ConeLength_5;
FGameplayTag SkillTree_LightningCoil_ConeAngle_1;
FGameplayTag SkillTree_LightningCoil_ConeAngle_2;
FGameplayTag SkillTree_LightningCoil_ConeAngle_3;
FGameplayTag SkillTree_LightningCoil_ConeAngle_4;
FGameplayTag SkillTree_LightningCoil_ConeAngle_5;
FGameplayTag SkillTree_LightningCoil_ChainCount_1;
FGameplayTag SkillTree_LightningCoil_ChainCount_2;
FGameplayTag SkillTree_LightningCoil_ChainCount_3;
FGameplayTag SkillTree_LightningCoil_ChainRadius_1;
FGameplayTag SkillTree_LightningCoil_ChainRadius_2;
FGameplayTag SkillTree_LightningCoil_ChainRadius_3;
FGameplayTag SkillTree_LightningCoil_ChainRadius_4;
FGameplayTag SkillTree_LightningCoil_ChainRadius_5;
FGameplayTag SkillTree_LightningCoil_DistanceLuckyRolls_1;
FGameplayTag SkillTree_LightningCoil_DistanceLuckyRolls_2;
FGameplayTag SkillTree_LightningCoil_DistanceLuckyRolls_3;
FGameplayTag SkillTree_LightningCoil_DamageLuckyRolls_1;
FGameplayTag SkillTree_LightningCoil_DamageLuckyRolls_2;
FGameplayTag SkillTree_LightningCoil_DamageLuckyRolls_3;
FGameplayTag SkillTree_LightningCoil_SkillSpeed_1;
FGameplayTag SkillTree_LightningCoil_SkillSpeed_2;

//SkillPassive
FGameplayTag SkillPassive_LightningCoil_IncreasedDamage;
FGameplayTag SkillPassive_LightningCoil_IncreasedCritChance;
FGameplayTag SkillPassive_LightningCoil_BoltAoERadius;
FGameplayTag SkillPassive_LightningCoil_CorrodedBuildup;
FGameplayTag SkillPassive_LightningCoil_MoreDamage;
FGameplayTag SkillPassive_LightningCoil_ConvergenceRate;
FGameplayTag SkillPassive_LightningCoil_ExtraBolts;
FGameplayTag SkillPassive_LightningCoil_ConeLength;
FGameplayTag SkillPassive_LightningCoil_ConeAngle;
FGameplayTag SkillPassive_LightningCoil_ChainCount;
FGameplayTag SkillPassive_LightningCoil_ChainRadius;
FGameplayTag SkillPassive_LightningCoil_DistanceLuckyRolls;
FGameplayTag SkillPassive_LightningCoil_DamageLuckyRolls;
FGameplayTag SkillPassive_LightningCoil_CastSpeed;

FGameplayTag SkillPassive_Impact_InitialRadius;
FGameplayTag SkillPassive_Impact_SecondaryRadius;
FGameplayTag SkillPassive_Impact_MaxChainDepth;
FGameplayTag SkillPassive_Impact_SecondaryDamageMultiplier;
FGameplayTag SkillPassive_Impact_ChainDelayModifier;

	//SkillTree — Leap
	FGameplayTag SkillTree_Leap_ExtraRange_1;
	FGameplayTag SkillTree_Leap_ExtraRange_2;
	FGameplayTag SkillTree_Leap_ExtraRange_3;
	FGameplayTag SkillTree_Leap_ExplosiveLanding_1;
	FGameplayTag SkillTree_Leap_NoWindup_1;

	//SkillTree — Leap (continued)
	FGameplayTag SkillTree_Leap_HeatCost_1;
	FGameplayTag SkillTree_Leap_HeatCost_2;
	FGameplayTag SkillTree_Leap_HeatCost_3;
	FGameplayTag SkillTree_Leap_HeatCost_4;
	FGameplayTag SkillTree_Leap_HeatCost_5;
	FGameplayTag SkillTree_Leap_HeatCost_6;
	FGameplayTag SkillTree_Leap_CooldownDuration_1;
	FGameplayTag SkillTree_Leap_CooldownDuration_2;
	FGameplayTag SkillTree_Leap_CooldownDuration_3;
	FGameplayTag SkillTree_Leap_CooldownDuration_4;

	//SkillPassive — Leap
	FGameplayTag SkillPassive_Leap_ExtraRange;
	FGameplayTag SkillPassive_Leap_ExplosiveLanding;
	FGameplayTag SkillPassive_Leap_NoWindup;
	FGameplayTag SkillPassive_Leap_HeatCost;
	FGameplayTag SkillPassive_Leap_CooldownDuration;
// ---- GRID TEST TAGS — DELETE AFTER PLACEMENT TESTING ----
FGameplayTag SkillTree_LightningCoil_Grid_N001; FGameplayTag SkillTree_LightningCoil_Grid_N002; FGameplayTag SkillTree_LightningCoil_Grid_N003; FGameplayTag SkillTree_LightningCoil_Grid_N004;
FGameplayTag SkillTree_LightningCoil_Grid_N005; FGameplayTag SkillTree_LightningCoil_Grid_N006; FGameplayTag SkillTree_LightningCoil_Grid_N007; FGameplayTag SkillTree_LightningCoil_Grid_N008;
FGameplayTag SkillTree_LightningCoil_Grid_N009; FGameplayTag SkillTree_LightningCoil_Grid_N010; FGameplayTag SkillTree_LightningCoil_Grid_N011; FGameplayTag SkillTree_LightningCoil_Grid_N012;
FGameplayTag SkillTree_LightningCoil_Grid_N013; FGameplayTag SkillTree_LightningCoil_Grid_N014; FGameplayTag SkillTree_LightningCoil_Grid_N015; FGameplayTag SkillTree_LightningCoil_Grid_N016;
FGameplayTag SkillTree_LightningCoil_Grid_N017; FGameplayTag SkillTree_LightningCoil_Grid_N018; FGameplayTag SkillTree_LightningCoil_Grid_N019; FGameplayTag SkillTree_LightningCoil_Grid_N020;
FGameplayTag SkillTree_LightningCoil_Grid_N021; FGameplayTag SkillTree_LightningCoil_Grid_N022; FGameplayTag SkillTree_LightningCoil_Grid_N023; FGameplayTag SkillTree_LightningCoil_Grid_N024;
FGameplayTag SkillTree_LightningCoil_Grid_N025; FGameplayTag SkillTree_LightningCoil_Grid_N026; FGameplayTag SkillTree_LightningCoil_Grid_N027; FGameplayTag SkillTree_LightningCoil_Grid_N028;
FGameplayTag SkillTree_LightningCoil_Grid_N029; FGameplayTag SkillTree_LightningCoil_Grid_N030; FGameplayTag SkillTree_LightningCoil_Grid_N031; FGameplayTag SkillTree_LightningCoil_Grid_N032;
FGameplayTag SkillTree_LightningCoil_Grid_N033; FGameplayTag SkillTree_LightningCoil_Grid_N034; FGameplayTag SkillTree_LightningCoil_Grid_N035; FGameplayTag SkillTree_LightningCoil_Grid_N036;
FGameplayTag SkillTree_LightningCoil_Grid_N037; FGameplayTag SkillTree_LightningCoil_Grid_N038; FGameplayTag SkillTree_LightningCoil_Grid_N039; FGameplayTag SkillTree_LightningCoil_Grid_N040;
FGameplayTag SkillTree_LightningCoil_Grid_N041; FGameplayTag SkillTree_LightningCoil_Grid_N042; FGameplayTag SkillTree_LightningCoil_Grid_N043; FGameplayTag SkillTree_LightningCoil_Grid_N044;
FGameplayTag SkillTree_LightningCoil_Grid_N045; FGameplayTag SkillTree_LightningCoil_Grid_N046; FGameplayTag SkillTree_LightningCoil_Grid_N047; FGameplayTag SkillTree_LightningCoil_Grid_N048;
FGameplayTag SkillTree_LightningCoil_Grid_N049; FGameplayTag SkillTree_LightningCoil_Grid_N050; FGameplayTag SkillTree_LightningCoil_Grid_N051; FGameplayTag SkillTree_LightningCoil_Grid_N052;
FGameplayTag SkillTree_LightningCoil_Grid_N053; FGameplayTag SkillTree_LightningCoil_Grid_N054; FGameplayTag SkillTree_LightningCoil_Grid_N055; FGameplayTag SkillTree_LightningCoil_Grid_N056;
FGameplayTag SkillTree_LightningCoil_Grid_N057; FGameplayTag SkillTree_LightningCoil_Grid_N058; FGameplayTag SkillTree_LightningCoil_Grid_N059; FGameplayTag SkillTree_LightningCoil_Grid_N060;
FGameplayTag SkillTree_LightningCoil_Grid_N061; FGameplayTag SkillTree_LightningCoil_Grid_N062; FGameplayTag SkillTree_LightningCoil_Grid_N063; FGameplayTag SkillTree_LightningCoil_Grid_N064;
FGameplayTag SkillTree_LightningCoil_Grid_N065; FGameplayTag SkillTree_LightningCoil_Grid_N066; FGameplayTag SkillTree_LightningCoil_Grid_N067; FGameplayTag SkillTree_LightningCoil_Grid_N068;
FGameplayTag SkillTree_LightningCoil_Grid_N069; FGameplayTag SkillTree_LightningCoil_Grid_N070; FGameplayTag SkillTree_LightningCoil_Grid_N071; FGameplayTag SkillTree_LightningCoil_Grid_N072;
FGameplayTag SkillTree_LightningCoil_Grid_N073; FGameplayTag SkillTree_LightningCoil_Grid_N074; FGameplayTag SkillTree_LightningCoil_Grid_N075; FGameplayTag SkillTree_LightningCoil_Grid_N076;
FGameplayTag SkillTree_LightningCoil_Grid_N077; FGameplayTag SkillTree_LightningCoil_Grid_N078; FGameplayTag SkillTree_LightningCoil_Grid_N079; FGameplayTag SkillTree_LightningCoil_Grid_N080;
FGameplayTag SkillTree_LightningCoil_Grid_N081; FGameplayTag SkillTree_LightningCoil_Grid_N082; FGameplayTag SkillTree_LightningCoil_Grid_N083; FGameplayTag SkillTree_LightningCoil_Grid_N084;
FGameplayTag SkillTree_LightningCoil_Grid_N085; FGameplayTag SkillTree_LightningCoil_Grid_N086; FGameplayTag SkillTree_LightningCoil_Grid_N087; FGameplayTag SkillTree_LightningCoil_Grid_N088;
FGameplayTag SkillTree_LightningCoil_Grid_N089; FGameplayTag SkillTree_LightningCoil_Grid_N090; FGameplayTag SkillTree_LightningCoil_Grid_N091; FGameplayTag SkillTree_LightningCoil_Grid_N092;
FGameplayTag SkillTree_LightningCoil_Grid_N093; FGameplayTag SkillTree_LightningCoil_Grid_N094; FGameplayTag SkillTree_LightningCoil_Grid_N095; FGameplayTag SkillTree_LightningCoil_Grid_N096;
FGameplayTag SkillTree_LightningCoil_Grid_N097; FGameplayTag SkillTree_LightningCoil_Grid_N098; FGameplayTag SkillTree_LightningCoil_Grid_N099; FGameplayTag SkillTree_LightningCoil_Grid_N100;
FGameplayTag SkillTree_LightningCoil_Grid_N101; FGameplayTag SkillTree_LightningCoil_Grid_N102; FGameplayTag SkillTree_LightningCoil_Grid_N103; FGameplayTag SkillTree_LightningCoil_Grid_N104;
FGameplayTag SkillTree_LightningCoil_Grid_N105; FGameplayTag SkillTree_LightningCoil_Grid_N106; FGameplayTag SkillTree_LightningCoil_Grid_N107; FGameplayTag SkillTree_LightningCoil_Grid_N108;
FGameplayTag SkillTree_LightningCoil_Grid_N109; FGameplayTag SkillTree_LightningCoil_Grid_N110; FGameplayTag SkillTree_LightningCoil_Grid_N111; FGameplayTag SkillTree_LightningCoil_Grid_N112;
FGameplayTag SkillTree_LightningCoil_Grid_N113; FGameplayTag SkillTree_LightningCoil_Grid_N114; FGameplayTag SkillTree_LightningCoil_Grid_N115; FGameplayTag SkillTree_LightningCoil_Grid_N116;
FGameplayTag SkillTree_LightningCoil_Grid_N117; FGameplayTag SkillTree_LightningCoil_Grid_N118; FGameplayTag SkillTree_LightningCoil_Grid_N119; FGameplayTag SkillTree_LightningCoil_Grid_N120;
FGameplayTag SkillTree_LightningCoil_Grid_N121; FGameplayTag SkillTree_LightningCoil_Grid_N122; FGameplayTag SkillTree_LightningCoil_Grid_N123; FGameplayTag SkillTree_LightningCoil_Grid_N124;
FGameplayTag SkillTree_LightningCoil_Grid_N125; FGameplayTag SkillTree_LightningCoil_Grid_N126; FGameplayTag SkillTree_LightningCoil_Grid_N127; FGameplayTag SkillTree_LightningCoil_Grid_N128;
FGameplayTag SkillTree_LightningCoil_Grid_N129; FGameplayTag SkillTree_LightningCoil_Grid_N130; FGameplayTag SkillTree_LightningCoil_Grid_N131; FGameplayTag SkillTree_LightningCoil_Grid_N132;
FGameplayTag SkillTree_LightningCoil_Grid_N133; FGameplayTag SkillTree_LightningCoil_Grid_N134; FGameplayTag SkillTree_LightningCoil_Grid_N135; FGameplayTag SkillTree_LightningCoil_Grid_N136;
FGameplayTag SkillTree_LightningCoil_Grid_N137; FGameplayTag SkillTree_LightningCoil_Grid_N138; FGameplayTag SkillTree_LightningCoil_Grid_N139; FGameplayTag SkillTree_LightningCoil_Grid_N140;
FGameplayTag SkillTree_LightningCoil_Grid_N141; FGameplayTag SkillTree_LightningCoil_Grid_N142; FGameplayTag SkillTree_LightningCoil_Grid_N143; FGameplayTag SkillTree_LightningCoil_Grid_N144;
FGameplayTag SkillTree_LightningCoil_Grid_N145; FGameplayTag SkillTree_LightningCoil_Grid_N146; FGameplayTag SkillTree_LightningCoil_Grid_N147; FGameplayTag SkillTree_LightningCoil_Grid_N148;
FGameplayTag SkillTree_LightningCoil_Grid_N149; FGameplayTag SkillTree_LightningCoil_Grid_N150; FGameplayTag SkillTree_LightningCoil_Grid_N151; FGameplayTag SkillTree_LightningCoil_Grid_N152;
FGameplayTag SkillTree_LightningCoil_Grid_N153; FGameplayTag SkillTree_LightningCoil_Grid_N154; FGameplayTag SkillTree_LightningCoil_Grid_N155; FGameplayTag SkillTree_LightningCoil_Grid_N156;
FGameplayTag SkillTree_LightningCoil_Grid_N157; FGameplayTag SkillTree_LightningCoil_Grid_N158; FGameplayTag SkillTree_LightningCoil_Grid_N159; FGameplayTag SkillTree_LightningCoil_Grid_N160;
FGameplayTag SkillTree_LightningCoil_Grid_N161; FGameplayTag SkillTree_LightningCoil_Grid_N162; FGameplayTag SkillTree_LightningCoil_Grid_N163; FGameplayTag SkillTree_LightningCoil_Grid_N164;
FGameplayTag SkillTree_LightningCoil_Grid_N165; FGameplayTag SkillTree_LightningCoil_Grid_N166; FGameplayTag SkillTree_LightningCoil_Grid_N167; FGameplayTag SkillTree_LightningCoil_Grid_N168;
FGameplayTag SkillTree_LightningCoil_Grid_N169; FGameplayTag SkillTree_LightningCoil_Grid_N170; FGameplayTag SkillTree_LightningCoil_Grid_N171; FGameplayTag SkillTree_LightningCoil_Grid_N172;
FGameplayTag SkillTree_LightningCoil_Grid_N173; FGameplayTag SkillTree_LightningCoil_Grid_N174; FGameplayTag SkillTree_LightningCoil_Grid_N175; FGameplayTag SkillTree_LightningCoil_Grid_N176;
FGameplayTag SkillTree_LightningCoil_Grid_N177; FGameplayTag SkillTree_LightningCoil_Grid_N178; FGameplayTag SkillTree_LightningCoil_Grid_N179; FGameplayTag SkillTree_LightningCoil_Grid_N180;
FGameplayTag SkillTree_LightningCoil_Grid_N181; FGameplayTag SkillTree_LightningCoil_Grid_N182; FGameplayTag SkillTree_LightningCoil_Grid_N183; FGameplayTag SkillTree_LightningCoil_Grid_N184;
FGameplayTag SkillTree_LightningCoil_Grid_N185; FGameplayTag SkillTree_LightningCoil_Grid_N186; FGameplayTag SkillTree_LightningCoil_Grid_N187; FGameplayTag SkillTree_LightningCoil_Grid_N188;
FGameplayTag SkillTree_LightningCoil_Grid_N189; FGameplayTag SkillTree_LightningCoil_Grid_N190; FGameplayTag SkillTree_LightningCoil_Grid_N191; FGameplayTag SkillTree_LightningCoil_Grid_N192;
FGameplayTag SkillTree_LightningCoil_Grid_N193; FGameplayTag SkillTree_LightningCoil_Grid_N194; FGameplayTag SkillTree_LightningCoil_Grid_N195; FGameplayTag SkillTree_LightningCoil_Grid_N196;
FGameplayTag SkillTree_LightningCoil_Grid_N197; FGameplayTag SkillTree_LightningCoil_Grid_N198; FGameplayTag SkillTree_LightningCoil_Grid_N199; FGameplayTag SkillTree_LightningCoil_Grid_N200;
FGameplayTag SkillTree_LightningCoil_Grid_N201; FGameplayTag SkillTree_LightningCoil_Grid_N202; FGameplayTag SkillTree_LightningCoil_Grid_N203; FGameplayTag SkillTree_LightningCoil_Grid_N204;
FGameplayTag SkillTree_LightningCoil_Grid_N205; FGameplayTag SkillTree_LightningCoil_Grid_N206; FGameplayTag SkillTree_LightningCoil_Grid_N207; FGameplayTag SkillTree_LightningCoil_Grid_N208;
FGameplayTag SkillTree_LightningCoil_Grid_N209; FGameplayTag SkillTree_LightningCoil_Grid_N210; FGameplayTag SkillTree_LightningCoil_Grid_N211; FGameplayTag SkillTree_LightningCoil_Grid_N212;
FGameplayTag SkillTree_LightningCoil_Grid_N213; FGameplayTag SkillTree_LightningCoil_Grid_N214; FGameplayTag SkillTree_LightningCoil_Grid_N215; FGameplayTag SkillTree_LightningCoil_Grid_N216;
FGameplayTag SkillTree_LightningCoil_Grid_N217; FGameplayTag SkillTree_LightningCoil_Grid_N218; FGameplayTag SkillTree_LightningCoil_Grid_N219; FGameplayTag SkillTree_LightningCoil_Grid_N220;
FGameplayTag SkillTree_LightningCoil_Grid_N221; FGameplayTag SkillTree_LightningCoil_Grid_N222; FGameplayTag SkillTree_LightningCoil_Grid_N223; FGameplayTag SkillTree_LightningCoil_Grid_N224;
FGameplayTag SkillTree_LightningCoil_Grid_N225; FGameplayTag SkillTree_LightningCoil_Grid_N226; FGameplayTag SkillTree_LightningCoil_Grid_N227; FGameplayTag SkillTree_LightningCoil_Grid_N228;
FGameplayTag SkillTree_LightningCoil_Grid_N229; FGameplayTag SkillTree_LightningCoil_Grid_N230; FGameplayTag SkillTree_LightningCoil_Grid_N231; FGameplayTag SkillTree_LightningCoil_Grid_N232;
FGameplayTag SkillTree_LightningCoil_Grid_N233; FGameplayTag SkillTree_LightningCoil_Grid_N234; FGameplayTag SkillTree_LightningCoil_Grid_N235; FGameplayTag SkillTree_LightningCoil_Grid_N236;
FGameplayTag SkillTree_LightningCoil_Grid_N237; FGameplayTag SkillTree_LightningCoil_Grid_N238; FGameplayTag SkillTree_LightningCoil_Grid_N239; FGameplayTag SkillTree_LightningCoil_Grid_N240;
FGameplayTag SkillTree_LightningCoil_Grid_N241; FGameplayTag SkillTree_LightningCoil_Grid_N242; FGameplayTag SkillTree_LightningCoil_Grid_N243; FGameplayTag SkillTree_LightningCoil_Grid_N244;
FGameplayTag SkillTree_LightningCoil_Grid_N245; FGameplayTag SkillTree_LightningCoil_Grid_N246; FGameplayTag SkillTree_LightningCoil_Grid_N247; FGameplayTag SkillTree_LightningCoil_Grid_N248;
FGameplayTag SkillTree_LightningCoil_Grid_N249; FGameplayTag SkillTree_LightningCoil_Grid_N250; FGameplayTag SkillTree_LightningCoil_Grid_N251; FGameplayTag SkillTree_LightningCoil_Grid_N252;
FGameplayTag SkillTree_LightningCoil_Grid_N253; FGameplayTag SkillTree_LightningCoil_Grid_N254; FGameplayTag SkillTree_LightningCoil_Grid_N255; FGameplayTag SkillTree_LightningCoil_Grid_N256;
// ---- END GRID TEST TAGS ----

private:
	static FFP_GameplayTags GameplayTags;
};