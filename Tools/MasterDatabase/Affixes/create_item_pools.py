"""
Creates one pool sheet per character equipment slot in AffixDatabase.xlsm.

Column schema (row 1 = header):
  A  GroupTag     — the affix's group (mutually exclusive within group)
  B  AffixID      — the specific affix within that group that can roll
  C  PoolWeight   — relative frequency of picking this AffixID from the pool
  D  AffinityTag  — optional; locks damage-type coherence across groups on the same item
                    First affix with an AffinityTag that rolls sets the item's affinity;
                    subsequent affixes in the same pool whose AffinityTag != locked affinity
                    are excluded for that roll.

Rolling algorithm (two-stage):
  1. Pick AffixID  — weighted random from this pool (respecting locked affinity)
  2. Pick Tier     — weighted random from AffixDatabase rows for that AffixID (uses Weight col)
  3. Pick Value    — uniform random in [Stat1_Min, Stat1_Max] at Stat1_Step

Sheets created (skipped if already exist):
  Weapon_Ranged, Weapon_Melee, Weapon_Utility
  Headwear_Might, Headwear_Agility
  BodyArmour_Might, BodyArmour_Agility
  Gloves_Might, Gloves_Agility
  Boots_Might, Boots_Agility
  Belt
  Ring, Amulet
"""
import openpyxl
from openpyxl.styles import Font, PatternFill, Alignment
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

HEADERS = ["GroupTag", "AffixID", "PoolWeight", "AffinityTag"]

# ── Affix pool definitions ─────────────────────────────────────────────────
# (GroupTag, AffixID, PoolWeight, AffinityTag)
# AffinityTag: "Physical" | "Energy" | "Chemical" | "Radiation" | "Explosive" | ""
# Pools are starters — expand as more affix batches are added.

