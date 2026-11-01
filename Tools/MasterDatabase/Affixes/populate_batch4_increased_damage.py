"""
Batch 4 — Increased Damage affixes (percentage, Prefix, integer storage, DisplayAs=Percent)

Groups:
  Group.IncreasedDamageType   — 5 damage types, 8 tiers each
  Group.IncreasedWeaponDamage — 12 weapon / combat types, 8 tiers each
  Group.IncreasedAoEDamage    — 1 affix, 4 tiers (ilvl 52-82)

Ordering low→high (T1 max):
  Damage types:  Physical(28) < Energy(30) < Chemical(33) < Radiation(35) < Explosive(36)
  Weapon types:  Pistol(22) < Unarmed(27) < Sword(29) ≈ Melee(30) ≈ Projectile(30)
                 < Rifle(32) < Club(34) ≈ Axe(36) < Shotgun(40) < Grenade(44)
                 < Drone(48) < Launcher(54)
"""
import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

TIER_CONFIG_8 = [
    (8, 1,  50),
    (7, 10, 100),
    (6, 20, 200),
    (5, 32, 300),
    (4, 44, 300),
    (3, 57, 200),
    (2, 68, 100),
    (1, 80, 50),
]

TIER_CONFIG_AOE4 = [
    (4, 52, 100),
    (3, 62, 200),
    (2, 72, 200),
    (1, 82, 100),
]

