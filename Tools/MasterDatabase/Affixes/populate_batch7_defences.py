"""
Batch 7 — Defence affixes (all Prefix, non-exclusive — each has its own group)

  Group.Armour                      flat,    8T  ilvl 1-80    max 1500
  Group.IncreasedArmour             %,       8T  ilvl 6-82    max 110%
  Group.ChanceToDodge               %,       8T  ilvl 8-80    max 12%
  Group.BlockChance                 %,       6T  ilvl 15-80   max 15%
  Group.HealthRegeneration          %,2dec,  8T  ilvl 1-80    max 2.50%  step 0.10
  Group.CriticalHitResistance       %,       6T  ilvl 35-85   max 15%
  Group.DeferredDamageAmount        %,1dec,  4T  ilvl 40-80   max 8.0%   step 0.5
  Group.DeferredDamageTime          sec,1dec,4T  ilvl 40-80   max 1.0s   step 0.1
  Group.ReducedDamageFromProjectiles%,       3T  ilvl 55-85   max 10%
  Group.ReducedDamageFromAoE        %,       3T  ilvl 55-85   max 10%
  Group.ReducedDamageFromMelee      %,       3T  ilvl 55-85   max 10%
"""
import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

TIER_8_1_80 = [
    (8, 1,   50), (7, 11, 100), (6, 21, 150), (5, 32, 200),
    (4, 44, 300), (3, 56, 200), (2, 68, 150), (1, 80, 100),
]
TIER_8_6_82 = [
    (8, 6,   50), (7, 17, 100), (6, 28, 150), (5, 39, 200),
    (4, 51, 300), (3, 62, 200), (2, 72, 150), (1, 82, 100),
]
TIER_8_8_80 = [
    (8, 8,   50), (7, 18, 100), (6, 28, 150), (5, 38, 200),
    (4, 49, 300), (3, 60, 200), (2, 70, 150), (1, 80, 100),
]
TIER_6_15_80 = [
    (6, 15,  50), (5, 28, 100), (4, 42, 200),
    (3, 56, 300), (2, 68, 200), (1, 80, 100),
]
TIER_6_35_85 = [
    (6, 35,  50), (5, 47, 100), (4, 57, 200),
    (3, 67, 300), (2, 76, 200), (1, 85, 100),
]
TIER_4_40_80 = [
    (4, 40, 100), (3, 53, 200), (2, 67, 200), (1, 80, 100),
]
TIER_3_55_85 = [
    (3, 55, 100), (2, 70, 200), (1, 85, 100),
]

