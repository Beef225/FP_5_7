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
//Damage
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


//Meta Attributes
//Damage
FGameplayTag MetaAttribute_IncomingDamage;

//XP
FGameplayTag MetaAttribute_IncomingXP;


//Misc Gameplay Tags
//Player Tags
FGameplayTag Player_Block_CursorTrace;
FGameplayTag Player_Block_InputHeld;
FGameplayTag Player_Block_InputReleased;

//Input Tags
FGameplayTag InputTag_LMB;
FGameplayTag InputTag_RMB;
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

//Effects
FGameplayTag Effects_HitReact;
	
	
	
private:
	static FFP_GameplayTags GameplayTags;
};