AFFIXES = [

    # ── Increased Damage by Type ──────────────────────────────────────────────

    {
        "id":          "IncreasedPhysicalDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedDamageType",
        "display_tag": "Damage, Physical",
        "attr":        "Attributes.Damage.IncreasedPhysical",
        "format":      "+{1}% Increased Physical Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "STRIKING",     # T8
            "FORCEFUL",     # T7
            "CRUSHING",     # T6
            "POWERFUL",     # T5
            "PULVERIZING",  # T4
            "DEVASTATING",  # T3
            "OVERWHELMING", # T2
            "PHYSICAL",     # T1
        ],
        "ranges": [
            (3,  6,  1),  # T8
            (7,  10, 1),  # T7
            (11, 14, 1),  # T6
            (15, 17, 1),  # T5
            (18, 20, 1),  # T4
            (21, 23, 1),  # T3
            (24, 25, 1),  # T2
            (26, 28, 1),  # T1
        ],
    },
    {
        "id":          "IncreasedEnergyDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedDamageType",
        "display_tag": "Damage, Energy",
        "attr":        "Attributes.Damage.IncreasedEnergy",
        "format":      "+{1}% Increased Energy Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "WARMING",      # T8
            "HEATING",      # T7
            "SCORCHING",    # T6
            "THERMAL",      # T5
            "BURNING",      # T4
            "BLAZING",      # T3
            "INCINERATING", # T2
            "ENERGY",       # T1
        ],
        "ranges": [
            (3,  7,  1),  # T8
            (8,  11, 1),  # T7
            (12, 15, 1),  # T6
            (16, 19, 1),  # T5
            (20, 22, 1),  # T4
            (23, 25, 1),  # T3
            (26, 27, 1),  # T2
            (28, 30, 1),  # T1
        ],
    },
    {
        "id":          "IncreasedChemicalDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedDamageType",
        "display_tag": "Damage, Chemical",
        "attr":        "Attributes.Damage.IncreasedChemical",
        "format":      "+{1}% Increased Chemical Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "REACTIVE",   # T8
            "DISSOLVING", # T7
            "CORRODING",  # T6
            "CAUSTIC",    # T5
            "TOXIC",      # T4
            "VIRULENT",   # T3
            "LETHAL",     # T2
            "CHEMICAL",   # T1
        ],
        "ranges": [
            (4,  7,  1),  # T8
            (8,  12, 1),  # T7
            (13, 16, 1),  # T6
            (17, 20, 1),  # T5
            (21, 24, 1),  # T4
            (25, 27, 1),  # T3
            (28, 30, 1),  # T2
            (31, 33, 1),  # T1
        ],
    },
    {
        "id":          "IncreasedRadiationDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedDamageType",
        "display_tag": "Damage, Radiation",
        "attr":        "Attributes.Damage.IncreasedRadiation",
        "format":      "+{1}% Increased Radiation Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "GLOWING",       # T8
            "EMITTING",      # T7
            "IRRADIATING",   # T6
            "CONTAMINATING", # T5
            "DECAYING",      # T4
            "PENETRATING",   # T3
            "CRITICAL",      # T2
            "RADIANT",       # T1
        ],
        "ranges": [
            (4,  8,  1),  # T8
            (9,  12, 1),  # T7
            (13, 17, 1),  # T6
            (18, 21, 1),  # T5
            (22, 25, 1),  # T4
            (26, 29, 1),  # T3
            (30, 32, 1),  # T2
            (33, 35, 1),  # T1
        ],
    },
    {
        "id":          "IncreasedExplosiveDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedDamageType",
        "display_tag": "Damage, Explosive",
        "attr":        "Attributes.Damage.IncreasedExplosive",
        "format":      "+{1}% Increased Explosive Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "CHARGED",     # T8
            "DETONATING",  # T7
            "BLASTING",    # T6
            "CONCUSSIVE",  # T5
            "SHATTERING",  # T4
            "FRAGMENTING", # T3
            "CATACLYSMIC", # T2
            "EXPLOSIVE",   # T1
        ],
        "ranges": [
            (5,  8,  1),  # T8
            (9,  13, 1),  # T7
            (14, 17, 1),  # T6
            (18, 22, 1),  # T5
            (23, 26, 1),  # T4
            (27, 30, 1),  # T3
            (31, 33, 1),  # T2
            (34, 36, 1),  # T1
        ],
    },

    # ── Increased Damage by Weapon / Combat Type ──────────────────────────────

    {
        "id":          "IncreasedPistolDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedWeaponDamage",
        "display_tag": "Damage, Pistol",
        "attr":        "Attributes.Damage.IncreasedPistol",
        "format":      "+{1}% Increased Pistol Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "QUICKDRAW",  # T8
            "PRACTICED",  # T7
            "ACCURATE",   # T6
            "GUNFIGHTER", # T5
            "DEADEYE",    # T4
            "GUNSLINGER", # T3
            "MARKSMAN",   # T2
            "PISTOLEER",  # T1
        ],
        "ranges": [
            (2,  4,  1),  # T8
            (5,  7,  1),  # T7
            (8,  10, 1),  # T6
            (11, 13, 1),  # T5
            (14, 16, 1),  # T4
            (17, 18, 1),  # T3
            (19, 20, 1),  # T2
            (21, 22, 1),  # T1
        ],
    },
    {
        "id":          "IncreasedUnarmedDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedWeaponDamage",
        "display_tag": "Damage, Unarmed",
        "attr":        "Attributes.Damage.IncreasedUnarmed",
        "format":      "+{1}% Increased Unarmed Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "BRAWLER",       # T8
            "PUGILIST",      # T7
            "SCRAPPER",      # T6
            "FIGHTER",       # T5
            "BRUISER",       # T4
            "COMBATANT",     # T3
            "MARTIAL",       # T2
            "STREETFIGHTER", # T1
        ],
        "ranges": [
            (2,  5,  1),  # T8
            (6,  8,  1),  # T7
            (9,  12, 1),  # T6
            (13, 15, 1),  # T5
            (16, 18, 1),  # T4
            (19, 21, 1),  # T3
            (22, 24, 1),  # T2
            (25, 27, 1),  # T1
        ],
    },
    {
        "id":          "IncreasedSwordDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedWeaponDamage",
        "display_tag": "Damage, Sword",
        "attr":        "Attributes.Damage.IncreasedSword",
        "format":      "+{1}% Increased Sword Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "CUTTING",     # T8
            "CLEAVING",    # T7
            "SLASHING",    # T6
            "PIERCING",    # T5
            "DUELIST",     # T4
            "FENCER",      # T3
            "BLADEMASTER", # T2
            "SWORDMASTER", # T1
        ],
        "ranges": [
            (3,  5,  1),  # T8
            (7,  9,  1),  # T7
            (10, 13, 1),  # T6
            (14, 17, 1),  # T5
            (18, 21, 1),  # T4
            (22, 23, 1),  # T3
            (25, 26, 1),  # T2
            (27, 29, 1),  # T1
        ],
    },
    {
        "id":          "IncreasedClubDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedWeaponDamage",
        "display_tag": "Damage, Club",
        "attr":        "Attributes.Damage.IncreasedClub",
        "format":      "+{1}% Increased Club Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "BLUDGEONING", # T8
            "BATTERING",   # T7
            "POUNDING",    # T6
            "SMASHING",    # T5
            "HAMMERING",   # T4
            "CRUSHING",    # T3
            "PULVERIZING", # T2
            "CLUBMASTER",  # T1
        ],
        "ranges": [
            (3,  6,  1),  # T8
            (8,  11, 1),  # T7
            (12, 16, 1),  # T6
            (17, 20, 1),  # T5
            (22, 25, 1),  # T4
            (26, 28, 1),  # T3
            (30, 31, 1),  # T2
            (32, 34, 1),  # T1
        ],
    },
    {
        "id":          "IncreasedRifleDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedWeaponDamage",
        "display_tag": "Damage, Rifle",
        "attr":        "Attributes.Damage.IncreasedRifle",
        "format":      "+{1}% Increased Rifle Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "AIMED",        # T8
            "FOCUSED",      # T7
            "SIGHTED",      # T6
            "SHARPSHOOTER", # T5
            "SNIPER",       # T4
            "RIFLEMAN",     # T3
            "DEADSHOT",     # T2
            "DEADEYE",      # T1
        ],
        "ranges": [
            (3,  6,  1),  # T8
            (7,  10, 1),  # T7
            (12, 15, 1),  # T6
            (16, 19, 1),  # T5
            (20, 23, 1),  # T4
            (25, 26, 1),  # T3
            (28, 29, 1),  # T2
            (30, 32, 1),  # T1
        ],
    },
    {
        "id":          "IncreasedAxeDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedWeaponDamage",
        "display_tag": "Damage, Axe",
        "attr":        "Attributes.Damage.IncreasedAxe",
        "format":      "+{1}% Increased Axe Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "HACKING",   # T8
            "CHOPPING",  # T7
            "CLEAVING",  # T6
            "SEVERING",  # T5
            "RENDING",   # T4
            "SPLITTING", # T3
            "SUNDERING", # T2
            "AXEMASTER", # T1
        ],
        "ranges": [
            (3,  7,  1),  # T8
            (8,  12, 1),  # T7
            (13, 17, 1),  # T6
            (18, 21, 1),  # T5
            (23, 26, 1),  # T4
            (28, 30, 1),  # T3
            (31, 33, 1),  # T2
            (34, 36, 1),  # T1
        ],
    },
    {
        "id":          "IncreasedShotgunDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedWeaponDamage",
        "display_tag": "Damage, Shotgun",
        "attr":        "Attributes.Damage.IncreasedShotgun",
        "format":      "+{1}% Increased Shotgun Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "BUCKSHOT",    # T8
            "BREACHER",    # T7
            "SCATTERSHOT", # T6
            "SHREDDER",    # T5
            "SPREADER",    # T4
            "DEVASTATOR",  # T3
            "OBLITERATOR", # T2
            "SHOTGUNNER",  # T1
        ],
        "ranges": [
            (4,  7,  1),  # T8
            (9,  13, 1),  # T7
            (14, 18, 1),  # T6
            (19, 23, 1),  # T5
            (25, 29, 1),  # T4
            (30, 32, 1),  # T3
            (34, 36, 1),  # T2
            (37, 40, 1),  # T1
        ],
    },
    {
        "id":          "IncreasedGrenadeDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedWeaponDamage",
        "display_tag": "Damage, Grenade",
        "attr":        "Attributes.Damage.IncreasedGrenade",
        "format":      "+{1}% Increased Grenade Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "FRAGMENTED",    # T8
            "CONCUSSIVE",    # T7
            "INCENDIARY",    # T6
            "ANTIPERSONNEL", # T5
            "DEMOLITIONS",   # T4
            "ORDNANCE",      # T3
            "DEMOLITIONIST", # T2
            "GRENADIER",     # T1
        ],
        "ranges": [
            (4,  8,  1),  # T8
            (10, 14, 1),  # T7
            (15, 20, 1),  # T6
            (21, 26, 1),  # T5
            (28, 32, 1),  # T4
            (33, 36, 1),  # T3
            (38, 40, 1),  # T2
            (42, 44, 1),  # T1
        ],
    },
    {
        "id":          "IncreasedDroneDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedWeaponDamage",
        "display_tag": "Damage, Drone",
        "attr":        "Attributes.Damage.IncreasedDrone",
        "format":      "+{1}% Increased Drone Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "AUTOMATED",    # T8
            "NETWORKED",    # T7
            "COORDINATED",  # T6
            "SYNCHRONIZED", # T5
            "TACTICAL",     # T4
            "STRATEGIC",    # T3
            "ORCHESTRATED", # T2
            "COMMANDER",    # T1
        ],
        "ranges": [
            (4,  9,  1),  # T8
            (11, 15, 1),  # T7
            (17, 22, 1),  # T6
            (24, 28, 1),  # T5
            (31, 35, 1),  # T4
            (37, 39, 1),  # T3
            (42, 44, 1),  # T2
            (46, 48, 1),  # T1
        ],
    },
    {
        "id":          "IncreasedLauncherDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedWeaponDamage",
        "display_tag": "Damage, Launcher",
        "attr":        "Attributes.Damage.IncreasedLauncher",
        "format":      "+{1}% Increased Launcher Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "BALLISTIC",  # T8
            "LOADED",     # T7
            "OVERCHARGED",# T6
            "HEAVY",      # T5
            "ARTILLERY",  # T4
            "BOMBARDIER", # T3
            "ANNIHILATOR",# T2
            "LAUNCHER",   # T1
        ],
        "ranges": [
            (5,  10, 1),  # T8
            (12, 17, 1),  # T7
            (19, 25, 1),  # T6
            (27, 32, 1),  # T5
            (34, 38, 1),  # T4
            (41, 44, 1),  # T3
            (46, 49, 1),  # T2
            (51, 54, 1),  # T1
        ],
    },
    {
        "id":          "IncreasedMeleeDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedWeaponDamage",
        "display_tag": "Damage, Melee",
        "attr":        "Attributes.Damage.IncreasedMelee",
        "format":      "+{1}% Increased Melee Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "BRAWLING",   # T8
            "AGGRESSIVE", # T7
            "RELENTLESS", # T6
            "FEROCIOUS",  # T5
            "SAVAGE",     # T4
            "BERSERKER",  # T3
            "FRENZIED",   # T2
            "MELEE",      # T1
        ],
        "ranges": [
            (3,  5,  1),  # T8
            (7,  9,  1),  # T7
            (11, 14, 1),  # T6
            (15, 18, 1),  # T5
            (19, 22, 1),  # T4
            (23, 24, 1),  # T3
            (26, 27, 1),  # T2
            (28, 30, 1),  # T1
        ],
    },
    {
        "id":          "IncreasedProjectileDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedWeaponDamage",
        "display_tag": "Damage, Projectile",
        "attr":        "Attributes.Damage.IncreasedProjectile",
        "format":      "+{1}% Increased Projectile Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "TARGETED",   # T8
            "AIMED",      # T7
            "RANGED",     # T6
            "BALLISTIC",  # T5
            "TACTICAL",   # T4
            "PRECISE",    # T3
            "DEADLY",     # T2
            "PROJECTILE", # T1
        ],
        "ranges": [
            (3,  5,  1),  # T8
            (7,  9,  1),  # T7
            (11, 13, 1),  # T6
            (15, 18, 1),  # T5
            (19, 22, 1),  # T4
            (23, 24, 1),  # T3
            (26, 27, 1),  # T2
            (28, 30, 1),  # T1
        ],
    },

    # ── Increased AoE Damage ──────────────────────────────────────────────────

    {
        "id":          "IncreasedAoEDamage",
        "type":        "Prefix",
        "group":       "Group.IncreasedAoEDamage",
        "display_tag": "Damage, AoE",
        "attr":        "Attributes.Damage.IncreasedAoE",
        "format":      "+{1}% Increased Area Damage",
        "tier_config": TIER_CONFIG_AOE4,
        "names": [
            "WIDE",        # T4
            "SWEEPING",    # T3
            "DEVASTATING", # T2
            "CATACLYSMIC", # T1
        ],
        "ranges": [
            (8,  13, 1),  # T4
            (14, 19, 1),  # T3
            (20, 25, 1),  # T2
            (26, 32, 1),  # T1
        ],
    },
]