AFFIXES = [

    # ── Armour ────────────────────────────────────────────────────────────────
    {
        "id":          "Armour",
        "type":        "Prefix",
        "group":       "Group.Armour",
        "display_tag": "Defence, Armour",
        "attr":        "Attributes.Defence.Armour",
        "display_as":  "Flat",
        "decimal":     0,
        "step":        1,
        "format":      "+{1} Armour",
        "tier_config": TIER_8_1_80,
        "names": [
            "SCUFFED",    # T8
            "PADDED",     # T7
            "COATED",     # T6
            "LAYERED",    # T5
            "REINFORCED", # T4
            "PLATED",     # T3
            "FORTIFIED",  # T2
            "ARMORED",    # T1
        ],
        "ranges": [
            (1,    100,  1),  # T8
            (101,  250,  1),  # T7
            (251,  450,  1),  # T6
            (451,  700,  1),  # T5
            (701,  950,  1),  # T4
            (951,  1150, 1),  # T3
            (1151, 1300, 1),  # T2
            (1301, 1500, 1),  # T1
        ],
    },

    # ── Increased Armour ──────────────────────────────────────────────────────
    {
        "id":          "IncreasedArmour",
        "type":        "Prefix",
        "group":       "Group.IncreasedArmour",
        "display_tag": "Defence, Armour",
        "attr":        "Attributes.Defence.IncreasedArmour",
        "display_as":  "Percent",
        "decimal":     0,
        "step":        1,
        "format":      "+{1}% Increased Armour",
        "tier_config": TIER_8_6_82,
        "names": [
            "TOUGHENING",    # T8
            "STRENGTHENING", # T7
            "BOLSTERING",    # T6
            "FORTIFYING",    # T5
            "HARDENING",     # T4
            "TEMPERING",     # T3
            "IRONING",       # T2
            "STEELING",      # T1
        ],
        "ranges": [
            (8,   15,  1),  # T8
            (16,  25,  1),  # T7
            (26,  40,  1),  # T6
            (41,  55,  1),  # T5
            (56,  70,  1),  # T4
            (71,  85,  1),  # T3
            (86,  98,  1),  # T2
            (99,  110, 1),  # T1
        ],
    },

    # ── Chance to Dodge ───────────────────────────────────────────────────────
    {
        "id":          "ChanceToDodge",
        "type":        "Prefix",
        "group":       "Group.ChanceToDodge",
        "display_tag": "Defence, Dodge",
        "attr":        "Attributes.Defence.ChanceToDodge",
        "display_as":  "Percent",
        "decimal":     0,
        "step":        1,
        "format":      "+{1}% Chance to Dodge",
        "tier_config": TIER_8_8_80,
        "names": [
            "ALERT",        # T8
            "QUICK",        # T7
            "NIMBLE",       # T6
            "EVASIVE",      # T5
            "ELUSIVE",      # T4
            "SLIPPERY",     # T3
            "UNTOUCHABLE",  # T2
            "GHOST",        # T1
        ],
        "ranges": [
            (1,  2,  1),  # T8
            (3,  4,  1),  # T7
            (5,  6,  1),  # T6
            (7,  8,  1),  # T5
            (9,  9,  1),  # T4  — single value
            (10, 10, 1),  # T3
            (11, 11, 1),  # T2
            (12, 12, 1),  # T1
        ],
    },

    # ── Block Chance ──────────────────────────────────────────────────────────
    {
        "id":          "BlockChance",
        "type":        "Prefix",
        "group":       "Group.BlockChance",
        "display_tag": "Defence, Block",
        "attr":        "Attributes.Defence.BlockChance",
        "display_as":  "Percent",
        "decimal":     0,
        "step":        1,
        "format":      "+{1}% Block Chance",
        "tier_config": TIER_6_15_80,
        "names": [
            "WARDING",       # T6
            "PARRYING",      # T5
            "DEFLECTING",    # T4
            "BLOCKING",      # T3
            "GUARDING",      # T2
            "IMPENETRABLE",  # T1
        ],
        "ranges": [
            (1,  3,  1),  # T6
            (4,  6,  1),  # T5
            (7,  9,  1),  # T4
            (10, 11, 1),  # T3
            (12, 13, 1),  # T2
            (14, 15, 1),  # T1
        ],
    },

    # ── Health Regeneration ───────────────────────────────────────────────────
    # Step 0.10 — necessary for 2-decimal % values in the 0.10-2.50 range.
    {
        "id":          "HealthRegeneration",
        "type":        "Prefix",
        "group":       "Group.HealthRegeneration",
        "display_tag": "Defence, Health",
        "attr":        "Attributes.Defence.HealthRegeneration",
        "display_as":  "Percent",
        "decimal":     2,
        "step":        0.10,
        "format":      "+{1}% Health Regeneration",
        "tier_config": TIER_8_1_80,
        "names": [
            "RECOVERING",    # T8
            "HEALING",       # T7
            "MENDING",       # T6
            "RESTORING",     # T5
            "REGENERATING",  # T4
            "REVITALIZING",  # T3
            "REJUVENATING",  # T2
            "RENEWING",      # T1
        ],
        "ranges": [
            (0.10, 0.30, 0.10),  # T8
            (0.40, 0.60, 0.10),  # T7
            (0.70, 0.90, 0.10),  # T6
            (1.00, 1.20, 0.10),  # T5
            (1.30, 1.60, 0.10),  # T4
            (1.70, 1.90, 0.10),  # T3
            (2.00, 2.20, 0.10),  # T2
            (2.30, 2.50, 0.10),  # T1
        ],
    },

    # ── Critical Hit Resistance ───────────────────────────────────────────────
    {
        "id":          "CriticalHitResistance",
        "type":        "Prefix",
        "group":       "Group.CriticalHitResistance",
        "display_tag": "Defence, Critical",
        "attr":        "Attributes.Defence.CriticalHitResistance",
        "display_as":  "Percent",
        "decimal":     0,
        "step":        1,
        "format":      "+{1}% Critical Hit Resistance",
        "tier_config": TIER_6_35_85,
        "names": [
            "BRACED",       # T6
            "COMPOSED",     # T5
            "STEADY",       # T4
            "STOIC",        # T3
            "UNSHAKEN",     # T2
            "UNBREAKABLE",  # T1
        ],
        "ranges": [
            (1,  3,  1),  # T6
            (4,  6,  1),  # T5
            (7,  9,  1),  # T4
            (10, 11, 1),  # T3
            (12, 13, 1),  # T2
            (14, 15, 1),  # T1
        ],
    },

    # ── Deferred Damage Amount ────────────────────────────────────────────────
    # Step 0.5 — 1-decimal % values in the 1.0-8.0 range.
    {
        "id":          "DeferredDamageAmount",
        "type":        "Prefix",
        "group":       "Group.DeferredDamageAmount",
        "display_tag": "Defence, Deferred",
        "attr":        "Attributes.Defence.DeferredDamageAmount",
        "display_as":  "Percent",
        "decimal":     1,
        "step":        0.5,
        "format":      "+{1}% Deferred Damage",
        "tier_config": TIER_4_40_80,
        "names": [
            "ABSORBING",     # T4
            "DEFLECTING",    # T3
            "REDISTRIBUTING",# T2
            "DISPERSING",    # T1
        ],
        "ranges": [
            (1.0, 2.5, 0.5),  # T4
            (3.0, 4.5, 0.5),  # T3
            (5.0, 6.5, 0.5),  # T2
            (7.0, 8.0, 0.5),  # T1
        ],
    },

    # ── Deferred Damage Duration ──────────────────────────────────────────────
    # Step 0.1 — duration in seconds, 1 decimal, max 1.0s.
    {
        "id":          "DeferredDamageTime",
        "type":        "Prefix",
        "group":       "Group.DeferredDamageTime",
        "display_tag": "Defence, Deferred",
        "attr":        "Attributes.Defence.DeferredDamageTime",
        "display_as":  "Flat",
        "decimal":     1,
        "step":        0.1,
        "format":      "+{1}s Deferred Damage Duration",
        "tier_config": TIER_4_40_80,
        "names": [
            "DELAYING",    # T4
            "POSTPONING",  # T3
            "DEFERRING",   # T2
            "SUSPENDING",  # T1
        ],
        "ranges": [
            (0.2, 0.3, 0.1),  # T4
            (0.4, 0.6, 0.1),  # T3
            (0.7, 0.9, 0.1),  # T2
            (1.0, 1.0, 0.1),  # T1
        ],
    },

    # ── Reduced Damage Taken by Source ────────────────────────────────────────
    {
        "id":          "ReducedDamageFromProjectiles",
        "type":        "Prefix",
        "group":       "Group.ReducedDamageFromProjectiles",
        "display_tag": "Defence, Projectile",
        "attr":        "Attributes.Defence.ReducedDamageFromProjectiles",
        "display_as":  "Percent",
        "decimal":     0,
        "step":        1,
        "format":      "+{1}% Reduced Damage from Projectiles",
        "tier_config": TIER_3_55_85,
        "names": ["DEFLECTING", "EVADING", "UNTOUCHABLE"],
        "ranges": [(1, 4, 1), (5, 7, 1), (8, 10, 1)],
    },
    {
        "id":          "ReducedDamageFromAoE",
        "type":        "Prefix",
        "group":       "Group.ReducedDamageFromAoE",
        "display_tag": "Defence, AoE",
        "attr":        "Attributes.Defence.ReducedDamageFromAoE",
        "display_as":  "Percent",
        "decimal":     0,
        "step":        1,
        "format":      "+{1}% Reduced Damage from Area Effects",
        "tier_config": TIER_3_55_85,
        "names": ["DISPERSED", "SCATTERED", "INSULATED"],
        "ranges": [(1, 4, 1), (5, 7, 1), (8, 10, 1)],
    },
    {
        "id":          "ReducedDamageFromMelee",
        "type":        "Prefix",
        "group":       "Group.ReducedDamageFromMelee",
        "display_tag": "Defence, Melee",
        "attr":        "Attributes.Defence.ReducedDamageFromMelee",
        "display_as":  "Percent",
        "decimal":     0,
        "step":        1,
        "format":      "+{1}% Reduced Damage from Melee",
        "tier_config": TIER_3_55_85,
        "names": ["STALWART", "UNYIELDING", "IRONCLAD"],
        "ranges": [(1, 4, 1), (5, 7, 1), (8, 10, 1)],
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
                affix["decimal"],          # Stat1_DecimalCount
                affix["display_as"],       # Stat1_DisplayAs
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
    print(f"  Armour / IncreasedArmour:     8+8   = 16")
    print(f"  Dodge / Block:                8+6   = 14")
    print(f"  HealthRegen / CritHitRes:     8+6   = 14")
    print(f"  DeferredAmount / Time:        4+4   =  8")
    print(f"  ReducedDmg ×3:                3×3   =  9")
    print(f"  Total expected:                       61 rows")


if __name__ == "__main__":
    main()