POOLS = {

    # ══════════════════════════════════════════════════════════════════════════
    # WEAPONS
    # ══════════════════════════════════════════════════════════════════════════

    "Weapon_Ranged": [
        # Added damage — one damage type rolls; sets affinity
        ("Group.AddedDamage",           "AddedPhysicalDamage",                  100, "Physical"),
        ("Group.AddedDamage",           "AddedEnergyDamage",                    100, "Energy"),
        ("Group.AddedDamage",           "AddedExplosiveDamage",                 100, "Explosive"),
        ("Group.AddedDamage",           "AddedChemicalDamage",                   50, "Chemical"),
        ("Group.AddedDamage",           "AddedRadiationDamage",                  50, "Radiation"),
        # Increased damage type — affinityTag ensures it matches the added damage type
        ("Group.IncreasedDamageType",   "IncreasedPhysicalDamage",              250, "Physical"),
        ("Group.IncreasedDamageType",   "IncreasedEnergyDamage",                250, "Energy"),
        ("Group.IncreasedDamageType",   "IncreasedExplosiveDamage",             250, "Explosive"),
        ("Group.IncreasedDamageType",   "IncreasedChemicalDamage",              150, "Chemical"),
        ("Group.IncreasedDamageType",   "IncreasedRadiationDamage",             150, "Radiation"),
        # Increased weapon type — no affinity needed (weapon-type, not damage-type)
        ("Group.IncreasedWeaponDamage", "IncreasedPistolDamage",                100, ""),
        ("Group.IncreasedWeaponDamage", "IncreasedRifleDamage",                 100, ""),
        ("Group.IncreasedWeaponDamage", "IncreasedShotgunDamage",               100, ""),
        ("Group.IncreasedWeaponDamage", "IncreasedGrenadeDamage",                75, ""),
        ("Group.IncreasedWeaponDamage", "IncreasedLauncherDamage",               50, ""),
        ("Group.IncreasedWeaponDamage", "IncreasedDroneDamage",                  75, ""),
        # Generic projectile
        ("Group.GenericDamage",         "IncreasedProjectileDamage",            200, ""),
        # Critical
        ("Group.CriticalStrikeChance",          "CriticalStrikeChance",         150, ""),
        ("Group.IncreasedCriticalStrikeChance", "IncreasedCriticalStrikeChance",200, ""),
        ("Group.CriticalStrikeMultiplier",      "CriticalStrikeMultiplier",     150, ""),
        # Debuff effectiveness — affinityTag ties to damage type
        ("Group.DebuffEffectiveness",   "BruisedEffectiveness",                  75, "Physical"),
        ("Group.DebuffEffectiveness",   "CorrodedEffectiveness",                 75, "Chemical"),
        ("Group.DebuffEffectiveness",   "IrradiatedEffectiveness",               75, "Radiation"),
        ("Group.DebuffEffectiveness",   "ThermalDestabilizationEffectiveness",   75, "Energy"),
        ("Group.DebuffEffectiveness",   "ConcussedEffectiveness",                75, "Explosive"),
        # Debuff threshold — matches same affinity
        ("Group.DebuffThreshold",       "BruisedThreshold",                      75, "Physical"),
        ("Group.DebuffThreshold",       "CorrodedThreshold",                     75, "Chemical"),
        ("Group.DebuffThreshold",       "IrradiatedThreshold",                   75, "Radiation"),
        ("Group.DebuffThreshold",       "ThermalDestabilizationThreshold",       75, "Energy"),
        ("Group.DebuffThreshold",       "ConcussedThreshold",                    75, "Explosive"),
    ],

    "Weapon_Melee": [
        # Physical and Chemical (electrical/acidic) are primary melee damage types
        ("Group.AddedDamage",           "AddedPhysicalDamage",                  150, "Physical"),
        ("Group.AddedDamage",           "AddedChemicalDamage",                  100, "Chemical"),
        ("Group.AddedDamage",           "AddedExplosiveDamage",                  50, "Explosive"),
        ("Group.IncreasedDamageType",   "IncreasedPhysicalDamage",              300, "Physical"),
        ("Group.IncreasedDamageType",   "IncreasedChemicalDamage",              200, "Chemical"),
        ("Group.IncreasedDamageType",   "IncreasedExplosiveDamage",             100, "Explosive"),
        # Melee weapon types
        ("Group.IncreasedWeaponDamage", "IncreasedSwordDamage",                 150, ""),
        ("Group.IncreasedWeaponDamage", "IncreasedAxeDamage",                   150, ""),
        ("Group.IncreasedWeaponDamage", "IncreasedClubDamage",                  150, ""),
        ("Group.IncreasedWeaponDamage", "IncreasedUnarmedDamage",               100, ""),
        # Generic melee
        ("Group.GenericDamage",         "IncreasedMeleeDamage",                 200, ""),
        # Critical
        ("Group.CriticalStrikeChance",          "CriticalStrikeChance",         150, ""),
        ("Group.IncreasedCriticalStrikeChance", "IncreasedCriticalStrikeChance",150, ""),
        ("Group.CriticalStrikeMultiplier",      "CriticalStrikeMultiplier",     200, ""),
        # Debuff
        ("Group.DebuffEffectiveness",   "BruisedEffectiveness",                 150, "Physical"),
        ("Group.DebuffEffectiveness",   "ConcussedEffectiveness",               100, "Explosive"),
        ("Group.DebuffEffectiveness",   "CorrodedEffectiveness",                100, "Chemical"),
        ("Group.DebuffThreshold",       "BruisedThreshold",                     150, "Physical"),
        ("Group.DebuffThreshold",       "ConcussedThreshold",                   100, "Explosive"),
        ("Group.DebuffThreshold",       "CorrodedThreshold",                    100, "Chemical"),
    ],

    "Weapon_Utility": [
        # Lighter damage — utility weapons support rather than maximise one type
        ("Group.AddedDamage",           "AddedEnergyDamage",                    100, "Energy"),
        ("Group.AddedDamage",           "AddedChemicalDamage",                  100, "Chemical"),
        ("Group.IncreasedDamageType",   "IncreasedEnergyDamage",                150, "Energy"),
        ("Group.IncreasedDamageType",   "IncreasedChemicalDamage",              150, "Chemical"),
        ("Group.GenericDamage",         "IncreasedAoEDamage",                   150, ""),
        # Critical (moderate)
        ("Group.CriticalStrikeChance",  "CriticalStrikeChance",                 100, ""),
        # Heat / destabilization
        ("Group.ThermalMass",           "ThermalMass",                          100, ""),
        ("Group.CoolingCoefficient",    "CoolingCoefficient",                   100, ""),
        # Debuff effectiveness — all types available on utility weapons
        ("Group.DebuffEffectiveness",   "BruisedEffectiveness",                  75, "Physical"),
        ("Group.DebuffEffectiveness",   "ConcussedEffectiveness",                75, "Explosive"),
        ("Group.DebuffEffectiveness",   "IrradiatedEffectiveness",               75, "Radiation"),
        ("Group.DebuffEffectiveness",   "CorrodedEffectiveness",                 75, "Chemical"),
        ("Group.DebuffEffectiveness",   "ThermalDestabilizationEffectiveness",   75, "Energy"),
        ("Group.DebuffThreshold",       "BruisedThreshold",                      75, "Physical"),
        ("Group.DebuffThreshold",       "ConcussedThreshold",                    75, "Explosive"),
        ("Group.DebuffThreshold",       "IrradiatedThreshold",                   75, "Radiation"),
        ("Group.DebuffThreshold",       "CorrodedThreshold",                     75, "Chemical"),
        ("Group.DebuffThreshold",       "ThermalDestabilizationThreshold",       75, "Energy"),
    ],

    # ══════════════════════════════════════════════════════════════════════════
    # HEADWEAR
    # ══════════════════════════════════════════════════════════════════════════

    "Headwear_Might": [
        # Armour
        ("Group.Armour",                    "Armour",                           300, ""),
        ("Group.IncreasedArmour",           "IncreasedArmour",                  200, ""),
        # Max resistances
        ("Group.MaxResistance",     "MaxPhysicalResistance",            100, ""),
        ("Group.MaxResistance",       "MaxEnergyResistance",              100, ""),
        ("Group.MaxResistance",    "MaxExplosiveResistance",           100, ""),
        # Individual resistances
        ("Group.Resistance",        "PhysicalResistance",               200, ""),
        ("Group.Resistance",          "EnergyResistance",                 150, ""),
        ("Group.Resistance",       "ExplosiveResistance",              150, ""),
        # Health
        ("Group.HealthRegeneration",        "HealthRegeneration",               150, ""),
        # Debuff duration reduction
        ("Group.DebuffDuration",            "BruisedDuration",                  100, ""),
        ("Group.DebuffDuration",            "ConcussedDuration",                100, ""),
        # Block
        ("Group.BlockChance",               "BlockChance",                      100, ""),
    ],

    "Headwear_Agility": [
        # Dodge
        ("Group.ChanceToDodge",                     "ChanceToDodge",            300, ""),
        # Resistances
        ("Group.MaxResistance",               "MaxEnergyResistance",      100, ""),
        ("Group.MaxResistance",             "MaxChemicalResistance",    100, ""),
        ("Group.Resistance",                  "EnergyResistance",         200, ""),
        ("Group.Resistance",                "ChemicalResistance",       200, ""),
        ("Group.Resistance",               "RadiationResistance",      150, ""),
        # Critical hit resistance (defensive)
        ("Group.CriticalHitResistance",             "CriticalHitResistance",    150, ""),
        # Debuff duration
        ("Group.DebuffDuration",                    "IrradiatedDuration",       100, ""),
        ("Group.DebuffDuration",                    "CorrodedDuration",         100, ""),
        # Light armour
        ("Group.Armour",                            "Armour",                   100, ""),
    ],

    # ══════════════════════════════════════════════════════════════════════════
    # BODY ARMOUR
    # ══════════════════════════════════════════════════════════════════════════

    "BodyArmour_Might": [
        # Heavy armour
        ("Group.Armour",                        "Armour",                       400, ""),
        ("Group.IncreasedArmour",               "IncreasedArmour",              300, ""),
        # Max resistances — all types available on body
        ("Group.MaxResistance",         "MaxPhysicalResistance",        150, ""),
        ("Group.MaxResistance",           "MaxEnergyResistance",          150, ""),
        ("Group.MaxResistance",         "MaxChemicalResistance",        150, ""),
        ("Group.MaxResistance",        "MaxRadiationResistance",       100, ""),
        ("Group.MaxResistance",        "MaxExplosiveResistance",       150, ""),
        ("Group.MaxResistance",         "MaxOverheatResistance",        100, ""),
        # Individual resistances
        ("Group.Resistance",            "PhysicalResistance",           250, ""),
        ("Group.Resistance",              "EnergyResistance",             200, ""),
        ("Group.Resistance",           "ExplosiveResistance",          200, ""),
        # Damage reduction
        ("Group.ReducedDamageFromMelee",        "ReducedDamageFromMelee",       200, ""),
        ("Group.ReducedDamageFromProjectiles",  "ReducedDamageFromProjectiles", 200, ""),
        ("Group.ReducedDamageFromAoE",          "ReducedDamageFromAoE",         150, ""),
        # Health
        ("Group.HealthRegeneration",            "HealthRegeneration",           200, ""),
        # Deferred damage
        ("Group.DeferredDamageAmount",          "DeferredDamageAmount",         100, ""),
        ("Group.DeferredDamageTime",            "DeferredDamageTime",           100, ""),
        # Block
        ("Group.BlockChance",                   "BlockChance",                  150, ""),
        # Debuff durations
        ("Group.DebuffDuration",                "BruisedDuration",              100, ""),
        ("Group.DebuffDuration",                "ConcussedDuration",            100, ""),
    ],

    "BodyArmour_Agility": [
        # Dodge-heavy
        ("Group.ChanceToDodge",                     "ChanceToDodge",            400, ""),
        # Resistances
        ("Group.MaxResistance",               "MaxEnergyResistance",      150, ""),
        ("Group.MaxResistance",             "MaxChemicalResistance",    150, ""),
        ("Group.MaxResistance",            "MaxRadiationResistance",   100, ""),
        ("Group.MaxResistance",            "MaxExplosiveResistance",   150, ""),
        ("Group.Resistance",                  "EnergyResistance",         250, ""),
        ("Group.Resistance",                "ChemicalResistance",       200, ""),
        ("Group.Resistance",               "RadiationResistance",      200, ""),
        # Hybrid resistances (agility armour can mix defence types)
        ("Group.HybridResistance",    "HybridEnergyChemicalResistance",    150, ""),
        ("Group.HybridResistance",    "HybridPhysicalEnergyResistance",    150, ""),
        # Crit resistance
        ("Group.CriticalHitResistance",             "CriticalHitResistance",    200, ""),
        # Damage reduction
        ("Group.ReducedDamageFromProjectiles",      "ReducedDamageFromProjectiles", 200, ""),
        ("Group.ReducedDamageFromAoE",              "ReducedDamageFromAoE",     150, ""),
        # Health
        ("Group.HealthRegeneration",                "HealthRegeneration",       150, ""),
        # Debuff durations
        ("Group.DebuffDuration",                    "IrradiatedDuration",       100, ""),
        ("Group.DebuffDuration",                    "CorrodedDuration",         100, ""),
    ],

    # ══════════════════════════════════════════════════════════════════════════
    # GLOVES
    # ══════════════════════════════════════════════════════════════════════════

    "Gloves_Might": [
        # Armour
        ("Group.Armour",                    "Armour",                           250, ""),
        ("Group.IncreasedArmour",           "IncreasedArmour",                  150, ""),
        # Resistances
        ("Group.Resistance",        "PhysicalResistance",               200, ""),
        ("Group.Resistance",       "ExplosiveResistance",              150, ""),
        # Some offensive potential (melee gloves)
        ("Group.AddedDamage",               "AddedPhysicalDamage",              150, "Physical"),
        ("Group.IncreasedDamageType",       "IncreasedPhysicalDamage",          200, "Physical"),
        ("Group.IncreasedWeaponDamage",     "IncreasedUnarmedDamage",           150, ""),
        # Debuff
        ("Group.DebuffEffectiveness",       "BruisedEffectiveness",             100, "Physical"),
        ("Group.DebuffThreshold",           "BruisedThreshold",                 100, "Physical"),
        # Block
        ("Group.BlockChance",               "BlockChance",                      100, ""),
    ],

    "Gloves_Agility": [
        # Dodge
        ("Group.ChanceToDodge",                     "ChanceToDodge",            200, ""),
        # Crit (sharp hands, precise shots)
        ("Group.CriticalStrikeChance",              "CriticalStrikeChance",     200, ""),
        ("Group.IncreasedCriticalStrikeChance",     "IncreasedCriticalStrikeChance", 200, ""),
        ("Group.CriticalStrikeMultiplier",          "CriticalStrikeMultiplier", 150, ""),
        # Resistances
        ("Group.Resistance",                "ChemicalResistance",       150, ""),
        ("Group.Resistance",                  "EnergyResistance",         150, ""),
        # Crit hit resistance
        ("Group.CriticalHitResistance",             "CriticalHitResistance",    100, ""),
        # Ranged weapon damage bonus
        ("Group.IncreasedWeaponDamage",             "IncreasedPistolDamage",    100, ""),
        ("Group.IncreasedWeaponDamage",             "IncreasedRifleDamage",     100, ""),
    ],

    # ══════════════════════════════════════════════════════════════════════════
    # BOOTS
    # ══════════════════════════════════════════════════════════════════════════

    "Boots_Might": [
        # Armour
        ("Group.Armour",                    "Armour",                           300, ""),
        ("Group.IncreasedArmour",           "IncreasedArmour",                  150, ""),
        # Resistances
        ("Group.Resistance",        "PhysicalResistance",               200, ""),
        ("Group.Resistance",          "EnergyResistance",                 150, ""),
        ("Group.Resistance",       "ExplosiveResistance",              150, ""),
        ("Group.MaxResistance",     "MaxPhysicalResistance",            100, ""),
        # Damage reduction (from stomp/approach)
        ("Group.ReducedDamageFromMelee",    "ReducedDamageFromMelee",           150, ""),
        # Health
        ("Group.HealthRegeneration",        "HealthRegeneration",               150, ""),
        # Debuff
        ("Group.DebuffDuration",            "BruisedDuration",                  100, ""),
    ],

    "Boots_Agility": [
        # Dodge
        ("Group.ChanceToDodge",                 "ChanceToDodge",                350, ""),
        # Resistances
        ("Group.Resistance",              "EnergyResistance",             200, ""),
        ("Group.Resistance",            "ChemicalResistance",           150, ""),
        ("Group.Resistance",           "RadiationResistance",          150, ""),
        ("Group.MaxResistance",           "MaxEnergyResistance",          100, ""),
        # Crit resistance
        ("Group.CriticalHitResistance",         "CriticalHitResistance",        150, ""),
        # Hybrid (flexible movement armour)
        ("Group.HybridResistance","HybridPhysicalEnergyResistance",100, ""),
        # Debuff duration
        ("Group.DebuffDuration",                "IrradiatedDuration",           100, ""),
        ("Group.DebuffDuration",                "CorrodedDuration",             100, ""),
    ],

    # ══════════════════════════════════════════════════════════════════════════
    # BELT — mixed utility, survivability focus
    # ══════════════════════════════════════════════════════════════════════════

    "Belt": [
        # Health
        ("Group.HealthRegeneration",                "HealthRegeneration",       300, ""),
        # Deferred damage (belt = shock absorption)
        ("Group.DeferredDamageAmount",              "DeferredDamageAmount",     200, ""),
        ("Group.DeferredDamageTime",                "DeferredDamageTime",       150, ""),
        # Resistances
        ("Group.Resistance",                "PhysicalResistance",       150, ""),
        ("Group.Resistance",                  "EnergyResistance",         150, ""),
        ("Group.Resistance",                "ChemicalResistance",       150, ""),
        ("Group.Resistance",               "ExplosiveResistance",      150, ""),
        # Hybrid resistances
        ("Group.HybridResistance",    "HybridPhysicalEnergyResistance",    100, ""),
        ("Group.HybridResistance",  "HybridPhysicalChemicalResistance",  100, ""),
        ("Group.HybridResistance",   "HybridEnergyExplosiveResistance",   100, ""),
        # Debuff duration reductions (all 4 + thermal insulation)
        ("Group.DebuffDuration",                    "BruisedDuration",          100, ""),
        ("Group.DebuffDuration",                    "ConcussedDuration",        100, ""),
        ("Group.DebuffDuration",                    "IrradiatedDuration",       100, ""),
        ("Group.DebuffDuration",                    "CorrodedDuration",         100, ""),
        ("Group.DebuffDuration",                    "ThermalInsulation",        100, ""),
        # Heat system
        ("Group.ThermalMass",                       "ThermalMass",              100, ""),
        ("Group.CoolingCoefficient",                "CoolingCoefficient",       100, ""),
    ],

    # ══════════════════════════════════════════════════════════════════════════
    # RING — balanced offensive + defensive
    # ══════════════════════════════════════════════════════════════════════════

    "Ring": [
        # Damage (all types available, ring can enhance any build)
        ("Group.AddedDamage",               "AddedPhysicalDamage",              100, "Physical"),
        ("Group.AddedDamage",               "AddedEnergyDamage",                100, "Energy"),
        ("Group.AddedDamage",               "AddedChemicalDamage",              100, "Chemical"),
        ("Group.AddedDamage",               "AddedRadiationDamage",              75, "Radiation"),
        ("Group.AddedDamage",               "AddedExplosiveDamage",             100, "Explosive"),
        ("Group.IncreasedDamageType",       "IncreasedPhysicalDamage",          150, "Physical"),
        ("Group.IncreasedDamageType",       "IncreasedEnergyDamage",            150, "Energy"),
        ("Group.IncreasedDamageType",       "IncreasedChemicalDamage",          150, "Chemical"),
        ("Group.IncreasedDamageType",       "IncreasedRadiationDamage",         100, "Radiation"),
        ("Group.IncreasedDamageType",       "IncreasedExplosiveDamage",         150, "Explosive"),
        # Critical
        ("Group.CriticalStrikeChance",              "CriticalStrikeChance",     150, ""),
        ("Group.IncreasedCriticalStrikeChance",     "IncreasedCriticalStrikeChance", 150, ""),
        # Debuff effectiveness (ring amplifies debuff application)
        ("Group.DebuffEffectiveness",   "BruisedEffectiveness",                 100, "Physical"),
        ("Group.DebuffEffectiveness",   "ConcussedEffectiveness",               100, "Explosive"),
        ("Group.DebuffEffectiveness",   "IrradiatedEffectiveness",              100, "Radiation"),
        ("Group.DebuffEffectiveness",   "CorrodedEffectiveness",                100, "Chemical"),
        ("Group.DebuffEffectiveness",   "ThermalDestabilizationEffectiveness",  100, "Energy"),
        # Light defence
        ("Group.Resistance",        "PhysicalResistance",               100, ""),
        ("Group.Resistance",          "EnergyResistance",                 100, ""),
        ("Group.HealthRegeneration",        "HealthRegeneration",               100, ""),
    ],

    # ══════════════════════════════════════════════════════════════════════════
    # AMULET — broad mix, higher weight on unique combinations
    # ══════════════════════════════════════════════════════════════════════════

    "Amulet": [
        # Damage
        ("Group.AddedDamage",               "AddedPhysicalDamage",              100, "Physical"),
        ("Group.AddedDamage",               "AddedEnergyDamage",                100, "Energy"),
        ("Group.AddedDamage",               "AddedChemicalDamage",              100, "Chemical"),
        ("Group.AddedDamage",               "AddedExplosiveDamage",             100, "Explosive"),
        ("Group.IncreasedDamageType",       "IncreasedPhysicalDamage",          150, "Physical"),
        ("Group.IncreasedDamageType",       "IncreasedEnergyDamage",            150, "Energy"),
        ("Group.IncreasedDamageType",       "IncreasedChemicalDamage",          150, "Chemical"),
        ("Group.IncreasedDamageType",       "IncreasedExplosiveDamage",         150, "Explosive"),
        # Critical
        ("Group.CriticalStrikeChance",              "CriticalStrikeChance",     100, ""),
        ("Group.CriticalStrikeMultiplier",          "CriticalStrikeMultiplier", 100, ""),
        # Max resistances (amulet can push caps)
        ("Group.MaxResistance",     "MaxPhysicalResistance",            100, ""),
        ("Group.MaxResistance",       "MaxEnergyResistance",              100, ""),
        ("Group.MaxResistance",     "MaxChemicalResistance",            100, ""),
        ("Group.MaxResistance",    "MaxExplosiveResistance",           100, ""),
        # Resistances
        ("Group.Resistance",        "PhysicalResistance",               150, ""),
        ("Group.Resistance",          "EnergyResistance",                 150, ""),
        ("Group.Resistance",        "ChemicalResistance",               150, ""),
        # Health + heat
        ("Group.HealthRegeneration",        "HealthRegeneration",               150, ""),
        ("Group.CoolingCoefficient",        "CoolingCoefficient",               100, ""),
        ("Group.ThermalMass",               "ThermalMass",                      100, ""),
        # Debuff (amulet can amplify any debuff type)
        ("Group.DebuffEffectiveness",   "BruisedEffectiveness",                  75, "Physical"),
        ("Group.DebuffEffectiveness",   "ConcussedEffectiveness",                75, "Explosive"),
        ("Group.DebuffEffectiveness",   "IrradiatedEffectiveness",               75, "Radiation"),
        ("Group.DebuffEffectiveness",   "CorrodedEffectiveness",                 75, "Chemical"),
        ("Group.DebuffEffectiveness",   "ThermalDestabilizationEffectiveness",   75, "Energy"),
        ("Group.DebuffDuration",        "BruisedDuration",                       75, ""),
        ("Group.DebuffDuration",        "IrradiatedDuration",                    75, ""),
        ("Group.DebuffDuration",        "ThermalInsulation",                     75, ""),
    ],
}

