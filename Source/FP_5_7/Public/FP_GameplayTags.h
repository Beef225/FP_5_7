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

//Item
FGameplayTag Item_Quantity;
FGameplayTag Item_Rarity;
FGameplayTag Item_WattsQuantity;


//Meta Attributes

//Misc Gameplay Tags
//Skills Meta
FGameplayTag Skill_Slot_1;
FGameplayTag Skill_Slot_2;
FGameplayTag Skill_Slot_3;
FGameplayTag Skill_Slot_4;
FGameplayTag Skill_Slot_5;

//Player Tags
FGameplayTag Player_Block_CursorTrace;
FGameplayTag Player_Block_InputHeld;
FGameplayTag Player_Block_InputReleased;
	
	
	
private:
	static FFP_GameplayTags GameplayTags;
};