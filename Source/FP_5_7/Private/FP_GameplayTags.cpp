// Copyright JG


#include "FP_GameplayTags.h"

#include "GameplayTagsManager.h"


FFP_GameplayTags FFP_GameplayTags::GameplayTags;

void FFP_GameplayTags::InitializeGameplayTags()
{

		//Vital Attributes
	GameplayTags.Attributes_Vital_HitPoints = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Vital.HitPoints"), 
		FString("Character Health")
		);
	GameplayTags.Attributes_Vital_MaxHitPoints = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Vital.MaxHitPoints"), 
		FString("Character Maximum Health")
		);
	GameplayTags.Attributes_Vital_Heat = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Vital.Heat"), 
		FString("Character Primary Resource")
		);
	GameplayTags.Attributes_Vital_MaxHeatThreshold = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Vital.MaxHeatThreshold"), 
		FString("Limit of Heat Generation Before Damage")
		);
	GameplayTags.Attributees_VitalMinHeatThreshold = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributees.VitalMinHeatThreshold"), 
		FString("Limit of Heat Generation Before Freezing")
		);


	//Primary Attributes
	GameplayTags.Attributes_Primary_Might = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Might"), 
		FString("Measure of Strength")
		);
	GameplayTags.Attributes_Primary_Resonance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Resonance"), 
		FString("Measure of Attunement")
		);
	GameplayTags.Attributes_Primary_Agility = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Agility"), 
		FString("Measure of Speed")
		);
	GameplayTags.Attributes_Primary_Fortitude = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Fortitude"), 
		FString("Measure of Heartiness")
		);


	//Secondary Attributes
	//Health
	GameplayTags.Health_Increased = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Health.Increased"),
		FString("Increased maximum hit points")
		);
	GameplayTags.Health_More = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Health.More"),
		FString("More maximum hit points")
		);

	//Damage
	GameplayTags.Damage_Increased_Generic = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Generic"), 
		FString("Increases all damage")
		);
	GameplayTags.Damage_More_Generic = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Generic"), 
		FString("More Damage")
		);
	GameplayTags.Damage_Added_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Added.Physical"), 
		FString("Adds flat physical damage")
		);
	GameplayTags.Damage_Increased_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Physical"), 
		FString("Increased physical damage")
		);
	GameplayTags.Damage_More_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Physical"), 
		FString("More physical damage")
		);
	GameplayTags.Damage_Added_Explosive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Added.Explosive"), 
		FString("Adds flat Explosive damage")
		);
	GameplayTags.Damage_Increased_Explosive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Explosive"), 
		FString("Increased Explosive damage")
		);
	GameplayTags.Damage_More_Explosive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Explosive"), 
		FString("More Explosive damage")
		);
	GameplayTags.Damage_Added_Radiation = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Added.Radiation"), 
		FString("Adds flat Radiation damage")
		);
	GameplayTags.Damage_Increased_Radiation = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Radiation"), 
		FString("Increased Radiation damage")
		);
	GameplayTags.Damage_More_Radiation = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Radiation"), 
		FString("More Radiation damage")
		);
	GameplayTags.Damage_Added_Chemical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Added.Chemical"), 
		FString("Adds flat Chemical damage")
		);
	GameplayTags.Damage_Increased_Chemical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Chemical"), 
		FString("Increased Chemical damage")
		);
	GameplayTags.Damage_More_Chemical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Chemical"), 
		FString("More Chemical damage")
		);
	GameplayTags.Damage_Added_Energy = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Added.Energy"), 
		FString("Adds flat Energy damage")
		);
	GameplayTags.Damage_Increased_Energy = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Energy"), 
		FString("Increased Energy damage")
		);
	GameplayTags.Damage_More_Energy = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Energy"), 
		FString("More Energy damage")
		);
	GameplayTags.Damage_Increased_Pistol = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Pistol"), 
		FString("Increased Pistol Damage")
		);
	GameplayTags.Damage_More_Pistol = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Pistol"), 
		FString("More Pistol Damage")
		);
	GameplayTags.Damage_Increased_Rifle = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Rifle"), 
		FString("Increased Rifle damage")
		);
	GameplayTags.Damage_More_Rifle = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Rifle"), 
		FString("More Rifle damage")
		);
	GameplayTags.Damage_Increased_Shotgun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Shotgun"),
		FString("Increased Shotgun damage")
		);
	GameplayTags.Damage_More_Shotgun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Shotgun"),
		FString("More Shotgun damage")
		);
	GameplayTags.Damage_Increased_DroneController = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.DroneController"),
		FString("Increased Drone Controller damage")
		);
	GameplayTags.Damage_More_DroneController = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.DroneController"),
		FString("More Drone Controller damage")
		);
	GameplayTags.Damage_Increased_Axe = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Axe"),
		FString("Increased Axe damage")
		);
	GameplayTags.Damage_More_Axe = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Axe"),
		FString("More Axe damage")
		);
	GameplayTags.Damage_Increased_Sword = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Sword"),
		FString("Increased Sword damage")
		);
	GameplayTags.Damage_More_Sword = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Sword"),
		FString("More Sword damage")
		);
	GameplayTags.Damage_Increased_Mace = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Mace"),
		FString("Increased Mace damage")
		);
	GameplayTags.Damage_More_Mace = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Mace"),
		FString("More Mace damage")
		);
	GameplayTags.Damage_Increased_Gauntlets = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Gauntlets"),
		FString("Increased Gauntlets damage")
		);
	GameplayTags.Damage_More_Gauntlets = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Gauntlets"),
		FString("More Gauntlets damage")
		);
	GameplayTags.Damage_Increased_Ordnance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Ordnance"),
		FString("Increased Ordnance damage")
		);
	GameplayTags.Damage_More_Ordnance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Ordnance"),
		FString("More Ordnance damage")
		);
	GameplayTags.Damage_Increased_Conduit = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Conduit"),
		FString("Increased Conduit damage")
		);
	GameplayTags.Damage_More_Conduit = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Conduit"),
		FString("More Conduit damage")
		);
	GameplayTags.Damage_Increased_Accelerator = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Accelerator"),
		FString("Increased Accelerator damage")
		);
	GameplayTags.Damage_More_Accelerator = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Accelerator"),
		FString("More Accelerator damage")
		);
	GameplayTags.Damage_Increased_Spear = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Spear"),
		FString("Increased Spear damage")
		);
	GameplayTags.Damage_More_Spear = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Spear"),
		FString("More Spear damage")
		);
	GameplayTags.Damage_Increased_Precision = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Precision"),
		FString("Increased Precision damage")
		);
	GameplayTags.Damage_More_Precision = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Precision"),
		FString("More Precision damage")
		);
	GameplayTags.Damage_Increased_Projector = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Projector"),
		FString("Increased Projector damage")
		);
	GameplayTags.Damage_More_Projector = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Projector"),
		FString("More Projector damage")
		);
	GameplayTags.Damage_Increased_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Melee"), 
		FString("Increased Melee Damage")
		);
	GameplayTags.Damage_More_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Melee"), 
		FString("More Melee damage")
		);
	GameplayTags.Damage_Increased_Projectile = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.Projectile"), 
		FString("Increased Projectile damage")
		);
	GameplayTags.Damage_More_Projectile = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.Projectile"), 
		FString("More Projectile damage")
		);
	GameplayTags.Damage_Increased_AoE = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Increased.AoE"), 
		FString("Increased Area damage")
		);
	GameplayTags.Damage_More_AoE = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.More.AoE"), 
		FString("More Area damage")
		);

	//Resistance
	GameplayTags.Resistance_Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Resistance.Damage.Physical"), 
		FString("Physical damage reduction")
		);
	GameplayTags.Resistance_Max_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Resistance.Max.Physical"), 
		FString("Physical Resistance maximum")
		);
	GameplayTags.Resistance_Damage_Explosive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Resistance.Damage.Explosive"), 
		FString("Explosive damage reduction")
		);
	GameplayTags.Resistance_Max_Explosive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Resistance.Max.Explosive"), 
		FString("Explosive Resistance maximum")
		);
	GameplayTags.Resistance_Damage_Radiation = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Resistance.Damage.Radiation"), 
		FString("Radiation damage reduction")
		);
	GameplayTags.Resistance_Max_Radiation = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Resistance.Max.Radiation"), 
		FString("Radiation Resistance maximum")
		);
	GameplayTags.Resistance_Damage_Chemical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Resistance.Damage.Chemical"), 
		FString("Chemical damage reduction")
		);
	GameplayTags.Resistance_Max_Chemical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Resistance.Max.Chemical"), 
		FString("Chemical Resistance maximum")
		);
	GameplayTags.Resistance_Damage_Energy = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Resistance.Damage.Energy"), 
		FString("Energy damage reduction")
		);
	GameplayTags.Resistance_Max_Energy = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Resistance.Max.Energy"), 
		FString("Energy Resistance maximum")
		);
	GameplayTags.Resistance_Damage_Overheat = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Resistance.Damage.Overheat"), 
		FString("Overheat damage reduction")
		);
	GameplayTags.Resistance_Max_Overheat = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Resistance.Max.Overheat"), 
		FString("Overheat Resistance maximum")
		);

	//Critical
	GameplayTags.Critical_StrikeChance_Base = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Critical.StrikeChance.Base"), 
		FString("Base Critical Strike Chance")
		);
	GameplayTags.Critical_StrikeChance_Increased = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Critical.StrikeChance.Increased"), 
		FString("Increased Critical Strike Chance")
		);
	GameplayTags.Critical_StrikeMultiplier = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Critical.StrikeMultiplier"), 
		FString("Critical Strike Multiplier")
		);

	//Defence
	GameplayTags.Defence_Armour = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Defence.Armour"), 
		FString("Armour Against All Hit Damage")
		);
	GameplayTags.Defence_DodgeChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Defence.DodgeChance"), 
		FString("Chance to avoid hit damage")
		);
	GameplayTags.Defence_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Defence.BlockChance"), 
		FString("Chance to Block Damage")
		);
	GameplayTags.Defence_BlockedDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Defence.BlockedDamage"), 
		FString("Amount of Damage Mitigated on Block")
		);
	GameplayTags.Defence_DeferredDamageAmount = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Defence.DeferredDamageAmount"), 
		FString("Defer Part of Hit Damage Over Time")
		);
	GameplayTags.Defence_DeferredDamageTime = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Defence.DeferredDamageTime"), 
		FString("Take Deferred Damage Over Time Frame")
		);
	GameplayTags.Defence_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Defence.HealthRegeneration"), 
		FString("Regenerate a Portion of Health Per Second")
		);
	GameplayTags.Defense_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Defense.CriticalHitResistance"), 
		FString("Reduced Damage Taken from Critical Strikes")
		);
	GameplayTags.Defense_ReducedDamage_Projectiles = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Defense.ReducedDamage.Projectiles"), 
		FString("Reduced Damage From Projectiles")
		);

	//Defense
	GameplayTags.Defense_IncreasedArmour = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Defense.IncreasedArmour"), 
		FString("Increased Armour")
		);
	GameplayTags.Defense_ReducedDamage_AoE = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Defense.ReducedDamage.AoE"), 
		FString("Reduced Damaged from Area of Effect Skills")
		);
	GameplayTags.Defense_ReduceeDamage_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Defense.ReduceeDamage.Melee"), 
		FString("Reduced Damage from Melee")
		);

	//Heat
	GameplayTags.Heat_AmbientTemperature = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Heat.AmbientTemperature"), 
		FString("Environment Tempurature")
		);
	GameplayTags.Heat_ThermalMass = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Heat.ThermalMass"), 
		FString("Heat Capaicity of a Player, Modified by Gear")
		);
	GameplayTags.Heat_CoolingCoefficient = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Heat.CoolingCoefficient"),
		FString("Rate of Heat Dissapation")
		);
	GameplayTags.Heat_SkillCost = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Heat.SkillCost"),
		FString("Heat cost of activating skills (decimal fraction, bidirectional: negative = reduced cost, positive = increased cost)")
		);

	//Debuff
	GameplayTags.Debuff_Effectiveness_Bruised = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Effectiveness.Bruised"), 
		FString("Bruised Effectiveness")
		);
	GameplayTags.Debuff_Effectiveness_Concussed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Effectiveness.Concussed"), 
		FString("Concussed Effectiveness")
		);
	GameplayTags.Debuff_Effectiveness_Irradiated = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Effectiveness.Irradiated"), 
		FString("Irradiated Effectiveness")
		);
	GameplayTags.Debuff_Effectiveness_Corroded = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Effectiveness.Corroded"), 
		FString("Corroded Effectiveness")
		);
	GameplayTags.Debuff_Effectiveness_ThermalDestabalization = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Effectiveness.ThermalDestabalization"), 
		FString("Thermal Destabalization Effectiveness")
		);
	GameplayTags.Debuff_Threshold_Bruised = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Threshold.Bruised"), 
		FString("Bruised Buildup before applying debuff")
		);
	GameplayTags.Debuff_Threshold_Concussed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Threshold.Concussed"), 
		FString("Concussed Buildup before applying debuff")
		);
	GameplayTags.Debuff_Threshold_Irradiated = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Threshold.Irradiated"), 
		FString("Irradiated Buildup before applying debuff")
		);
	GameplayTags.Debuff_Threshold_Corroded = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Threshold.Corroded"), 
		FString("Corroded Buildup before applying debuff")
		);
	GameplayTags.Debuff_Duration_Bruised = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Duration.Bruised"), 
		FString("Time Bruised drops off")
		);
	GameplayTags.Debuff_Duration_Concussed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Duration.Concussed"), 
		FString("Time Concussed drops off")
		);
	GameplayTags.Debuff_Duration_Irradiated = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Duration.Irradiated"), 
		FString("Time Irradiated drops off")
		);
	GameplayTags.Debuff_Duration_Corroded = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Duration.Corroded"), 
		FString("Time Corroded drops off")
		);
	GameplayTags.Debuff_ThermalInsulation = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.ThermalInsulation"), 
		FString("Change the amount of heat damage")
		);

	//AoE
	GameplayTags.AoE_Multiplier = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("AoE.Multiplier"), 
		FString("Increase the Area of Effect")
		);
	GameplayTags.AoE_AdditionalRadius = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("AoE.AdditionalRadius"), 
		FString("Increase the base radius of AoE")
		);

	//Speed
	GameplayTags.Speed_Movement = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Speed.Movement"), 
		FString("Movement Speed")
		);
	GameplayTags.Speed_Skill = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Speed.Skill"), 
		FString("Skill Speed")
		);
	GameplayTags.Speed_Movement_DuringSkill = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Speed.Movement.DuringSkill"), 
		FString("Multiplier of Movment speed during Skills that allow Movement")
		);
	GameplayTags.Speed_Projectile = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Speed.Projectile"),
		FString("Increased Projectile Speed")
		);

	//Duration
	GameplayTags.Duration_Skill = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Duration.Skill"),
		FString("Skill duration scalar (delta from neutral 0) affecting skill-defined timers such as chain/detonation delays")
		);

	//Item
	GameplayTags.Item_Quantity = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Item.Quantity"), 
		FString("Drop More Items")
		);
	GameplayTags.Item_Rarity = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Item.Rarity"), 
		FString("Drop Better Items")
		);
	GameplayTags.Item_WattsQuantity = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Item.WattsQuantity"), 
		FString("Drop More Watts")
		);

	//DoT
	GameplayTags.DoT_Increased = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("DoT.Increased"), 
		FString("Increased Damage OverTime")
		);
	GameplayTags.DoT_Multiplier = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("DoT.Multiplier"), 
		FString("Damage Over Time Multiplier")
		);

	//Penetration
	GameplayTags.Penetration_Armour = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Penetration.Armour"), 
		FString("Ignores Armour during damage Calc")
		);
	GameplayTags.Penetration_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Penetration.Physical"), 
		FString("Subtract from Max of Physical Res")
		);
	GameplayTags.Penetration_Explosive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Penetration.Explosive"), 
		FString("Subtract from Max of Explosive Res")
		);
	GameplayTags.Penetration_Radiation = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Penetration.Radiation"), 
		FString("Subtract from Max of Radiation Res")
		);
	GameplayTags.Penetration_Chemical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Penetration.Chemical"), 
		FString("Subtract from Max of Chemical Res")
		);
	GameplayTags.Penetration_Energy = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Penetration.Energy"), 
		FString("Subtract from Max of Energy Res")
		);

	//Projectile
	GameplayTags.Projectile_Pierce = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Projectile.Pierce"), 
		FString("How many targets Projectile can peirce before being absorbed")
		);
	GameplayTags.Projectile_Bounce = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Projectile.Bounce"), 
		FString("How man targets Projectile changes to within AoE Radius")
		);
	GameplayTags.Projectile_Count = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Projectile.Count"), 
		FString("How many added Projectiles to Skill")
		);

	//Conversion
	GameplayTags.Conversion_Damage_PhysicalToEnergy = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Conversion.Damage.PhysicalToEnergy"), 
		FString("Convert Physical Damage Dealt to Energy Damage")
		);
	GameplayTags.Conversion_Damage_ChemicalToRadiation = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Conversion.Damage.ChemicalToRadiation"), 
		FString("Convert Chemical Damage to Radiation Damage")
		);
	GameplayTags.Conversion_Damage_ExplosiveToPhysical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Conversion.Damage.ExplosiveToPhysical"), 
		FString("Convert Explosive Damage to Physical Damage")
		);
	GameplayTags.Conversion_Defense_EnergyToRadiation = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Conversion.Defense.EnergyToRadiation"), 
		FString("Convert Incoming Energy Damage as Radiation Damage ")
		);
	GameplayTags.Conversion_Defense_ChemicalToExplosive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Conversion.Defense.ChemicalToExplosive"), 
		FString("Convert Incoming Chemical Damage asExplosive Damage")
		);

	//Conditional
	GameplayTags.Conditional_Damage_IncreasedAgainstBruised = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Conditional.Damage.IncreasedAgainstBruised"), 
		FString("Deal Increased Damage if Target is Bruised")
		);
	GameplayTags.Conditional_Damage_IncreasedAgainstConcussed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Conditional.Damage.IncreasedAgainstConcussed"), 
		FString("Deal Increased Damage if Target is Concussed")
		);
	GameplayTags.Conditional_Damage_IncreasedAgainstIrradiated = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Conditional.Damage.IncreasedAgainstIrradiated"), 
		FString("Deal Increased Damage if Target is Irradiated")
		);
	GameplayTags.Conditional_Damage_IncreasedAgainstCorroded = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Conditional.Damage.IncreasedAgainstCorroded"), 
		FString("Deal Increased Damage if Target is Corroded")
		);
	GameplayTags.Conditional_Damage_IncreasedAgainstOverheated = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Conditional.Damage.IncreasedAgainstOverheated"), 
		FString("Deal Increased Damage if Target is Overheated")
		);
	GameplayTags.Conditional_Damage_IncreasedAgainstFrozen = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Conditional.Damage.IncreasedAgainstFrozen"), 
		FString("Deal Increased Damage if Target is Frozen")
		);
	GameplayTags.Conditional_Damage_PerHeat = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Conditional.Damage.PerHeat"), 
		FString("Deal Increased Damage Per Heat Away From Stable Tempurature")
		);

	//Recovery
	GameplayTags.Recovery_Life_OnHit = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Recovery.Life.OnHit"), 
		FString("Gain Life on Hit")
		);
	GameplayTags.Recovery_Life_OnKill = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Recovery.Life.OnKill"), 
		FString("Gain Life on Kill")
		);


	//Meta Attributes
	//Damage
	GameplayTags.MetaAttribute_IncomingDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("MetaAttribute.IncomingDamage"), 
		FString("Used for Calculating Incoming Damage")
		);

	//XP
	GameplayTags.MetaAttribute_IncomingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("MetaAttribute.IncomingXP"),
		FString("Used to add to Player Experience")
		);

	//DoT duration
	GameplayTags.SetByCaller_DotDuration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SetByCaller.DotDuration"),
		FString("SetByCaller tag for damage-over-time duration in seconds")
		);

	//Deferred damage per tick
	GameplayTags.SetByCaller_DeferredDamagePerTick = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SetByCaller.DeferredDamagePerTick"),
		FString("SetByCaller tag for deferred damage — damage applied per tick (pre-scaled, no defences)")
		);

	//Deferred damage gameplay event
	GameplayTags.GameplayEvent_DeferredDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayEvent.DeferredDamage"),
		FString("Sent by ExecCalc_Damage when a deferred damage split is computed")
		);


	//Misc Gameplay Tags
	//Player Tags
	GameplayTags.Player_Block_CursorTrace = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.CursorTrace"), 
		FString("Block tracing the cursor")
		);
	GameplayTags.Player_Block_InputHeld = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.InputHeld"), 
		FString("Block Input Held callback for input")
		);
	GameplayTags.Player_Block_InputReleased = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.InputReleased"), 
		FString("Block Input Released callback for input")
		);

	//Input Tags
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.LMB"), 
		FString("InputTag for Left Mouse Button")
		);
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.RMB"),
		FString("InputTag for Right Mouse Button")
		);
	GameplayTags.InputTag_Space = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Space"),
		FString("InputTag for Spacebar — movement abilities such as dodge roll")
		);
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.1"), 
		FString("InputTag for Key 1")
		);
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.2"), 
		FString("InputTag for Key 2")
		);
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.3"), 
		FString("InputTag for Key 3")
		);
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.4"), 
		FString("InputTag for Key 4")
		);
	GameplayTags.InputTag_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.5"), 
		FString("InputTag for Key 5")
		);
	GameplayTags.InputTag_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.6"), 
		FString("InputTag for Key 6")
		);
	GameplayTags.InputTag_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.7"), 
		FString("InputTag for Key 7")
		);
	GameplayTags.InputTag_8 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.8"), 
		FString("InputTag for Key 8")
		);
	GameplayTags.InputTag_9 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.9"), 
		FString("InputTag for Key 9")
		);
	GameplayTags.InputTag_10 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.10"), 
		FString("InputTag for Key 10")
		);

	//InputTags
	GameplayTags.InputTag_MMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.MMB"), 
		FString("InputTag for Middle Mouse Button")
		);
	GameplayTags.InputTag_Menu_Character = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Menu.Character"), 
		FString("Open Character Menu")
		);
	GameplayTags.InputTag_Menu_Skills = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Menu.Skills"), 
		FString("Open Skills Menu")
		);
	GameplayTags.InputTag_Menu_Options = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Menu.Options"), 
		FString("Open Option Menu")
		);
	GameplayTags.InputTag_Menu_Inventory = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Menu.Inventory"), 
		FString("Open Inventory")
		);
	GameplayTags.InputTag_Menu_PassiveTree = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Menu.PassiveTree"), 
		FString("Open Passive Tree")
		);
	GameplayTags.InputTag_Menu_WorldMap = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Menu.WorldMap"), 
		FString("Open World Map ")
		);
	GameplayTags.InputTag_Menu_LocalMap = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Menu.LocalMap"), 
		FString("Toggle Local Map overlay")
		);
	GameplayTags.InputTag_Menu_Stash = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Menu.Stash"), 
		FString("Open Stash ")
		);

	//Events
	GameplayTags.Event_Montage_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Montage.1"), 
		FString("Notify Montage 1")
		);
	GameplayTags.Event_Montage_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Montage.2"), 
		FString("Notify Montage 2")
		);
	GameplayTags.Event_Montage_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Montage.3"), 
		FString("Notify Montage 3")
		);
	GameplayTags.Event_Montage_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Montage.4"), 
		FString("Notify Montage 4")
		);
	GameplayTags.Event_Montage_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Event.Montage.5"), 
		FString("Notify Montage 5")
		);

	//Damage
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage"), 
		FString("Damage")
		);
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Physical"), 
		FString("Physical Damage")
		);
	GameplayTags.Damage_Explosive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Explosive"), 
		FString("Explosive Damage")
		);
	GameplayTags.Damage_Radiation = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Radiation"), 
		FString("Radiation Damage")
		);
	GameplayTags.Damage_Chemical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Chemical"), 
		FString("Chemical Damage")
		);
	GameplayTags.Damage_Energy = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Energy"), 
		FString("Energy Damage")
		);
	GameplayTags.Damage_Overheat = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Overheat"), 
		FString("Overheat Damage")
		);

	//Effects
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Effects.HitReact"), 
		FString("Hit React Montage")
		);
	GameplayTags.Effects_DeathReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Effects.DeathReact"), 
		FString("Death React Montage")
		);

	//Skills
	GameplayTags.Skills_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Skills.Melee"), 
		FString("Melee Skill")
		);
	GameplayTags.Skill_Ranged = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Skill.Ranged"), 
		FString("Ranged Skill")
		);
	GameplayTags.Skill_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Skill.Summon"), 
		FString("Summon Skill")
		);
	GameplayTags.Skill_Movement = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Skill.Movement"),
		FString("Movement Skill")
		);
	GameplayTags.Skills_Movement_DodgeRoll = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Skills.Movement.DodgeRoll"),
		FString("Dodge Roll — burst movement in the player's input direction")
		);
	GameplayTags.Skills_Movement_Leap = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Skills.Movement.Leap"),
		FString("Leap — teleport to mouse cursor position up to max range")
		);
	GameplayTags.Skills_Rotation_Cancelled = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Skills.Rotation.Cancelled"),
		FString("Suppresses mouse-facing rotation while an ability is active")
		);

	GameplayTags.Skill_MoveSpeed_Diminished = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Skill.MoveSpeed.Diminished"), 
		FString("Reduce Skill Speed by Attribute")
		);
	GameplayTags.Skill_MoveSpeed_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Skill.MoveSpeed.None"), 
		FString("No Movement Speed When Tag Applied")
		);
	GameplayTags.Skills_Status_Equipped = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Skills.Status.Equipped"), 
		FString("Added Tag to Equipped Skills")
		);

	//Montage
	GameplayTags.Montage_Skill_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Skill.Weapon"), 
		FString("Weapon Montage")
		);
	GameplayTags.Montage_Skill_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Skill.LeftHand"), 
		FString("LeftHand Montage")
		);
	GameplayTags.Montage_Skill_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Skill.RightHand"), 
		FString("RightHand Montage")
		);
	GameplayTags.Montage_Skill_Tail = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Skill.Tail"), 
		FString("Tail Montage")
		);
	GameplayTags.Montage_Skill_Mouth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Skill.Mouth"), 
		FString("Mouth Montage")
		);

	//State
	GameplayTags.State_Frozen = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Frozen"), 
		FString("Frozen Flag for Anims and ASC")
		);
	GameplayTags.State_Chilled = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Chilled"), 
		FString("Chilled Flag  for Anims and ASC")
		);
	GameplayTags.State_Overheat = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Overheat"), 
		FString("Overheat Flag  for Anims and ASC")
		);

	//SetByCaller
	GameplayTags.SetByCaller_Attribute_IncomingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SetByCaller.Attribute.IncomingXP"),
		FString("Set By Caller Tag for IncomingXP")
		);
	GameplayTags.SetByCaller_DodgeRoll_MoveSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SetByCaller.DodgeRoll.MoveSpeed"),
		FString("SetByCaller tag driving the dodge roll movement speed ramp GE")
		);

	GameplayTags.SetByCaller_SkillBonus_IncreasedDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SetByCaller.SkillBonus.IncreasedDamage"),        FString("Per-skill passive increased damage bonus"));
	GameplayTags.SetByCaller_SkillBonus_MoreDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SetByCaller.SkillBonus.MoreDamage"),             FString("Per-skill passive more damage bonus"));
	GameplayTags.SetByCaller_SkillBonus_IncreasedChemicalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SetByCaller.SkillBonus.IncreasedChemicalDamage"),FString("Per-skill passive increased chemical damage bonus"));
	GameplayTags.SetByCaller_SkillBonus_MoreChemicalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SetByCaller.SkillBonus.MoreChemicalDamage"),     FString("Per-skill passive more chemical damage bonus"));
	GameplayTags.SetByCaller_SkillBonus_CritChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SetByCaller.SkillBonus.CritChance"),             FString("Per-skill passive flat crit chance bonus"));
	GameplayTags.SetByCaller_SkillBonus_IncreasedCritChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SetByCaller.SkillBonus.IncreasedCritChance"),    FString("Per-skill passive increased crit chance bonus"));
	GameplayTags.SetByCaller_SkillBonus_CritMultiplier = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SetByCaller.SkillBonus.CritMultiplier"),         FString("Per-skill passive crit multiplier bonus"));

	//SkillTagMods
	//SkillTagMod
	GameplayTags.SkillTagMod_Pistol = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Pistol"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Rifle = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Rifle"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Shotgun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Shotgun"), 
		FString("")
		);
	GameplayTags.SkillTagMod_DroneController = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.DroneController"),
		FString("")
		);
	GameplayTags.SkillTagMod_Axe = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Axe"),
		FString("")
		);
	GameplayTags.SkillTagMod_Sword = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Sword"),
		FString("")
		);
	GameplayTags.SkillTagMod_Mace = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Mace"),
		FString("")
		);
	GameplayTags.SkillTagMod_Gauntlets = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Gauntlets"),
		FString("")
		);
	GameplayTags.SkillTagMod_Ordnance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Ordnance"),
		FString("")
		);
	GameplayTags.SkillTagMod_Conduit = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Conduit"),
		FString("")
		);
	GameplayTags.SkillTagMod_Accelerator = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Accelerator"),
		FString("")
		);
	GameplayTags.SkillTagMod_Spear = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Spear"),
		FString("")
		);
	GameplayTags.SkillTagMod_Precision = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Precision"),
		FString("")
		);
	GameplayTags.SkillTagMod_Projector = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Projector"),
		FString("")
		);
	GameplayTags.SkillTagMod_MeleeWeapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.MeleeWeapon"),
		FString("Weapon-class: inherently close-range weapon (Mace/Axe/Sword/Gauntlets/Spear). Distinct from the SkillTagMod.Melee hit-type tag.")
		);
	GameplayTags.SkillTagMod_RangedWeapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.RangedWeapon"),
		FString("Weapon-class: inherently long-range weapon (Pistol/Rifle/Shotgun/Accelerator/Precision/etc). Distinct from the SkillTagMod.Projectile hit-type tag.")
		);
	GameplayTags.SkillTagMod_Projectile = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Projectile"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Melee"), 
		FString("")
		);
	GameplayTags.SkillTagMod_AoE = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.AoE"), 
		FString("")
		);
	GameplayTags.SkillTagMod_DamageOverTime = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.DamageOverTime"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Bomb = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Bomb"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Physical"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Explosive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Explosive"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Chemical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Chemical"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Radiation = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Radiation"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Energy = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Energy"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Bruising = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Bruising"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Concussing = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Concussing"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Irradiating = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Irradiating"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Corroding = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Corroding"), 
		FString("")
		);
	GameplayTags.SkillTagMod_ThermalDestabilizing = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.ThermalDestabilizing"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Piercing = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Piercing"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Channeling = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Channeling"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Duration"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Cooldown = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Cooldown"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Trigger = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Trigger"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Movement = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Movement"), 
		FString("")
		);
	GameplayTags.SkillTagMod_Buff = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTagMod.Buff"), 
		FString("")
		);


	//Skills
	//Skills
	GameplayTags.Skills_Energy_Firebolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Skills.Energy.Firebolt"),
		FString("Skill Name Firebolt")
		);
	GameplayTags.Skills_Radiation_Microwave = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Skills.Radiation.Microwave"),
		FString("Skill Name Microwave")
		);

	GameplayTags.Skills_Chemical_LightningCoil = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Skills.Chemical.LightningCoil"),
		FString("Lightning Coil - fires arc bolts in a cone from the weapon tip, each with a circular AoE impact")
		);
	GameplayTags.Skills_Physical_Impact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Skills.Physical.Impact"),
		FString("Impact - large circular AoE at the weapon tip; hit enemies detonate their own smaller chained AoE after a delay")
		);

	// -------------------------------------------------------------------------
	// Inventory
	// -------------------------------------------------------------------------

	// Consumable
	GameplayTags.Inventory_Consumable_Potion = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Consumable.Potion"), FString("A consumable potion item"));

	// Craftable
	GameplayTags.Inventory_Craftable_Material = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Craftable.Material"), FString("A raw material used in crafting"));

	// Quest
	GameplayTags.Inventory_Quest_Item = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Quest.Item"), FString("A quest-related item"));

	// Misc
	GameplayTags.Inventory_Misc_Junk = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Misc.Junk"), FString("A miscellaneous or low-value item"));

	// -------------------------------------------------------------------------
	// Inventory — Fragments
	// -------------------------------------------------------------------------

	GameplayTags.Fragment_Grid = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.Grid"), FString("Identifies the grid fragment: defines tile footprint on the inventory grid"));

	GameplayTags.Fragment_Icon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.Icon"), FString("Identifies the image fragment: stores item icon texture and display dimensions"));

	GameplayTags.Fragment_Stackable = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.Stackable"), FString("Identifies the stackable fragment: defines max stack size and current stack count"));

	GameplayTags.Fragment_Consumable = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.Consumable"), FString("Identifies the consumable fragment: defines consume behaviour per item type"));

	GameplayTags.Fragment_ItemName = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.ItemName"), FString("Identifies the text fragment: stores item name or other display text"));

	GameplayTags.Fragment_PrimaryStat = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.PrimaryStat"), FString("Identifies a labeled numeric stat fragment (e.g. damage, armour)"));

	GameplayTags.Fragment_ItemType = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.ItemType"), FString("Identifies a text leaf displaying the item's type/category"));

	GameplayTags.Fragment_FlavorText = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.FlavorText"), FString("Identifies a text leaf displaying flavour/lore text"));

	GameplayTags.Fragment_SellValue = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.SellValue"), FString("Identifies a labeled-value leaf displaying the item's sell value"));

	GameplayTags.Fragment_RequiredLevel = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.RequiredLevel"), FString("Identifies a labeled-value leaf displaying the item's required level"));

	GameplayTags.Fragment_Mesh = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.Mesh"), FString("Identifies FFP_MeshFragment: mesh replacement/attachment operations applied on equip"));

	GameplayTags.Fragment_Rarity = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.Rarity"), FString("Identifies FFP_RarityFragment: item rarity and drop-weight table"));
	GameplayTags.Fragment_ItemLevel = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.ItemLevel"), FString("Identifies FFP_ItemLevelFragment: item level set from the dropping enemy"));
	GameplayTags.Fragment_AttributeRequirements = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.AttributeRequirements"), FString("Identifies FFP_AttributeRequirementFragment: primary stat minimums to equip"));
	GameplayTags.Fragment_Affixes = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.Affixes"), FString("Identifies FFP_AffixFragment: all rolled affixes for an equippable item"));
	GameplayTags.Fragment_Implicits = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.Implicits"), FString("Identifies FFP_ImplicitFragment: fixed implicit modifiers present on all rarities"));
	GameplayTags.Fragment_Skills = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.Skills"), FString("Identifies FFP_SkillFragment: skills rolled from a pool and granted to the player on equip"));

	GameplayTags.Fragment_GripStance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Fragment.GripStance"), FString("Identifies FFP_GripStanceFragment: applies a weapon grip style to the player on equip"));

	// -------------------------------------------------------------------------
	// Equipment Slot Types  (parent tags — items use child tags under these)
	// -------------------------------------------------------------------------
	GameplayTags.Inventory_Equippable_BodyArmour = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Equippable.BodyArmour"), FString("Body armour equipment slot"));
	GameplayTags.Inventory_Equippable_Headwear = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Equippable.Headwear"), FString("Headwear equipment slot"));
	GameplayTags.Inventory_Equippable_Gloves = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Equippable.Gloves"), FString("Gloves equipment slot"));
	GameplayTags.Inventory_Equippable_Boots = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Equippable.Boots"), FString("Boots equipment slot"));
	GameplayTags.Inventory_Equippable_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Equippable.Weapon"), FString("Weapon equipment slot"));
	GameplayTags.Inventory_Equippable_Injector = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Equippable.Injector"), FString("Injector equipment slot (any of the 3 injector slots)"));
	GameplayTags.Inventory_Equippable_Belt = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Equippable.Belt"), FString("Belt equipment slot"));
	GameplayTags.Inventory_Equippable_Ring = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Equippable.Ring"), FString("Ring equipment slot (any of the 2 ring slots)"));
	GameplayTags.Inventory_Equippable_Amulet = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Equippable.Amulet"), FString("Amulet equipment slot"));

	// -------------------------------------------------------------------------
	// Item Types
	// -------------------------------------------------------------------------
	GameplayTags.Inventory_Equippable_Headwear_CombatHelm = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Equippable.Headwear.CombatHelm"), FString("Combat Helm headwear item"));
	GameplayTags.Inventory_Equippable_Gloves_CombatGloves = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Equippable.Gloves.CombatGloves"), FString("Combat Gloves item"));
	GameplayTags.Inventory_Equippable_BodyArmour_CombatArmour = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Equippable.BodyArmour.CombatArmour"), FString("Combat Armour body armour item"));
	GameplayTags.Inventory_Equippable_BodyArmour_DusterCoat = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Equippable.BodyArmour.DusterCoat"), FString("Duster Coat body armour item"));
	GameplayTags.Inventory_Equippable_Boots_CombatBoots = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Equippable.Boots.CombatBoots"), FString("Combat Boots item"));

	// -------------------------------------------------------------------------
	// Weapons
	// -------------------------------------------------------------------------
	GameplayTags.Inventory_Equippable_Weapon_ShockRail = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Equippable.Weapon.ShockRail"), FString("ShockRail weapon item"));

	GameplayTags.Inventory_Equippable_Weapon_ImpactorMace = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Inventory.Equippable.Weapon.ImpactorMace"), FString("ImpactorMace weapon item"));
	
	
	// -------------------------------------------------------------------------
	//Locations
	GameplayTags.Location_Startup = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Location.Startup"), FString("Startup / new character level"));
	GameplayTags.Location_The_Aurora = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Location.The_Aurora"), FString("The Aurora"));
	GameplayTags.Location_Colony = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Location.Colony"), FString("Colony"));
	GameplayTags.Location_Lunar_Base = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Location.Lunar_Base"), FString("Lunar Base"));
	GameplayTags.Location_Void_Domain = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Location.Void_Domain"), FString("Void Domain"));
	GameplayTags.Location_Derelict_Station = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Location.Derelict_Station"), FString("Derelict Station"));
	GameplayTags.Location_Crystalline_Depths = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Location.Crystalline_Depths"), FString("Crystalline Depths"));
	GameplayTags.Location_Frozen_Orbit = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Location.Frozen_Orbit"), FString("Frozen Orbit"));
	GameplayTags.Location_Ash_Fields = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Location.Ash_Fields"), FString("Ash Fields"));
	GameplayTags.Location_Titans_Hollow = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Location.Titans_Hollow"), FString("Titan's Hollow"));
	GameplayTags.Location_Shattered_Reach = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Location.Shattered_Reach"), FString("Shattered Reach"));
	GameplayTags.Location_SafeZone = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Location.SafeZone"), FString("Safe Zone — the returning hub between endless-simulation runs"));
	GameplayTags.Location_EndlessSimulation = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Location.EndlessSimulation"), FString("Endless Simulation — the reused combat zone, entered at varying depth"));


	//PassiveTree
	GameplayTags.PassiveTree_Might = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Might"),
		FString("Might passive skill tree")
		);
	GameplayTags.PassiveTree_Resonance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Resonance"),
		FString("Resonance passive skill tree")
		);
	GameplayTags.PassiveTree_Agility = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Agility"),
		FString("Agility passive skill tree")
		);
	GameplayTags.PassiveTree_Fortitude = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Fortitude"),
		FString("Fortitude passive skill tree")
		);
	GameplayTags.PassiveTree_IncreasedArmour_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedArmour.1"),
		FString("8% Increased Armour")
		);
	GameplayTags.PassiveTree_IncreasedArmour_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedArmour.2"),
		FString("8% Increased Armour")
		);
	GameplayTags.PassiveTree_IncreasedArmour_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedArmour.3"),
		FString("8% Increased Armour")
		);
	GameplayTags.PassiveTree_IncreasedArmour_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedArmour.4"),
		FString("8% Increased Armour")
		);
	GameplayTags.PassiveTree_IncreasedMelee_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedMelee.1"),
		FString("5% Increased Melee Damage")
		);
	GameplayTags.PassiveTree_IncreasedMelee_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedMelee.2"),
		FString("5% Increased Melee Damage")
		);
	GameplayTags.PassiveTree_IncreasedMelee_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedMelee.3"),
		FString("5% Increased Melee Damage")
		);
	GameplayTags.PassiveTree_IncreasedMelee_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedMelee.4"),
		FString("5% Increased Melee Damage")
		);
	GameplayTags.PassiveTree_IncreasedMelee_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedMelee.5"),
		FString("5% Increased Melee Damage")
		);
	GameplayTags.PassiveTree_IncreasedMelee_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedMelee.6"),
		FString("5% Increased Melee Damage")
		);
	GameplayTags.PassiveTree_BruisedConcussedThreshold_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.BruisedConcussedThreshold.1"),
		FString("+5 to Bruised Threshold\n+5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_BruisedConcussedThreshold_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.BruisedConcussedThreshold.2"),
		FString("+5 to Bruised Threshold\n+5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_BruisedConcussedThreshold_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.BruisedConcussedThreshold.3"),
		FString("+5 to Bruised Threshold\n+5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_BruisedConcussedThreshold_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.BruisedConcussedThreshold.4"),
		FString("+5 to Bruised Threshold\n+5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_BruisedConcussedThreshold_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.BruisedConcussedThreshold.5"),
		FString("+5 to Bruised Threshold\n+5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_BruisedConcussedThreshold_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.BruisedConcussedThreshold.6"),
		FString("+5 to Bruised Threshold\n+5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_Might_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Might.3"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Might_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Might.1"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Might_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Might.2"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Might_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Might.4"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Might_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Might.5"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Might_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Might.6"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_DeferredDamage_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.DeferredDamage.1"),
		FString("2% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_DeferredDamage_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.DeferredDamage.2"),
		FString("2% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_DeferredDamage_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.DeferredDamage.3"),
		FString("2% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_DeferredDamage_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.DeferredDamage.4"),
		FString("2% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_DeferredDamage_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.DeferredDamage.5"),
		FString("2% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_DeferredDamage_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.DeferredDamage.6"),
		FString("2% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_IncreasedMelee_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedMelee.7"),
		FString("5% Increased Melee Damage")
		);
	GameplayTags.PassiveTree_RelentlessStrength_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.RelentlessStrength.1"),
		FString("10% Increased Melee Damage\n+2 Might")
		);
	GameplayTags.PassiveTree_IncreasedMelee_8 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedMelee.8"),
		FString("5% Increased Melee Damage")
		);
	GameplayTags.PassiveTree_IncreasedMelee_9 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedMelee.9"),
		FString("5% Increased Melee Damage")
		);
	GameplayTags.PassiveTree_Outlasting_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Outlasting.1"),
		FString("+5 to All Debuff Thresholds\n +1 Second to Deferred Damage Over Time")
		);
	GameplayTags.PassiveTree_BruisedConcussedThreshold_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.BruisedConcussedThreshold.7"),
		FString("+5 to Bruised Threshold\n+5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_BruisedConcussedThreshold_8 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.BruisedConcussedThreshold.8"),
		FString("+5 to Bruised Threshold\n+5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_Might_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Might.7"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Might_8 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Might.8"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_DeferredDamage_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.DeferredDamage.7"),
		FString("2% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_DeferredDamage_8 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.DeferredDamage.8"),
		FString("2% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_IncreasedMelee_10 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedMelee.10"),
		FString("5% Increased Melee Damage")
		);
	GameplayTags.PassiveTree_IncreasedMelee_11 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedMelee.11"),
		FString("5% Increased Melee Damage")
		);
	GameplayTags.PassiveTree_IncreasedMelee_12 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedMelee.12"),
		FString("5% Increased Melee Damage")
		);
	GameplayTags.PassiveTree_IncreasedMelee_13 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedMelee.13"),
		FString("5% Increased Melee Damage")
		);
	GameplayTags.PassiveTree_BruisedConcussedThreshold_9 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.BruisedConcussedThreshold.9"),
		FString("+5 to Bruised Threshold\n+5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_BruisedConcussedThreshold_10 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.BruisedConcussedThreshold.10"),
		FString("+5 to Bruised Threshold\n+5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_BruisedConcussedThreshold_11 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.BruisedConcussedThreshold.11"),
		FString("+5 to Bruised Threshold\n+5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_BruisedConcussedThreshold_12 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.BruisedConcussedThreshold.12"),
		FString("+5 to Bruised Threshold\n+5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_BruisedConcussedThreshold_13 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.BruisedConcussedThreshold.13"),
		FString("+5 to Bruised Threshold\n+5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_Might_9 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Might.9"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Might_10 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Might.10"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Might_11 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Might.11"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Might_12 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Might.12"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Might_13 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Might.13"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_DeferredDamage_9 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.DeferredDamage.9"),
		FString("2% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_DeferredDamage_10 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.DeferredDamage.10"),
		FString("2% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_DeferredDamage_11 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.DeferredDamage.11"),
		FString("2% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_DeferredDamage_12 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.DeferredDamage.12"),
		FString("2% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_DeferredDamage_13 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.DeferredDamage.13"),
		FString("2% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_IncreasedHitPoints_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedHitPoints.1"),
		FString("4% Increased Hit Points")
		);
	GameplayTags.PassiveTree_Fortitude_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Fortitude.3"),
		FString("+1 Fortitude")
		);
	GameplayTags.PassiveTree_Fortitude_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Fortitude.1"),
		FString("+1 Fortitude")
		);
	GameplayTags.PassiveTree_Fortitude_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Fortitude.2"),
		FString("+1 Fortitude")
		);
	GameplayTags.PassiveTree_Fortitude_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Fortitude.4"),
		FString("+1 Fortitude")
		);
	GameplayTags.PassiveTree_Fortitude_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Fortitude.5"),
		FString("+1 Fortitude")
		);
	GameplayTags.PassiveTree_Fortitude_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Fortitude.6"),
		FString("+1 Fortitude")
		);
	GameplayTags.PassiveTree_AllThreshold_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.AllThreshold.2"),
		FString("+2 to All Debuff Thresholds")
		);
	GameplayTags.PassiveTree_Bulwark_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Bulwark.1"),
		FString("8% Increased Hit Points, 12% Increased Armour")
		);
	GameplayTags.PassiveTree_NeverFall_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.NeverFall.1"),
		FString("+2 to Fortitude, +4 to All Debuff Thresholds")
		);
	GameplayTags.PassiveTree_Fortitude_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Fortitude.7"),
		FString("+1 Fortitude")
		);
	GameplayTags.PassiveTree_Fortitude_8 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Fortitude.8"),
		FString("+1 Fortitude")
		);
	GameplayTags.PassiveTree_Fortitude_9 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Fortitude.9"),
		FString("+1 Fortitude")
		);
	GameplayTags.PassiveTree_Fortitude_10 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Fortitude.10"),
		FString("+1 Fortitude")
		);
	GameplayTags.PassiveTree_Fortitude_11 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Fortitude.11"),
		FString("+1 Fortitude")
		);
	GameplayTags.PassiveTree_Fortitude_12 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Fortitude.12"),
		FString("+1 Fortitude")
		);
	GameplayTags.PassiveTree_Fortitude_13 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Fortitude.13"),
		FString("+1 Fortitude")
		);
	GameplayTags.PassiveTree_IncreasedHitPoints_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedHitPoints.2"),
		FString("4% Increased Hit Points")
		);
	GameplayTags.PassiveTree_IncreasedHitPoints_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedHitPoints.3"),
		FString("4% Increased Hit Points")
		);
	GameplayTags.PassiveTree_IncreasedHitPoints_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedHitPoints.4"),
		FString("4% Increased Hit Points")
		);
	GameplayTags.PassiveTree_IncreasedHitPoints_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedHitPoints.5"),
		FString("4% Increased Hit Points")
		);
	GameplayTags.PassiveTree_IncreasedHitPoints_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedHitPoints.6"),
		FString("4% Increased Hit Points")
		);
	GameplayTags.PassiveTree_IncreasedArmour_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedArmour.5"),
		FString("+5 to Bruised Threshold, +5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_IncreasedArmour_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedArmour.6"),
		FString("+5 to Bruised Threshold, +5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_AllThreshold_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.AllThreshold.1"),
		FString("+2 to All Debuff Thresholds")
		);
	GameplayTags.PassiveTree_AllThreshold_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.AllThreshold.3"),
		FString("+2 to All Debuff Thresholds")
		);
	GameplayTags.PassiveTree_AllThreshold_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.AllThreshold.4"),
		FString("+2 to All Debuff Thresholds")
		);
	GameplayTags.PassiveTree_AllThreshold_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.AllThreshold.5"),
		FString("+2 to All Debuff Thresholds")
		);
	GameplayTags.PassiveTree_AllThreshold_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.AllThreshold.6"),
		FString("+2 to All Debuff Thresholds")
		);
	GameplayTags.PassiveTree_IncreasedHitPoints_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedHitPoints.7"),
		FString("4% Increased Hit Points")
		);
	GameplayTags.PassiveTree_IncreasedHitPoints_8 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedHitPoints.8"),
		FString("4% Increased Hit Points")
		);
	GameplayTags.PassiveTree_IncreasedHitPoints_9 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedHitPoints.9"),
		FString("4% Increased Hit Points")
		);
	GameplayTags.PassiveTree_IncreasedArmour_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedArmour.7"),
		FString("+5 to Bruised Threshold, +5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_IncreasedArmour_8 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedArmour.8"),
		FString("+5 to Bruised Threshold, +5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_AllThreshold_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.AllThreshold.7"),
		FString("+2 to All Debuff Thresholds")
		);
	GameplayTags.PassiveTree_AllThreshold_8 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.AllThreshold.8"),
		FString("+2 to All Debuff Thresholds")
		);
	GameplayTags.PassiveTree_IncreasedHitPoints_10 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedHitPoints.10"),
		FString("4% Increased Hit Points")
		);
	GameplayTags.PassiveTree_IncreasedHitPoints_11 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedHitPoints.11"),
		FString("4% Increased Hit Points")
		);
	GameplayTags.PassiveTree_IncreasedHitPoints_12 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedHitPoints.12"),
		FString("4% Increased Hit Points")
		);
	GameplayTags.PassiveTree_IncreasedHitPoints_13 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedHitPoints.13"),
		FString("4% Increased Hit Points")
		);
	GameplayTags.PassiveTree_IncreasedArmour_9 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedArmour.9"),
		FString("+5 to Bruised Threshold, +5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_IncreasedArmour_10 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedArmour.10"),
		FString("+5 to Bruised Threshold, +5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_IncreasedArmour_11 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedArmour.11"),
		FString("+5 to Bruised Threshold, +5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_IncreasedArmour_12 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedArmour.12"),
		FString("+5 to Bruised Threshold, +5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_IncreasedArmour_13 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.IncreasedArmour.13"),
		FString("+5 to Bruised Threshold, +5 to Concussed Threshold")
		);
	GameplayTags.PassiveTree_AllThreshold_9 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.AllThreshold.9"),
		FString("+2 to All Debuff Thresholds")
		);
	GameplayTags.PassiveTree_AllThreshold_10 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.AllThreshold.10"),
		FString("+2 to All Debuff Thresholds")
		);
	GameplayTags.PassiveTree_AllThreshold_11 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.AllThreshold.11"),
		FString("+2 to All Debuff Thresholds")
		);
	GameplayTags.PassiveTree_AllThreshold_12 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.AllThreshold.12"),
		FString("+2 to All Debuff Thresholds")
		);
	GameplayTags.PassiveTree_AllThreshold_13 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.AllThreshold.13"),
		FString("+2 to All Debuff Thresholds")
		);
	GameplayTags.PassiveTree_Accelerator_AcceleratorDamage_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.AcceleratorDamage.1"),
		FString("8% Increased Accelerator Damage")
		);
	GameplayTags.PassiveTree_Accelerator_AcceleratorDamage_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.AcceleratorDamage.2"),
		FString("8% Increased Accelerator Damage")
		);
	GameplayTags.PassiveTree_Accelerator_AcceleratorDamage_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.AcceleratorDamage.3"),
		FString("8% Increased Accelerator Damage")
		);
	GameplayTags.PassiveTree_Accelerator_AcceleratorDamage_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.AcceleratorDamage.4"),
		FString("8% Increased Accelerator Damage")
		);
	GameplayTags.PassiveTree_Accelerator_AcceleratorDamage_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.AcceleratorDamage.5"),
		FString("8% Increased Accelerator Damage")
		);
	GameplayTags.PassiveTree_Accelerator_AcceleratorDamage_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.AcceleratorDamage.6"),
		FString("8% Increased Accelerator Damage")
		);
	GameplayTags.PassiveTree_Accelerator_AcceleratorDamage_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.AcceleratorDamage.7"),
		FString("12% Increased Accelerator Damage, 5% More Accelerator Damage")
		);
	GameplayTags.PassiveTree_Accelerator_ParticleCharge_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.ParticleCharge.1"),
		FString("8% Increased Energy Damage with Accelerator Weapons")
		);
	GameplayTags.PassiveTree_Accelerator_ParticleCharge_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.ParticleCharge.2"),
		FString("8% Increased Energy Damage with Accelerator Weapons")
		);
	GameplayTags.PassiveTree_Accelerator_ParticleCharge_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.ParticleCharge.3"),
		FString("8% Increased Energy Damage with Accelerator Weapons")
		);
	GameplayTags.PassiveTree_Accelerator_ParticleCharge_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.ParticleCharge.4"),
		FString("8% Increased Energy Damage with Accelerator Weapons")
		);
	GameplayTags.PassiveTree_Accelerator_ParticleCharge_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.ParticleCharge.5"),
		FString("15% Increased Energy Damage with Accelerator Weapons")
		);
	GameplayTags.PassiveTree_Accelerator_PenetratorWheel_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.PenetratorWheel.1"),
		FString("Penetrate 2% of Physical Resistance with Accelerator Weapons")
		);
	GameplayTags.PassiveTree_Accelerator_PenetratorWheel_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.PenetratorWheel.2"),
		FString("15% Critical Strike Multiplier with Accelerator Weapons")
		);
	GameplayTags.PassiveTree_Accelerator_PenetratorWheel_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.PenetratorWheel.3"),
		FString("Penetrate 2% of Physical Resistance with Accelerator Weapons")
		);
	GameplayTags.PassiveTree_Accelerator_PenetratorWheel_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.PenetratorWheel.4"),
		FString("15% Critical Strike Multiplier with Accelerator Weapons")
		);
	GameplayTags.PassiveTree_Accelerator_PenetratorWheel_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.PenetratorWheel.5"),
		FString("Penetrate 5% of Physical Resistance with Accelerator Weapons, 25% Critical Strike Multiplier with Accelerator Weapons")
		);
	GameplayTags.PassiveTree_Accelerator_RailgunMastery_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.RailgunMastery.1"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Accelerator_RailgunMastery_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.RailgunMastery.2"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Accelerator_RailgunMastery_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.RailgunMastery.3"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Accelerator_RailgunMastery_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.RailgunMastery.4"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Accelerator_RailgunMastery_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.RailgunMastery.5"),
		FString("25% Critical Strike Multiplier with Accelerator Weapons, 6% More Accelerator Damage")
		);
	GameplayTags.PassiveTree_Accelerator_VelocityRounds_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.VelocityRounds.1"),
		FString("8% Increased Projectile Speed with Accelerator Weapons")
		);
	GameplayTags.PassiveTree_Accelerator_VelocityRounds_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.VelocityRounds.2"),
		FString("8% Increased Projectile Speed with Accelerator Weapons")
		);
	GameplayTags.PassiveTree_Accelerator_VelocityRounds_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.VelocityRounds.3"),
		FString("8% Increased Projectile Speed with Accelerator Weapons")
		);
	GameplayTags.PassiveTree_Accelerator_VelocityRounds_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.VelocityRounds.4"),
		FString("15% Increased Projectile Speed with Accelerator Weapons")
		);
	GameplayTags.PassiveTree_Accelerator_VelocityRounds_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Accelerator.VelocityRounds.5"),
		FString("Increases and Reductions to Projectile Speed also Increase Projectile Damage from Accelerator Weapons")
		);
	GameplayTags.PassiveTree_Axe_AxeDamage_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.AxeDamage.1"),
		FString("8% Increased Axe Damage")
		);
	GameplayTags.PassiveTree_Axe_AxeDamage_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.AxeDamage.2"),
		FString("8% Increased Axe Damage")
		);
	GameplayTags.PassiveTree_Axe_AxeDamage_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.AxeDamage.3"),
		FString("8% Increased Axe Damage")
		);
	GameplayTags.PassiveTree_Axe_AxeDamage_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.AxeDamage.4"),
		FString("8% Increased Axe Damage")
		);
	GameplayTags.PassiveTree_Axe_AxeDamage_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.AxeDamage.5"),
		FString("8% Increased Axe Damage")
		);
	GameplayTags.PassiveTree_Axe_AxeDamage_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.AxeDamage.6"),
		FString("8% Increased Axe Damage")
		);
	GameplayTags.PassiveTree_Axe_AxeDamage_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.AxeDamage.7"),
		FString("12% Increased Axe Damage, 5% More Axe Damage")
		);
	GameplayTags.PassiveTree_Axe_BerserkersAxework_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.BerserkersAxework.1"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Axe_BerserkersAxework_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.BerserkersAxework.2"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Axe_BerserkersAxework_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.BerserkersAxework.3"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Axe_BerserkersAxework_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.BerserkersAxework.4"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Axe_BerserkersAxework_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.BerserkersAxework.5"),
		FString("25% Critical Strike Multiplier with Axes, 6% More Axe Damage")
		);
	GameplayTags.PassiveTree_Axe_BleedWheel_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.BleedWheel.1"),
		FString("8% Increased Damage Over Time with Axes")
		);
	GameplayTags.PassiveTree_Axe_BleedWheel_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.BleedWheel.2"),
		FString("10% Increased Bruised Debuff Effectiveness with Axes")
		);
	GameplayTags.PassiveTree_Axe_BleedWheel_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.BleedWheel.3"),
		FString("8% Increased Damage Over Time with Axes")
		);
	GameplayTags.PassiveTree_Axe_BleedWheel_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.BleedWheel.4"),
		FString("10% Increased Bruised Debuff Effectiveness with Axes")
		);
	GameplayTags.PassiveTree_Axe_BleedWheel_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.BleedWheel.5"),
		FString("15% Increased Damage Over Time with Axes, 20% Increased Bruised Debuff Effectiveness with Axes")
		);
	GameplayTags.PassiveTree_Axe_CleaveWheel_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.CleaveWheel.1"),
		FString("8% Increased Area of Effect with Axes")
		);
	GameplayTags.PassiveTree_Axe_CleaveWheel_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.CleaveWheel.2"),
		FString("15% Critical Strike Multiplier with Axes")
		);
	GameplayTags.PassiveTree_Axe_CleaveWheel_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.CleaveWheel.3"),
		FString("8% Increased Area of Effect with Axes")
		);
	GameplayTags.PassiveTree_Axe_CleaveWheel_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.CleaveWheel.4"),
		FString("15% Critical Strike Multiplier with Axes")
		);
	GameplayTags.PassiveTree_Axe_CleaveWheel_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.CleaveWheel.5"),
		FString("15% Increased Area of Effect with Axes, 25% Critical Strike Multiplier with Axes")
		);
	GameplayTags.PassiveTree_Axe_Execute_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.Execute.1"),
		FString("Kill Enemies with less than 5% of Life Remaining with Axes")
		);
	GameplayTags.PassiveTree_Axe_ExecutionersEdge_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.ExecutionersEdge.1"),
		FString("15% Critical Strike Multiplier with Axes")
		);
	GameplayTags.PassiveTree_Axe_ExecutionersEdge_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.ExecutionersEdge.2"),
		FString("15% Critical Strike Multiplier with Axes")
		);
	GameplayTags.PassiveTree_Axe_ExecutionersEdge_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.ExecutionersEdge.3"),
		FString("15% Critical Strike Multiplier with Axes")
		);
	GameplayTags.PassiveTree_Axe_ExecutionersEdge_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Axe.ExecutionersEdge.4"),
		FString("30% Critical Strike Multiplier with Axes")
		);
	GameplayTags.PassiveTree_Bide_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Bide.1"),
		FString("3% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_Bide_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Bide.2"),
		FString("3% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_Bide_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Bide.3"),
		FString("3% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_Bide_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Bide.4"),
		FString("Damage Deffered Over 2 Additional Seconds, 25% Increased Damage while Taking Deffered Damage")
		);
	GameplayTags.PassiveTree_Endure_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Endure.1"),
		FString("5% Increased Maximum Hit Points")
		);
	GameplayTags.PassiveTree_Endure_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Endure.2"),
		FString("5% Increased Maximum Hit Points")
		);
	GameplayTags.PassiveTree_Endure_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Endure.3"),
		FString("10% Increased Maximum Hit Points")
		);
	GameplayTags.PassiveTree_Mace_ArmorBreak_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.ArmorBreak.1"),
		FString("Penetrate 2% of Armour with Maces")
		);
	GameplayTags.PassiveTree_Mace_ArmorBreak_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.ArmorBreak.2"),
		FString("Penetrate 2% of Armour with Maces")
		);
	GameplayTags.PassiveTree_Mace_ArmorBreak_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.ArmorBreak.3"),
		FString("Penetrate 2% of Armour with Maces")
		);
	GameplayTags.PassiveTree_Mace_ArmorBreak_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.ArmorBreak.4"),
		FString("Penetrate 2% of Armour with Maces")
		);
	GameplayTags.PassiveTree_Mace_ArmorBreak_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.ArmorBreak.5"),
		FString("Penetrate 5% of Armour with Maces, Penetrate 5% of All Resistances with Maces")
		);
	GameplayTags.PassiveTree_Mace_GroundSlam_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.GroundSlam.1"),
		FString("10% Increased Area of Effect with Maces")
		);
	GameplayTags.PassiveTree_Mace_GroundSlam_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.GroundSlam.2"),
		FString("10% Increased Area of Effect with Maces")
		);
	GameplayTags.PassiveTree_Mace_GroundSlam_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.GroundSlam.3"),
		FString("10% Area Damage with Maces")
		);
	GameplayTags.PassiveTree_Mace_GroundSlam_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.GroundSlam.4"),
		FString("10% Area Damage with Maces")
		);
	GameplayTags.PassiveTree_Mace_GroundSlam_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.GroundSlam.5"),
		FString("+3 Additional Area Radius with Maces")
		);
	GameplayTags.PassiveTree_Mace_HeavySwing_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.HeavySwing.1"),
		FString("15% Critical Strike Multiplier with Maces")
		);
	GameplayTags.PassiveTree_Mace_HeavySwing_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.HeavySwing.2"),
		FString("15% Critical Strike Multiplier with Maces")
		);
	GameplayTags.PassiveTree_Mace_HeavySwing_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.HeavySwing.3"),
		FString("25% Increased Critical Strike Chance with Maces")
		);
	GameplayTags.PassiveTree_Mace_HeavySwing_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.HeavySwing.4"),
		FString("25% Increased Critical Strike Chance with Maces")
		);
	GameplayTags.PassiveTree_Mace_HeavySwing_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.HeavySwing.5"),
		FString("+3% Base Critical Strike Chance with Maces, 35% Critical Strike Multiplier with Maces")
		);
	GameplayTags.PassiveTree_Mace_MaceDamageA_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.MaceDamageA.1"),
		FString("9% Increased Mace Damage")
		);
	GameplayTags.PassiveTree_Mace_MaceDamageA_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.MaceDamageA.2"),
		FString("9% Increased Mace Damage")
		);
	GameplayTags.PassiveTree_Mace_MaceDamageA_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.MaceDamageA.3"),
		FString("9% Increased Mace Damage")
		);
	GameplayTags.PassiveTree_Mace_MaceDamageA_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.MaceDamageA.4"),
		FString("9% Increased Mace Damage")
		);
	GameplayTags.PassiveTree_Mace_MaceDamageA_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.MaceDamageA.5"),
		FString("9% Increased Mace Damage")
		);
	GameplayTags.PassiveTree_Mace_MaceDamageA_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.MaceDamageA.6"),
		FString("9% Increased Mace Damage")
		);
	GameplayTags.PassiveTree_Mace_MaceDamageA_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.MaceDamageA.7"),
		FString("12% Increased Mace Damage, 5% Increased Movement Speed")
		);
	GameplayTags.PassiveTree_Mace_MaceDamageB_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.MaceDamageB.1"),
		FString("5% Increased Mace Damage, 5% Increased Area Damage")
		);
	GameplayTags.PassiveTree_Mace_MaceDamageB_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.MaceDamageB.2"),
		FString("5% Increased Mace Damage, 5% Increased Area Damage")
		);
	GameplayTags.PassiveTree_Mace_MaceDamageB_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.MaceDamageB.3"),
		FString("5% Increased Mace Damage, 5% Increased Area Damage")
		);
	GameplayTags.PassiveTree_Mace_MaceDamageB_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.MaceDamageB.4"),
		FString("5% Increased Mace Damage, 5% Increased Area Damage")
		);
	GameplayTags.PassiveTree_Mace_MaceDamageB_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.MaceDamageB.5"),
		FString("5% Increased Mace Damage, 5% Increased Area Damage")
		);
	GameplayTags.PassiveTree_Mace_MaceDamageB_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.MaceDamageB.6"),
		FString("10% Increased Mace Damage, +1 Additional Area Radius")
		);
	GameplayTags.PassiveTree_Mace_SledgehammerMastery_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.SledgehammerMastery.1"),
		FString("5% Reduced Heat Cost with Maces")
		);
	GameplayTags.PassiveTree_Mace_SledgehammerMastery_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.SledgehammerMastery.2"),
		FString("5% Reduced Heat Cost with Maces")
		);
	GameplayTags.PassiveTree_Mace_SledgehammerMastery_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.SledgehammerMastery.3"),
		FString("5% Increased Heat Cost with Maces")
		);
	GameplayTags.PassiveTree_Mace_SledgehammerMastery_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.SledgehammerMastery.4"),
		FString("5% Increased Heat Cost with Maces")
		);
	GameplayTags.PassiveTree_Mace_SledgehammerMastery_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.SledgehammerMastery.5"),
		FString("+0.15 Thermal Mass")
		);
	GameplayTags.PassiveTree_Mace_StaggerWheel_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.StaggerWheel.1"),
		FString("6% Increased Concussed Debuff Effectiveness")
		);
	GameplayTags.PassiveTree_Mace_StaggerWheel_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.StaggerWheel.2"),
		FString("15% Increased Damage Against with Maces Concussed Enemies")
		);
	GameplayTags.PassiveTree_Mace_StaggerWheel_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.StaggerWheel.3"),
		FString("6% Increased Concussed Debuff Effectiveness")
		);
	GameplayTags.PassiveTree_Mace_StaggerWheel_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.StaggerWheel.4"),
		FString("15% Increased Damage with Maces Against Concussed Enemies")
		);
	GameplayTags.PassiveTree_Mace_StaggerWheel_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Mace.StaggerWheel.5"),
		FString("15% Increased Concussed Debuff Effectiveness, 24% Increased Damage with Maces Against Concussed Enemies")
		);
	GameplayTags.PassiveTree_Ordnance_BlastWheel_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.BlastWheel.1"),
		FString("8% Increased Area Damage with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_BlastWheel_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.BlastWheel.2"),
		FString("10% Increased Area of Effect with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_BlastWheel_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.BlastWheel.3"),
		FString("8% Increased Area Damage with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_BlastWheel_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.BlastWheel.4"),
		FString("+2 Additional Area Radius with Ordnance , 20% Increased Area Damage with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_DevastatingImpact_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.DevastatingImpact.1"),
		FString("15% Critical Strike Multiplier with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_DevastatingImpact_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.DevastatingImpact.2"),
		FString("15% Critical Strike Multiplier with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_DevastatingImpact_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.DevastatingImpact.3"),
		FString("20% Increased Critical Strike Chance with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_DevastatingImpact_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.DevastatingImpact.4"),
		FString("20% Increased Critical Strike Chance with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_DevastatingImpact_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.DevastatingImpact.5"),
		FString("2% Base Critical Strike Chance with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_ExplosivePenetration_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.ExplosivePenetration.1"),
		FString("2% Explosive Resistance Penetration with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_ExplosivePenetration_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.ExplosivePenetration.2"),
		FString("2% Explosive Resistance Penetration with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_ExplosivePenetration_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.ExplosivePenetration.3"),
		FString("2% Explosive Resistance Penetration with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_ExplosivePenetration_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.ExplosivePenetration.4"),
		FString("2% Explosive Resistance Penetration with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_ExplosivePenetration_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.ExplosivePenetration.5"),
		FString("7% Explosive Resistance Penetration with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_HighExplosive_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.HighExplosive.1"),
		FString("10% Increased Explosive Damage with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_HighExplosive_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.HighExplosive.2"),
		FString("10% Increased Explosive Damage with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_HighExplosive_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.HighExplosive.3"),
		FString("10% Increased Explosive Damage with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_HighExplosive_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.HighExplosive.4"),
		FString("10% Increased Explosive Damage with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_HighExplosive_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.HighExplosive.5"),
		FString("25% Increased Explosive Damage with Ordnance")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceDamageA_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceDamageA.1"),
		FString("8% Increased Ordnance Damage")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceDamageA_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceDamageA.2"),
		FString("8% Increased Ordnance Damage")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceDamageA_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceDamageA.3"),
		FString("8% Increased Ordnance Damage")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceDamageA_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceDamageA.4"),
		FString("8% Increased Ordnance Damage")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceDamageA_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceDamageA.5"),
		FString("8% Increased Ordnance Damage")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceDamageA_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceDamageA.6"),
		FString("8% Increased Ordnance Damage")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceDamageA_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceDamageA.7"),
		FString("15% Increased Ordnance Damage, 20% Explosive Damage")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceDamageB_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceDamageB.1"),
		FString("8% Increased Ordnance Damage")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceDamageB_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceDamageB.2"),
		FString("8% Increased Ordnance Damage")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceDamageB_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceDamageB.3"),
		FString("8% Increased Ordnance Damage")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceDamageB_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceDamageB.4"),
		FString("8% Increased Ordnance Damage")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceDamageB_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceDamageB.5"),
		FString("8% Increased Ordnance Damage")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceDamageB_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceDamageB.6"),
		FString("25% Increased Ordnance Damage")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceSpecialist_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceSpecialist.1"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceSpecialist_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceSpecialist.2"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceSpecialist_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceSpecialist.3"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceSpecialist_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceSpecialist.4"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceSpecialist_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceSpecialist.5"),
		FString("+1 Might")
		);
	GameplayTags.PassiveTree_Ordnance_OrdnanceSpecialist_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Ordnance.OrdnanceSpecialist.6"),
		FString("10% More Ordnance Damage")
		);
	GameplayTags.PassiveTree_Rifle_MarksmanWheel_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.MarksmanWheel.1"),
		FString("10% Increased Critical Strike with Rifles")
		);
	GameplayTags.PassiveTree_Rifle_MarksmanWheel_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.MarksmanWheel.2"),
		FString("15% Increased Critical Strike Multiplier with Rifles")
		);
	GameplayTags.PassiveTree_Rifle_MarksmanWheel_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.MarksmanWheel.3"),
		FString("8% Increased Critical Strike with Rifles")
		);
	GameplayTags.PassiveTree_Rifle_MarksmanWheel_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.MarksmanWheel.4"),
		FString("15% Increased Critical Strike Multiplier with Rifles")
		);
	GameplayTags.PassiveTree_Rifle_MarksmanWheel_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.MarksmanWheel.5"),
		FString("+3% Base Critical Strike Chance with Rifles, 30% Critical Strike Multiplier with Rifles")
		);
	GameplayTags.PassiveTree_Rifle_PenetratingRounds_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.PenetratingRounds.1"),
		FString("+1 Extra Target Pierced with Rifles")
		);
	GameplayTags.PassiveTree_Rifle_PenetratingRounds_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.PenetratingRounds.2"),
		FString("+2 Extra Targets Pierced with Rifles")
		);
	GameplayTags.PassiveTree_Rifle_RapidCycling_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RapidCycling.1"),
		FString("4% Increased Skill Speed with Rifles")
		);
	GameplayTags.PassiveTree_Rifle_RapidCycling_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RapidCycling.2"),
		FString("4% Increased Skill Speed with Rifles")
		);
	GameplayTags.PassiveTree_Rifle_RapidCycling_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RapidCycling.3"),
		FString("4% Increased Skill Speed with Rifles")
		);
	GameplayTags.PassiveTree_Rifle_RapidCycling_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RapidCycling.4"),
		FString("4% Increased Skill Speed with Rifles")
		);
	GameplayTags.PassiveTree_Rifle_RapidCycling_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RapidCycling.5"),
		FString("8% Increased Skill Speed with Rifles, Rifles Fire an Extra Projectile")
		);
	GameplayTags.PassiveTree_Rifle_RifleDamage_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RifleDamage.1"),
		FString("7% Increased Rifle Damage")
		);
	GameplayTags.PassiveTree_Rifle_RifleDamage_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RifleDamage.2"),
		FString("7% Increased Rifle Damage")
		);
	GameplayTags.PassiveTree_Rifle_RifleDamage_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RifleDamage.3"),
		FString("7% Increased Rifle Damage")
		);
	GameplayTags.PassiveTree_Rifle_RifleDamage_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RifleDamage.4"),
		FString("7% Increased Rifle Damage")
		);
	GameplayTags.PassiveTree_Rifle_RifleDamage_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RifleDamage.5"),
		FString("7% Increased Rifle Damage")
		);
	GameplayTags.PassiveTree_Rifle_RifleDamage_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RifleDamage.6"),
		FString("7% Increased Rifle Damage")
		);
	GameplayTags.PassiveTree_Rifle_RifleDamage_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RifleDamage.7"),
		FString("10% Increased Rifle Damage, 5% More Rifle Damage")
		);
	GameplayTags.PassiveTree_Rifle_RiflemansMastery_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RiflemansMastery.1"),
		FString("5% Increased Rifle Damage, +1 Might")
		);
	GameplayTags.PassiveTree_Rifle_RiflemansMastery_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RiflemansMastery.2"),
		FString("5% Increased Rifle Damage, +1 Might")
		);
	GameplayTags.PassiveTree_Rifle_RiflemansMastery_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RiflemansMastery.3"),
		FString("5% Increased Rifle Damage, +1 Might")
		);
	GameplayTags.PassiveTree_Rifle_RiflemansMastery_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RiflemansMastery.4"),
		FString("5% Increased Rifle Damage, +1 Might")
		);
	GameplayTags.PassiveTree_Rifle_RiflemansMastery_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.Rifle.RiflemansMastery.5"),
		FString("+25% Increased Area of Effect with Rifles, +4 Might")
		);
	GameplayTags.PassiveTree_RunningHot_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.RunningHot.1"),
		FString("4% Increased Maximum Hit Points, 4% Overheat Resistance")
		);
	GameplayTags.PassiveTree_RunningHot_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.RunningHot.2"),
		FString("4% Increased Maximum Hit Points, 4% Overheat Resistance")
		);
	GameplayTags.PassiveTree_RunningHot_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.RunningHot.3"),
		FString("4% Increased Maximum Hit Points, 4% Overheat Resistance")
		);
	GameplayTags.PassiveTree_RunningHot_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.RunningHot.4"),
		FString("1% of Hit Points Regenerated per Second, +5% Maximum Overheat Resistance")
		);
	GameplayTags.PassiveTree_SkillSpeed_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.SkillSpeed.1"),
		FString("3% Increased Skill Speed")
		);
	GameplayTags.PassiveTree_SkillSpeed_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.SkillSpeed.2"),
		FString("3% Increased Skill Speed")
		);
	GameplayTags.PassiveTree_SkillSpeed_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.SkillSpeed.3"),
		FString("7% Increased Skill Speed")
		);
	GameplayTags.PassiveTree_SplicedMalacostraca_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.SplicedMalacostraca.1"),
		FString("0.35% of Hit Points Regenerated per Second, 10% Increased Armour")
		);
	GameplayTags.PassiveTree_SplicedMalacostraca_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.SplicedMalacostraca.2"),
		FString("0.35% of Hit Points Regenerated per Second, 10% Increased Armour")
		);
	GameplayTags.PassiveTree_SplicedMalacostraca_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.SplicedMalacostraca.3"),
		FString("1% of Hit Points Regenerated per Second, 20% Increased Armour")
		);
	GameplayTags.PassiveTree_CarryOn_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.CarryOn.1"),
		FString("0.35% of Hit Points Regenerated per Second, 2% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_CarryOn_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.CarryOn.2"),
		FString("0.35% of Hit Points Regenerated per Second, 2% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_CarryOn_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.CarryOn.3"),
		FString("1% of Hit Points Regenerated per Second, 5% of Damage Deferred Over Time")
		);
	GameplayTags.PassiveTree_AblativeArmour_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.AblativeArmour.1"),
		FString("5% Explosive Resistance, 10% Increased Armour")
		);
	GameplayTags.PassiveTree_AblativeArmour_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.AblativeArmour.2"),
		FString("5% Explosive Resistance, 10% Increased Armour")
		);
	GameplayTags.PassiveTree_AblativeArmour_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.AblativeArmour.3"),
		FString("Double the Effect of Armour Against Explosive Damage, +5% to Maximum Explosive Damage")
		);
	GameplayTags.PassiveTree_SplicedPlanariidae_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.SplicedPlanariidae.1"),
		FString("0.5% of Hit Points Regenerated per Second")
		);
	GameplayTags.PassiveTree_SplicedPlanariidae_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.SplicedPlanariidae.2"),
		FString("0.5% of Hit Points Regenerated per Second")
		);
	GameplayTags.PassiveTree_SplicedPlanariidae_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.SplicedPlanariidae.3"),
		FString("0.5% of Hit Points Regenerated per Second")
		);
	GameplayTags.PassiveTree_SplicedPlanariidae_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.SplicedPlanariidae.4"),
		FString("2% of Hit Points Regenerated per Second")
		);
	GameplayTags.PassiveTree_LiquidCooledSystems_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("PassiveTree.LiquidCooledSystems.1"),
		FString("Skills From Weapons That Require Might Cost Hit Points Instead of Heat, 300% Increased Skill Cost For Skills From Weapons That Require Might")
		);

	// -------------------------------------------------------------------------
	// Debuff State Tags
	// -------------------------------------------------------------------------
	GameplayTags.State_Bruised = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Bruised"),
		FString("Target is currently bruised")
		);
	GameplayTags.State_Concussed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Concussed"),
		FString("Target is currently concussed")
		);
	GameplayTags.State_Irradiated = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Irradiated"),
		FString("Target is currently irradiated")
		);
	GameplayTags.State_Corroded = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("State.Corroded"),
		FString("Target is currently corroded")
		);

	// -------------------------------------------------------------------------
	// Debuff Buildup Gameplay Events
	// -------------------------------------------------------------------------
	GameplayTags.GameplayEvent_Debuff_Buildup_Bruised = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayEvent.Debuff.Buildup.Bruised"),
		FString("Sent to a target's ASC to apply bruise buildup — EventMagnitude = raw buildup amount")
		);
	GameplayTags.GameplayEvent_Debuff_Buildup_Concussed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayEvent.Debuff.Buildup.Concussed"),
		FString("Sent to a target's ASC to apply concuss buildup — EventMagnitude = raw buildup amount")
		);
	GameplayTags.GameplayEvent_Debuff_Buildup_Irradiated = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayEvent.Debuff.Buildup.Irradiated"),
		FString("Sent to a target's ASC to apply irradiate buildup — EventMagnitude = raw buildup amount")
		);
	GameplayTags.GameplayEvent_Debuff_Buildup_Corroded = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayEvent.Debuff.Buildup.Corroded"),
		FString("Sent to a target's ASC to apply corrode buildup — EventMagnitude = raw buildup amount")
		);

	//SkillTree
	GameplayTags.SkillTree_LightningCoil_Damage_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.Damage.1"),
		FString("8% Increased Damage")
		);
	GameplayTags.SkillTree_LightningCoil_Damage_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.Damage.2"),
		FString("8% Increased Damage")
		);
	GameplayTags.SkillTree_LightningCoil_Damage_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.Damage.3"),
		FString("8% Increased Damage")
		);
	GameplayTags.SkillTree_LightningCoil_Damage_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.Damage.4"),
		FString("8% Increased Damage")
		);
	GameplayTags.SkillTree_LightningCoil_Damage_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.Damage.5"),
		FString("8% Increased Damage")
		);
	GameplayTags.SkillTree_LightningCoil_Damage_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.Damage.6"),
		FString("8% Increased Damage")
		);
	GameplayTags.SkillTree_LightningCoil_Damage_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.Damage.7"),
		FString("8% Increased Damage")
		);
	GameplayTags.SkillTree_LightningCoil_IncreasedCritChance_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.IncreasedCritChance.1"),
		FString("10% Increased Critical Chance")
		);
	GameplayTags.SkillTree_LightningCoil_IncreasedCritChance_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.IncreasedCritChance.2"),
		FString("10% Increased Critical Chance")
		);
	GameplayTags.SkillTree_LightningCoil_IncreasedCritChance_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.IncreasedCritChance.3"),
		FString("10% Increased Critical Chance")
		);
	GameplayTags.SkillTree_LightningCoil_IncreasedCritChance_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.IncreasedCritChance.4"),
		FString("10% Increased Critical Chance")
		);
	GameplayTags.SkillTree_LightningCoil_IncreasedCritChance_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.IncreasedCritChance.5"),
		FString("10% Increased Critical Chance")
		);
	GameplayTags.SkillTree_LightningCoil_IncreasedCritChance_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.IncreasedCritChance.6"),
		FString("10% Increased Critical Chance")
		);
	GameplayTags.SkillTree_LightningCoil_IncreasedCritChance_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.IncreasedCritChance.7"),
		FString("10% Increased Critical Chance")
		);
	GameplayTags.SkillTree_LightningCoil_IncreasedCritChance_8 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.IncreasedCritChance.8"),
		FString("10% Increased Critical Chance")
		);
	GameplayTags.SkillTree_LightningCoil_BoltAoE_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.BoltAoE.1"),
		FString("+10cm Bolt Area Of Effect Radius")
		);
	GameplayTags.SkillTree_LightningCoil_BoltAoE_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.BoltAoE.2"),
		FString("+10cm Bolt Area Of Effect Radius")
		);
	GameplayTags.SkillTree_LightningCoil_BoltAoE_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.BoltAoE.3"),
		FString("+10cm Bolt Area Of Effect Radius")
		);
	GameplayTags.SkillTree_LightningCoil_BoltAoE_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.BoltAoE.4"),
		FString("+10cm Bolt Area Of Effect Radius")
		);
	GameplayTags.SkillTree_LightningCoil_BoltAoE_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.BoltAoE.5"),
		FString("+10cm Bolt Area Of Effect Radius")
		);
	GameplayTags.SkillTree_LightningCoil_BoltAoE_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.BoltAoE.6"),
		FString("+10cm Bolt Area Of Effect Radius")
		);
	GameplayTags.SkillTree_LightningCoil_CorrodedBuildup_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.CorrodedBuildup.1"),
		FString("+2 to Corroded Buildup per bolt")
		);
	GameplayTags.SkillTree_LightningCoil_CorrodedBuildup_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.CorrodedBuildup.2"),
		FString("+2 to Corroded Buildup per bolt")
		);
	GameplayTags.SkillTree_LightningCoil_CorrodedBuildup_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.CorrodedBuildup.3"),
		FString("+2 to Corroded Buildup per bolt")
		);
	GameplayTags.SkillTree_LightningCoil_CorrodedBuildup_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.CorrodedBuildup.4"),
		FString("+2 to Corroded Buildup per bolt")
		);
	GameplayTags.SkillTree_LightningCoil_CorrodedBuildup_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.CorrodedBuildup.5"),
		FString("+2 to Corroded Buildup per bolt")
		);
	GameplayTags.SkillTree_LightningCoil_CorrodedBuildup_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.CorrodedBuildup.6"),
		FString("+2 to Corroded Buildup per bolt")
		);
	GameplayTags.SkillTree_LightningCoil_CorrodedBuildup_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.CorrodedBuildup.7"),
		FString("+2 to Corroded Buildup per bolt")
		);
	GameplayTags.SkillTree_LightningCoil_Convergence_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.Convergence.1"), FString("Cone Convergence — 10% per bolt, -30% more damage"));
	GameplayTags.SkillTree_LightningCoil_Convergence_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.Convergence.2"), FString("Cone Convergence +5%"));
	GameplayTags.SkillTree_LightningCoil_Convergence_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.Convergence.3"), FString("Cone Convergence +5%"));
	GameplayTags.SkillTree_LightningCoil_ExtraBolts_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ExtraBolts.1"), FString("+1 Bolt"));
	GameplayTags.SkillTree_LightningCoil_ExtraBolts_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ExtraBolts.2"), FString("+1 Bolt"));
	GameplayTags.SkillTree_LightningCoil_ExtraBolts_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ExtraBolts.3"), FString("-1 Bolt (chain trade-off)"));
	GameplayTags.SkillTree_LightningCoil_ConeLength_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ConeLength.1"), FString("+50cm Cone Length"));
	GameplayTags.SkillTree_LightningCoil_ConeLength_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ConeLength.2"), FString("+50cm Cone Length"));
	GameplayTags.SkillTree_LightningCoil_ConeLength_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ConeLength.3"), FString("+50cm Cone Length"));
	GameplayTags.SkillTree_LightningCoil_ConeLength_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ConeLength.4"), FString("+50cm Cone Length"));
	GameplayTags.SkillTree_LightningCoil_ConeLength_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ConeLength.5"), FString("+50cm Cone Length"));
	GameplayTags.SkillTree_LightningCoil_ConeAngle_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ConeAngle.1"), FString("+5 degrees Cone Angle"));
	GameplayTags.SkillTree_LightningCoil_ConeAngle_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ConeAngle.2"), FString("+5 degrees Cone Angle"));
	GameplayTags.SkillTree_LightningCoil_ConeAngle_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ConeAngle.3"), FString("+5 degrees Cone Angle"));
	GameplayTags.SkillTree_LightningCoil_ConeAngle_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ConeAngle.4"), FString("+5 degrees Cone Angle"));
	GameplayTags.SkillTree_LightningCoil_ConeAngle_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ConeAngle.5"), FString("+5 degrees Cone Angle"));
	GameplayTags.SkillTree_LightningCoil_ChainCount_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ChainCount.1"), FString("+1 Chain Jump"));
	GameplayTags.SkillTree_LightningCoil_ChainCount_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ChainCount.2"), FString("+1 Chain Jump"));
	GameplayTags.SkillTree_LightningCoil_ChainCount_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ChainCount.3"), FString("+1 Chain Jump"));
	GameplayTags.SkillTree_LightningCoil_ChainRadius_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ChainRadius.1"), FString("+100cm Chain Radius"));
	GameplayTags.SkillTree_LightningCoil_ChainRadius_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ChainRadius.2"), FString("+100cm Chain Radius"));
	GameplayTags.SkillTree_LightningCoil_ChainRadius_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ChainRadius.3"), FString("+100cm Chain Radius"));
	GameplayTags.SkillTree_LightningCoil_ChainRadius_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ChainRadius.4"), FString("+100cm Chain Radius"));
	GameplayTags.SkillTree_LightningCoil_ChainRadius_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.ChainRadius.5"), FString("+100cm Chain Radius"));
	GameplayTags.SkillTree_LightningCoil_DistanceLuckyRolls_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.DistanceLuckyRolls.1"), FString("+1 Distance Lucky Roll"));
	GameplayTags.SkillTree_LightningCoil_DistanceLuckyRolls_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.DistanceLuckyRolls.2"), FString("+1 Distance Lucky Roll"));
	GameplayTags.SkillTree_LightningCoil_DistanceLuckyRolls_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.DistanceLuckyRolls.3"), FString("+1 Distance Lucky Roll"));
	GameplayTags.SkillTree_LightningCoil_DamageLuckyRolls_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.DamageLuckyRolls.1"), FString("+1 Damage Lucky Roll"));
	GameplayTags.SkillTree_LightningCoil_DamageLuckyRolls_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.DamageLuckyRolls.2"), FString("+1 Damage Lucky Roll"));
	GameplayTags.SkillTree_LightningCoil_DamageLuckyRolls_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SkillTree.LightningCoil.DamageLuckyRolls.3"), FString("+1 Damage Lucky Roll"));

	//SkillPassive
	GameplayTags.SkillPassive_LightningCoil_IncreasedDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.LightningCoil.IncreasedDamage"),
		FString("Per-skill passive: Increased Damage")
		);
	GameplayTags.SkillPassive_LightningCoil_IncreasedCritChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.LightningCoil.IncreasedCritChance"),
		FString("Per-skill passive: Increased Crit Chance")
		);
	GameplayTags.SkillPassive_LightningCoil_BoltAoERadius = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.LightningCoil.BoltAoERadius"),
		FString("Per-skill passive: Bolt Ao ERadius")
		);
	GameplayTags.SkillPassive_LightningCoil_CorrodedBuildup = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.LightningCoil.CorrodedBuildup"),
		FString("Per-skill passive: Corroded Buildup")
		);
	GameplayTags.SkillPassive_LightningCoil_MoreDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.LightningCoil.MoreDamage"),
		FString("Per-skill passive: Less/More Damage multiplier")
		);
	GameplayTags.SkillPassive_LightningCoil_ConvergenceRate = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.LightningCoil.ConvergenceRate"),
		FString("Per-skill passive: cone convergence rate per bolt fired")
		);
	GameplayTags.SkillPassive_LightningCoil_ExtraBolts = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.LightningCoil.ExtraBolts"),
		FString("Per-skill passive: flat bolt count modifier (can be negative)")
		);
	GameplayTags.SkillPassive_LightningCoil_ConeLength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.LightningCoil.ConeLength"),
		FString("Per-skill passive: flat cone length addition (cm)")
		);
	GameplayTags.SkillPassive_LightningCoil_ConeAngle = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.LightningCoil.ConeAngle"),
		FString("Per-skill passive: flat cone half-angle addition (degrees)")
		);
	GameplayTags.SkillPassive_LightningCoil_ChainCount = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.LightningCoil.ChainCount"),
		FString("Per-skill passive: flat chain jump count addition")
		);
	GameplayTags.SkillPassive_LightningCoil_ChainRadius = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.LightningCoil.ChainRadius"),
		FString("Per-skill passive: flat chain search radius addition (cm)")
		);
	GameplayTags.SkillPassive_LightningCoil_DistanceLuckyRolls = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.LightningCoil.DistanceLuckyRolls"),
		FString("Per-skill passive: extra distance rolls (take furthest)")
		);
	GameplayTags.SkillPassive_LightningCoil_DamageLuckyRolls = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.LightningCoil.DamageLuckyRolls"),
		FString("Per-skill passive: extra damage rolls (take highest)")
		);
	GameplayTags.SkillPassive_LightningCoil_CastSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.LightningCoil.CastSpeed"),
		FString("Per-skill passive: increased skill cast speed")
		);

	GameplayTags.SkillPassive_Impact_InitialRadius = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.Impact.InitialRadius"),
		FString("Per-skill passive: flat radius addition (cm) to Impact's initial weapon-tip AoE")
		);
	GameplayTags.SkillPassive_Impact_SecondaryRadius = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.Impact.SecondaryRadius"),
		FString("Per-skill passive: flat radius addition (cm) to Impact's secondary/chained AoE")
		);
	GameplayTags.SkillPassive_Impact_MaxChainDepth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.Impact.MaxChainDepth"),
		FString("Per-skill passive: additional chain generations beyond the base (default 1: initial hit + one secondary generation)")
		);
	GameplayTags.SkillPassive_Impact_SecondaryDamageMultiplier = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.Impact.SecondaryDamageMultiplier"),
		FString("Per-skill passive: delta to secondary/chained AoE damage (0 = same as initial hit)")
		);
	GameplayTags.SkillPassive_Impact_ChainDelayModifier = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.Impact.ChainDelayModifier"),
		FString("Per-skill passive: delta to Impact's chain delay multiplier, stacks with the global Duration.Skill attribute")
		);

	GameplayTags.SkillTree_LightningCoil_SkillSpeed_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.SkillSpeed.1"),
		FString("5% Increased Skill Speed")
		);
	GameplayTags.SkillTree_LightningCoil_SkillSpeed_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.LightningCoil.SkillSpeed.2"),
		FString("5% Increased Skill Speed")
		);

	// ---- Leap SkillTree nodes ----
	GameplayTags.SkillTree_Leap_ExtraRange_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.Leap.ExtraRange.1"),
		FString("Extra 200cm leap range")
		);
	GameplayTags.SkillTree_Leap_ExtraRange_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.Leap.ExtraRange.2"),
		FString("Extra 200cm leap range")
		);
	GameplayTags.SkillTree_Leap_ExtraRange_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.Leap.ExtraRange.3"),
		FString("Extra 200cm leap range")
		);
	GameplayTags.SkillTree_Leap_ExplosiveLanding_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.Leap.ExplosiveLanding.1"),
		FString("Deals explosive AoE damage on landing")
		);
	GameplayTags.SkillTree_Leap_NoWindup_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.Leap.NoWindup.1"),
		FString("Removes windup animation before the leap")
		);

	GameplayTags.SkillTree_Leap_HeatCost_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.Leap.HeatCost.1"),
		FString("Adds heat cost per cast")
		);
	GameplayTags.SkillTree_Leap_HeatCost_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.Leap.HeatCost.2"),
		FString("Adds heat cost per cast")
		);
	GameplayTags.SkillTree_Leap_HeatCost_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.Leap.HeatCost.3"),
		FString("Adds heat cost per cast")
		);
	GameplayTags.SkillTree_Leap_HeatCost_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.Leap.HeatCost.4"),
		FString("Adds heat cost per cast")
		);
	GameplayTags.SkillTree_Leap_HeatCost_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.Leap.HeatCost.5"),
		FString("Adds heat cost per cast")
		);
	GameplayTags.SkillTree_Leap_HeatCost_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.Leap.HeatCost.6"),
		FString("Adds heat cost per cast")
		);
	GameplayTags.SkillTree_Leap_CooldownDuration_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.Leap.CooldownDuration.1"),
		FString("Reduces cooldown duration")
		);
	GameplayTags.SkillTree_Leap_CooldownDuration_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.Leap.CooldownDuration.2"),
		FString("Reduces cooldown duration")
		);
	GameplayTags.SkillTree_Leap_CooldownDuration_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.Leap.CooldownDuration.3"),
		FString("Reduces cooldown duration")
		);
	GameplayTags.SkillTree_Leap_CooldownDuration_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillTree.Leap.CooldownDuration.4"),
		FString("Reduces cooldown duration")
		);

	// ---- Leap SkillPassive values ----
	GameplayTags.SkillPassive_Leap_ExtraRange = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.Leap.ExtraRange"),
		FString("Per-skill passive: extra leap range in cm")
		);
	GameplayTags.SkillPassive_Leap_ExplosiveLanding = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.Leap.ExplosiveLanding"),
		FString("Per-skill passive: flag — >0 enables landing explosion")
		);
	GameplayTags.SkillPassive_Leap_NoWindup = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.Leap.NoWindup"),
		FString("Per-skill passive: flag — >0 disables windup animation")
		);
	GameplayTags.SkillPassive_Leap_HeatCost = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.Leap.HeatCost"),
		FString("Per-skill passive: accumulated heat generated per cast")
		);
	GameplayTags.SkillPassive_Leap_CooldownDuration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SkillPassive.Leap.CooldownDuration"),
		FString("Per-skill passive: accumulated cooldown reduction in seconds")
		);

	// ---- GRID TEST TAGS — DELETE AFTER PLACEMENT TESTING ----
	auto R=[](FFP_GameplayTags& GT, FGameplayTag& Tag, const char* Name){ Tag=UGameplayTagsManager::Get().AddNativeGameplayTag(FName(Name),FString("Grid test")); };
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N001,"SkillTree.LightningCoil.Grid.N001"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N002,"SkillTree.LightningCoil.Grid.N002"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N003,"SkillTree.LightningCoil.Grid.N003"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N004,"SkillTree.LightningCoil.Grid.N004");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N005,"SkillTree.LightningCoil.Grid.N005"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N006,"SkillTree.LightningCoil.Grid.N006"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N007,"SkillTree.LightningCoil.Grid.N007"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N008,"SkillTree.LightningCoil.Grid.N008");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N009,"SkillTree.LightningCoil.Grid.N009"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N010,"SkillTree.LightningCoil.Grid.N010"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N011,"SkillTree.LightningCoil.Grid.N011"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N012,"SkillTree.LightningCoil.Grid.N012");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N013,"SkillTree.LightningCoil.Grid.N013"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N014,"SkillTree.LightningCoil.Grid.N014"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N015,"SkillTree.LightningCoil.Grid.N015"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N016,"SkillTree.LightningCoil.Grid.N016");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N017,"SkillTree.LightningCoil.Grid.N017"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N018,"SkillTree.LightningCoil.Grid.N018"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N019,"SkillTree.LightningCoil.Grid.N019"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N020,"SkillTree.LightningCoil.Grid.N020");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N021,"SkillTree.LightningCoil.Grid.N021"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N022,"SkillTree.LightningCoil.Grid.N022"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N023,"SkillTree.LightningCoil.Grid.N023"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N024,"SkillTree.LightningCoil.Grid.N024");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N025,"SkillTree.LightningCoil.Grid.N025"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N026,"SkillTree.LightningCoil.Grid.N026"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N027,"SkillTree.LightningCoil.Grid.N027"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N028,"SkillTree.LightningCoil.Grid.N028");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N029,"SkillTree.LightningCoil.Grid.N029"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N030,"SkillTree.LightningCoil.Grid.N030"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N031,"SkillTree.LightningCoil.Grid.N031"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N032,"SkillTree.LightningCoil.Grid.N032");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N033,"SkillTree.LightningCoil.Grid.N033"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N034,"SkillTree.LightningCoil.Grid.N034"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N035,"SkillTree.LightningCoil.Grid.N035"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N036,"SkillTree.LightningCoil.Grid.N036");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N037,"SkillTree.LightningCoil.Grid.N037"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N038,"SkillTree.LightningCoil.Grid.N038"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N039,"SkillTree.LightningCoil.Grid.N039"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N040,"SkillTree.LightningCoil.Grid.N040");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N041,"SkillTree.LightningCoil.Grid.N041"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N042,"SkillTree.LightningCoil.Grid.N042"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N043,"SkillTree.LightningCoil.Grid.N043"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N044,"SkillTree.LightningCoil.Grid.N044");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N045,"SkillTree.LightningCoil.Grid.N045"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N046,"SkillTree.LightningCoil.Grid.N046"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N047,"SkillTree.LightningCoil.Grid.N047"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N048,"SkillTree.LightningCoil.Grid.N048");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N049,"SkillTree.LightningCoil.Grid.N049"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N050,"SkillTree.LightningCoil.Grid.N050"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N051,"SkillTree.LightningCoil.Grid.N051"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N052,"SkillTree.LightningCoil.Grid.N052");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N053,"SkillTree.LightningCoil.Grid.N053"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N054,"SkillTree.LightningCoil.Grid.N054"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N055,"SkillTree.LightningCoil.Grid.N055"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N056,"SkillTree.LightningCoil.Grid.N056");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N057,"SkillTree.LightningCoil.Grid.N057"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N058,"SkillTree.LightningCoil.Grid.N058"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N059,"SkillTree.LightningCoil.Grid.N059"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N060,"SkillTree.LightningCoil.Grid.N060");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N061,"SkillTree.LightningCoil.Grid.N061"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N062,"SkillTree.LightningCoil.Grid.N062"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N063,"SkillTree.LightningCoil.Grid.N063"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N064,"SkillTree.LightningCoil.Grid.N064");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N065,"SkillTree.LightningCoil.Grid.N065"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N066,"SkillTree.LightningCoil.Grid.N066"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N067,"SkillTree.LightningCoil.Grid.N067"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N068,"SkillTree.LightningCoil.Grid.N068");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N069,"SkillTree.LightningCoil.Grid.N069"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N070,"SkillTree.LightningCoil.Grid.N070"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N071,"SkillTree.LightningCoil.Grid.N071"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N072,"SkillTree.LightningCoil.Grid.N072");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N073,"SkillTree.LightningCoil.Grid.N073"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N074,"SkillTree.LightningCoil.Grid.N074"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N075,"SkillTree.LightningCoil.Grid.N075"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N076,"SkillTree.LightningCoil.Grid.N076");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N077,"SkillTree.LightningCoil.Grid.N077"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N078,"SkillTree.LightningCoil.Grid.N078"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N079,"SkillTree.LightningCoil.Grid.N079"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N080,"SkillTree.LightningCoil.Grid.N080");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N081,"SkillTree.LightningCoil.Grid.N081"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N082,"SkillTree.LightningCoil.Grid.N082"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N083,"SkillTree.LightningCoil.Grid.N083"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N084,"SkillTree.LightningCoil.Grid.N084");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N085,"SkillTree.LightningCoil.Grid.N085"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N086,"SkillTree.LightningCoil.Grid.N086"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N087,"SkillTree.LightningCoil.Grid.N087"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N088,"SkillTree.LightningCoil.Grid.N088");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N089,"SkillTree.LightningCoil.Grid.N089"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N090,"SkillTree.LightningCoil.Grid.N090"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N091,"SkillTree.LightningCoil.Grid.N091"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N092,"SkillTree.LightningCoil.Grid.N092");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N093,"SkillTree.LightningCoil.Grid.N093"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N094,"SkillTree.LightningCoil.Grid.N094"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N095,"SkillTree.LightningCoil.Grid.N095"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N096,"SkillTree.LightningCoil.Grid.N096");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N097,"SkillTree.LightningCoil.Grid.N097"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N098,"SkillTree.LightningCoil.Grid.N098"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N099,"SkillTree.LightningCoil.Grid.N099"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N100,"SkillTree.LightningCoil.Grid.N100");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N101,"SkillTree.LightningCoil.Grid.N101"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N102,"SkillTree.LightningCoil.Grid.N102"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N103,"SkillTree.LightningCoil.Grid.N103"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N104,"SkillTree.LightningCoil.Grid.N104");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N105,"SkillTree.LightningCoil.Grid.N105"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N106,"SkillTree.LightningCoil.Grid.N106"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N107,"SkillTree.LightningCoil.Grid.N107"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N108,"SkillTree.LightningCoil.Grid.N108");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N109,"SkillTree.LightningCoil.Grid.N109"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N110,"SkillTree.LightningCoil.Grid.N110"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N111,"SkillTree.LightningCoil.Grid.N111"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N112,"SkillTree.LightningCoil.Grid.N112");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N113,"SkillTree.LightningCoil.Grid.N113"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N114,"SkillTree.LightningCoil.Grid.N114"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N115,"SkillTree.LightningCoil.Grid.N115"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N116,"SkillTree.LightningCoil.Grid.N116");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N117,"SkillTree.LightningCoil.Grid.N117"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N118,"SkillTree.LightningCoil.Grid.N118"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N119,"SkillTree.LightningCoil.Grid.N119"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N120,"SkillTree.LightningCoil.Grid.N120");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N121,"SkillTree.LightningCoil.Grid.N121"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N122,"SkillTree.LightningCoil.Grid.N122"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N123,"SkillTree.LightningCoil.Grid.N123"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N124,"SkillTree.LightningCoil.Grid.N124");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N125,"SkillTree.LightningCoil.Grid.N125"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N126,"SkillTree.LightningCoil.Grid.N126"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N127,"SkillTree.LightningCoil.Grid.N127"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N128,"SkillTree.LightningCoil.Grid.N128");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N129,"SkillTree.LightningCoil.Grid.N129"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N130,"SkillTree.LightningCoil.Grid.N130"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N131,"SkillTree.LightningCoil.Grid.N131"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N132,"SkillTree.LightningCoil.Grid.N132");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N133,"SkillTree.LightningCoil.Grid.N133"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N134,"SkillTree.LightningCoil.Grid.N134"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N135,"SkillTree.LightningCoil.Grid.N135"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N136,"SkillTree.LightningCoil.Grid.N136");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N137,"SkillTree.LightningCoil.Grid.N137"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N138,"SkillTree.LightningCoil.Grid.N138"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N139,"SkillTree.LightningCoil.Grid.N139"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N140,"SkillTree.LightningCoil.Grid.N140");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N141,"SkillTree.LightningCoil.Grid.N141"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N142,"SkillTree.LightningCoil.Grid.N142"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N143,"SkillTree.LightningCoil.Grid.N143"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N144,"SkillTree.LightningCoil.Grid.N144");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N145,"SkillTree.LightningCoil.Grid.N145"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N146,"SkillTree.LightningCoil.Grid.N146"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N147,"SkillTree.LightningCoil.Grid.N147"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N148,"SkillTree.LightningCoil.Grid.N148");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N149,"SkillTree.LightningCoil.Grid.N149"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N150,"SkillTree.LightningCoil.Grid.N150"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N151,"SkillTree.LightningCoil.Grid.N151"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N152,"SkillTree.LightningCoil.Grid.N152");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N153,"SkillTree.LightningCoil.Grid.N153"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N154,"SkillTree.LightningCoil.Grid.N154"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N155,"SkillTree.LightningCoil.Grid.N155"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N156,"SkillTree.LightningCoil.Grid.N156");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N157,"SkillTree.LightningCoil.Grid.N157"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N158,"SkillTree.LightningCoil.Grid.N158"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N159,"SkillTree.LightningCoil.Grid.N159"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N160,"SkillTree.LightningCoil.Grid.N160");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N161,"SkillTree.LightningCoil.Grid.N161"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N162,"SkillTree.LightningCoil.Grid.N162"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N163,"SkillTree.LightningCoil.Grid.N163"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N164,"SkillTree.LightningCoil.Grid.N164");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N165,"SkillTree.LightningCoil.Grid.N165"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N166,"SkillTree.LightningCoil.Grid.N166"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N167,"SkillTree.LightningCoil.Grid.N167"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N168,"SkillTree.LightningCoil.Grid.N168");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N169,"SkillTree.LightningCoil.Grid.N169"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N170,"SkillTree.LightningCoil.Grid.N170"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N171,"SkillTree.LightningCoil.Grid.N171"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N172,"SkillTree.LightningCoil.Grid.N172");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N173,"SkillTree.LightningCoil.Grid.N173"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N174,"SkillTree.LightningCoil.Grid.N174"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N175,"SkillTree.LightningCoil.Grid.N175"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N176,"SkillTree.LightningCoil.Grid.N176");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N177,"SkillTree.LightningCoil.Grid.N177"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N178,"SkillTree.LightningCoil.Grid.N178"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N179,"SkillTree.LightningCoil.Grid.N179"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N180,"SkillTree.LightningCoil.Grid.N180");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N181,"SkillTree.LightningCoil.Grid.N181"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N182,"SkillTree.LightningCoil.Grid.N182"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N183,"SkillTree.LightningCoil.Grid.N183"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N184,"SkillTree.LightningCoil.Grid.N184");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N185,"SkillTree.LightningCoil.Grid.N185"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N186,"SkillTree.LightningCoil.Grid.N186"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N187,"SkillTree.LightningCoil.Grid.N187"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N188,"SkillTree.LightningCoil.Grid.N188");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N189,"SkillTree.LightningCoil.Grid.N189"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N190,"SkillTree.LightningCoil.Grid.N190"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N191,"SkillTree.LightningCoil.Grid.N191"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N192,"SkillTree.LightningCoil.Grid.N192");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N193,"SkillTree.LightningCoil.Grid.N193"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N194,"SkillTree.LightningCoil.Grid.N194"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N195,"SkillTree.LightningCoil.Grid.N195"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N196,"SkillTree.LightningCoil.Grid.N196");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N197,"SkillTree.LightningCoil.Grid.N197"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N198,"SkillTree.LightningCoil.Grid.N198"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N199,"SkillTree.LightningCoil.Grid.N199"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N200,"SkillTree.LightningCoil.Grid.N200");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N201,"SkillTree.LightningCoil.Grid.N201"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N202,"SkillTree.LightningCoil.Grid.N202"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N203,"SkillTree.LightningCoil.Grid.N203"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N204,"SkillTree.LightningCoil.Grid.N204");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N205,"SkillTree.LightningCoil.Grid.N205"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N206,"SkillTree.LightningCoil.Grid.N206"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N207,"SkillTree.LightningCoil.Grid.N207"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N208,"SkillTree.LightningCoil.Grid.N208");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N209,"SkillTree.LightningCoil.Grid.N209"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N210,"SkillTree.LightningCoil.Grid.N210"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N211,"SkillTree.LightningCoil.Grid.N211"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N212,"SkillTree.LightningCoil.Grid.N212");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N213,"SkillTree.LightningCoil.Grid.N213"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N214,"SkillTree.LightningCoil.Grid.N214"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N215,"SkillTree.LightningCoil.Grid.N215"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N216,"SkillTree.LightningCoil.Grid.N216");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N217,"SkillTree.LightningCoil.Grid.N217"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N218,"SkillTree.LightningCoil.Grid.N218"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N219,"SkillTree.LightningCoil.Grid.N219"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N220,"SkillTree.LightningCoil.Grid.N220");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N221,"SkillTree.LightningCoil.Grid.N221"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N222,"SkillTree.LightningCoil.Grid.N222"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N223,"SkillTree.LightningCoil.Grid.N223"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N224,"SkillTree.LightningCoil.Grid.N224");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N225,"SkillTree.LightningCoil.Grid.N225"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N226,"SkillTree.LightningCoil.Grid.N226"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N227,"SkillTree.LightningCoil.Grid.N227"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N228,"SkillTree.LightningCoil.Grid.N228");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N229,"SkillTree.LightningCoil.Grid.N229"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N230,"SkillTree.LightningCoil.Grid.N230"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N231,"SkillTree.LightningCoil.Grid.N231"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N232,"SkillTree.LightningCoil.Grid.N232");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N233,"SkillTree.LightningCoil.Grid.N233"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N234,"SkillTree.LightningCoil.Grid.N234"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N235,"SkillTree.LightningCoil.Grid.N235"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N236,"SkillTree.LightningCoil.Grid.N236");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N237,"SkillTree.LightningCoil.Grid.N237"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N238,"SkillTree.LightningCoil.Grid.N238"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N239,"SkillTree.LightningCoil.Grid.N239"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N240,"SkillTree.LightningCoil.Grid.N240");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N241,"SkillTree.LightningCoil.Grid.N241"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N242,"SkillTree.LightningCoil.Grid.N242"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N243,"SkillTree.LightningCoil.Grid.N243"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N244,"SkillTree.LightningCoil.Grid.N244");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N245,"SkillTree.LightningCoil.Grid.N245"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N246,"SkillTree.LightningCoil.Grid.N246"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N247,"SkillTree.LightningCoil.Grid.N247"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N248,"SkillTree.LightningCoil.Grid.N248");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N249,"SkillTree.LightningCoil.Grid.N249"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N250,"SkillTree.LightningCoil.Grid.N250"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N251,"SkillTree.LightningCoil.Grid.N251"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N252,"SkillTree.LightningCoil.Grid.N252");
	R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N253,"SkillTree.LightningCoil.Grid.N253"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N254,"SkillTree.LightningCoil.Grid.N254"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N255,"SkillTree.LightningCoil.Grid.N255"); R(GameplayTags,GameplayTags.SkillTree_LightningCoil_Grid_N256,"SkillTree.LightningCoil.Grid.N256");
	// ---- END GRID TEST TAGS ----
}