SHEET_ORDER = [
    "Weapon_Ranged", "Weapon_Melee", "Weapon_Utility",
    "Headwear_Might", "Headwear_Agility",
    "BodyArmour_Might", "BodyArmour_Agility",
    "Gloves_Might", "Gloves_Agility",
    "Boots_Might", "Boots_Agility",
    "Belt",
    "Ring", "Amulet",
]


def style_header(ws):
    fill = PatternFill("solid", fgColor="1F3864")
    font = Font(bold=True, color="FFFFFF")
    for cell in ws[1]:
        cell.fill = fill
        cell.font = font
        cell.alignment = Alignment(horizontal="center")


def set_column_widths(ws):
    ws.column_dimensions["A"].width = 40  # GroupTag
    ws.column_dimensions["B"].width = 42  # AffixID
    ws.column_dimensions["C"].width = 14  # PoolWeight
    ws.column_dimensions["D"].width = 14  # AffinityTag


def main():
    if not os.path.exists(WORKBOOK_PATH):
        print(f"ERROR: Could not find workbook at {WORKBOOK_PATH}")
        return

    wb = openpyxl.load_workbook(WORKBOOK_PATH, keep_vba=True)
    existing_sheets = set(wb.sheetnames)

    created, skipped = 0, 0
    for name in SHEET_ORDER:
        if name in existing_sheets:
            print(f"  SKIP  {name}  (sheet already exists)")
            skipped += 1
            continue

        ws = wb.create_sheet(title=name)
        ws.append(HEADERS)
        style_header(ws)
        set_column_widths(ws)

        for row in POOLS[name]:
            ws.append(list(row))

        print(f"  CREATE {name}  ({len(POOLS[name])} rows)")
        created += 1

    wb.save(WORKBOOK_PATH)
    print(f"\nDone — created {created} sheets, skipped {skipped} existing.")


if __name__ == "__main__":
    main()
