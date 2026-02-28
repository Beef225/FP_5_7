// Copyright JG
#pragma once

#include "CoreMinimal.h"


UENUM(BlueprintType)
enum class EWeaponGripStyle : uint8
{
	Unarmed            UMETA(DisplayName="Unarmed"),
	Pistol1H           UMETA(DisplayName="Pistol (1H)"),
	Pistol2H           UMETA(DisplayName="Pistol (2H Support)"),
	Rifle2H            UMETA(DisplayName="Rifle (2H)"),
	Shotgun2H          UMETA(DisplayName="Shotgun (2H)"),
	Sword1H            UMETA(DisplayName="Sword (1H)"),
	Sword2H            UMETA(DisplayName="Sword (2H)"),
	Axe1H              UMETA(DisplayName="Axe (1H)"),
	Axe2H              UMETA(DisplayName="Axe (2H)"),
	Club1H            UMETA(DisplayName="Club (1H)"),
	Club2H            UMETA(DisplayName="Club (1H)"),
	Dual1H             UMETA(DisplayName="Dual 1H"),
	HeavyHip           UMETA(DisplayName="Heavy (Hip-fire / Minigun)"),
	ShoulderLauncher   UMETA(DisplayName="Shoulder Launcher"),
};

UENUM(BlueprintType)
enum class ELocomotionMode : uint8
{
	Idle        UMETA(DisplayName="Idle"),
	Walk        UMETA(DisplayName="Walk"),
	Run         UMETA(DisplayName="Run"),
	Roll        UMETA(DisplayName="Roll"),
	Sprint      UMETA(DisplayName="Sprint"),
	Float       UMETA(DisplayName="Float"),
	Crouch      UMETA(DisplayName="Crouch"),
	Crawl       UMETA(DisplayName="Crawl"),
	Leap        UMETA(DisplayName="Leap"),
};

UENUM(BlueprintType)
enum class EDisplayType : uint8
{
	Numerical   UMETA(DisplayName="Numerical"),
	Percentage  UMETA(DisplayName="Percentage"),
	
};

UENUM(BlueprintType)
enum class EMenu : uint8
{
	Character		UMETA(DisplayName="Character"),
	Skills			UMETA(DisplayName="Skills"),
	PassiveTree		UMETA(DisplayName="PassiveTree"),
	Inventory		UMETA(DisplayName="Inventory"),
	Settings		UMETA(DisplayName="Settings"),
	CloseGame		UMETA(DisplayName="CloseGame")
	
};

UENUM(BlueprintType)
enum class ESkillGrantedBy : uint8
{
	Equipment		UMETA(DisplayName="Equipment"),
	Startup			UMETA(DisplayName="Startup"),
	PassiveTree		UMETA(DisplayName="PassiveTree"),
	SkillTree		UMETA(DisplayName="SkillTree"),
	Quest			UMETA(DisplayName="Quest"),
	Injector		UMETA(DisplayName="Injector"),
};