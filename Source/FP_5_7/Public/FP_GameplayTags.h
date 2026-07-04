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
FGameplayTag Damage_Increased_Grenade;
FGameplayTag Damage_More_Grenade;
FGameplayTag Damage_Increased_Shotgun;
FGameplayTag Damage_More_Shotgun;
FGameplayTag Damage_Increased_Drone;
FGameplayTag Damage_More_Drone;
FGameplayTag Damage_Increased_Axe;
FGameplayTag Damage_More_Axe;
FGameplayTag Damage_Increased_Sword;
FGameplayTag Damage_More_Sword;
FGameplayTag Damage_Increased_Club;
FGameplayTag Damage_More_Club;
FGameplayTag Damage_Increased_Unarmed;
FGameplayTag Damage_More_Unarmed;
FGameplayTag Damage_Increased_Launcher;
FGameplayTag Damage_More_Launcher;
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


//SkillTagMods
//SkillTagMod
FGameplayTag SkillTagMod_Pistol;
FGameplayTag SkillTagMod_Rifle;
FGameplayTag SkillTagMod_Shotgun;
FGameplayTag SkillTagMod_Grenade;
FGameplayTag SkillTagMod_Drone;
FGameplayTag SkillTagMod_Axe;
FGameplayTag SkillTagMod_Sword;
FGameplayTag SkillTagMod_Club;
FGameplayTag SkillTagMod_Unarmed;
FGameplayTag SkillTagMod_Launcher;
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

//Skills
//Skills
FGameplayTag Skills_Energy_Firebolt;
FGameplayTag Skills_Radiation_Microwave;
FGameplayTag Skills_Chemical_LightningCoil;
	
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