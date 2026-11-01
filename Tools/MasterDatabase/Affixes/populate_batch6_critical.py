"""
Batch 6 — Critical Strike affixes (all Prefix, non-exclusive — each has its own group)

  Group.CriticalStrikeChance          — base crit %, 5 tiers, ilvl 45-80, single value per tier (1-5%)
  Group.IncreasedCriticalStrikeChance — increased crit %, 8 tiers, ilvl 10-75, max 50%
  Group.CriticalStrikeMultiplier      — crit multiplier bonus, 8 tiers, ilvl 15-78
                                        single value per tier (0.1-0.8), 1 decimal, +Xx display
                                        max value = 1.8x total (base 1.0 + 0.8 stored)
"""
import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

# Base crit chance: 5 single-value tiers (1-5%), ilvl 45-80
# Weighted toward mid tiers; T1 (5%) is rarer
TIER_CONFIG_CRIT5 = [
    (5, 45, 100),
    (4, 52, 200),
    (3, 60, 350),
    (2, 70, 200),
    (1, 80, 100),
]

# Increased crit chance: 8 tiers, ilvl 10-75
TIER_CONFIG_ICSC8 = [
    (8, 10,  50),
    (7, 19, 100),
    (6, 28, 150),
    (5, 38, 200),
    (4, 47, 300),
    (3, 56, 200),
    (2, 65, 150),
    (1, 75, 100),
]

# Crit multiplier: 8 single-value tiers (0.1-0.8), ilvl 15-78
TIER_CONFIG_CSM8 = [
    (8, 15,  50),
    (7, 25, 100),
    (6, 35, 150),
    (5, 44, 200),
    (4, 53, 300),
    (3, 62, 200),
    (2, 70, 150),
    (1, 78, 100),
]

AFFIXES = [

    # ── Base Critical Strike Chance ───────────────────────────────────────────
    # Single value per tier (1%, 2%, ... 5%). Stored as integer, DisplayAs=Percent.
    {
        "id":          "CriticalStrikeChance",
        "type":        "Prefix",
        "group":       "Group.CriticalStrikeChance",
        "display_tag": "Critical, Chance",
        "attr":        "Attributes.Critical.Chance",
        "display_as":  "Percent",
        "decimal":     0,
        "format":      "+{1}% Critical Strike Chance",
        "tier_config": TIER_CONFIG_CRIT5,
        "names": [
            "LUCKY",    # T5 — 1%
            "SHARP",    # T4 — 2%
            "FOCUSED",  # T3 — 3%
            "PINPOINT", # T2 — 4%
            "PRECISE",  # T1 — 5%
        ],
        "ranges": [
            (1, 1, 1),  # T5
            (2, 2, 1),  # T4
            (3, 3, 1),  # T3
            (4, 4, 1),  # T2
            (5, 5, 1),  # T1
        ],
    },

    # ── Increased Critical Strike Chance ─────────────────────────────────────
    {
        "id":          "IncreasedCriticalStrikeChance",
        "type":        "Prefix",
        "group":       "Group.IncreasedCriticalStrikeChance",
        "display_tag": "Critical, Chance",
        "attr":        "Attributes.Critical.IncreasedChance",
        "display_as":  "Percent",
        "decimal":     0,
        "format":      "+{1}% Increased Critical Strike Chance",
        "tier_config": TIER_CONFIG_ICSC8,
        "names": [
            "OPPORTUNISTIC", # T8
            "CALCULATING",   # T7
            "PREDATORY",     # T6
            "EXACTING",      # T5
            "HUNTING",       # T4
            "ASSASSIN",      # T3
            "EXECUTIONER",   # T2
            "ANNIHILATING",  # T1
        ],
        "ranges": [
            (6,  10, 1),  # T8
            (11, 16, 1),  # T7
            (17, 22, 1),  # T6
            (23, 28, 1),  # T5
            (29, 34, 1),  # T4
            (35, 39, 1),  # T3
            (40, 44, 1),  # T2
            (45, 50, 1),  # T1
        ],
    },

    # ── Critical Strike Multiplier ────────────────────────────────────────────
    # Single value per tier (0.1-0.8). Stored as float, 1 decimal.
    # Total crit damage = base 1.0x + stored value. Max total = 1.8x.
    # Display: "+0.8x Critical Strike Multiplier"
    {
        "id":          "CriticalStrikeMultiplier",
        "type":        "Prefix",
        "group":       "Group.CriticalStrikeMultiplier",
        "display_tag": "Critical, Multiplier",
        "attr":        "Attributes.Critical.Multiplier",
        "display_as":  "Flat",
        "decimal":     1,
        "format":      "+{1}x Critical Strike Multiplier",
        "tier_config": TIER_CONFIG_CSM8,
        "names": [
            "STINGING",     # T8 — +0.1x
            "IMPACTFUL",    # T7 — +0.2x
            "WOUNDING",     # T6 — +0.3x
            "PUNISHING",    # T5 — +0.4x
            "DEVASTATING",  # T4 — +0.5x
            "CRIPPLING",    # T3 — +0.6x
            "CRUSHING",     # T2 — +0.7x
            "CATASTROPHIC", # T1 — +0.8x
        ],
        "ranges": [
            (0.1, 0.1, 0.1),  # T8
            (0.2, 0.2, 0.1),  # T7
            (0.3, 0.3, 0.1),  # T6
            (0.4, 0.4, 0.1),  # T5
            (0.5, 0.5, 0.1),  # T4
            (0.6, 0.6, 0.1),  # T3
            (0.7, 0.7, 0.1),  # T2
            (0.8, 0.8, 0.1),  # T1
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
    print(f"  CriticalStrikeChance:          5 rows  (1-5%, single value per tier)")
    print(f"  IncreasedCriticalStrikeChance: 8 rows  (6-50%)")
    print(f"  CriticalStrikeMultiplier:      8 rows  (+0.1x to +0.8x, single value per tier)")
    print(f"  Total: 21 rows")


if __name__ == "__main__":
    main()