def build_rows():
    rows = []
    for affix in AFFIXES:
        for i, (tier, ilvl, weight) in enumerate(affix["tier_config"]):
            min_val, max_val, step = affix["ranges"][i]
            rows.append([
                f"{affix['id']}_T{tier}",  # RowName
                affix["id"],               # AffixID
                affix["names"][i],         # ItemName
                affix["type"],             # Type
                affix["group"],            # GroupTag
                affix["display_tag"],      # DisplayTag
                tier,                      # Tier
                ilvl,                      # RequiredItemLevel
                weight,                    # Weight
                affix["attr"],             # Stat1_Attr
                min_val,                   # Stat1_Min
                max_val,                   # Stat1_Max
                step,                      # Stat1_Step
                0,                         # Stat1_DecimalCount
                "Percent",                 # Stat1_DisplayAs
                "",                        # Stat2_Attr
                "",                        # Stat2_Min
                "",                        # Stat2_Max
                "",                        # Stat2_Step
                "",                        # Stat2_DecimalCount
                "",                        # Stat2_DisplayAs
                affix["format"],           # DisplayFormat
            ])
    return rows


def main():
    if not os.path.exists(WORKBOOK_PATH):
        print(f"ERROR: Could not find workbook at {WORKBOOK_PATH}")
        return

    wb = openpyxl.load_workbook(WORKBOOK_PATH, keep_vba=True)
    ws = wb.active

    existing = set()
    for row in ws.iter_rows(min_row=2, values_only=True):
        if row[0]:
            existing.add(row[0])

    rows = build_rows()
    added, skipped = 0, 0
    for row in rows:
        if row[0] in existing:
            skipped += 1
        else:
            ws.append(row)
            added += 1

    wb.save(WORKBOOK_PATH)
    print(f"Done — added {added} rows, skipped {skipped} already-existing rows.")
    print(f"  Damage types: 5×8=40 | Weapon types: 12×8=96 | AoE: 1×4=4  (total 140)")


if __name__ == "__main__":
    main()
