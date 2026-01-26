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
	//Damage
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

}