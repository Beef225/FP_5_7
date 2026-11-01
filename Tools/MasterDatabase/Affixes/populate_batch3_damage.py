import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

TIER_CONFIG_7 = [
    (7, 1,  50),
    (6, 12, 100),
    (5, 24, 200),
    (4, 36, 300),
    (3, 50, 200),
    (2, 65, 100),
    (1, 80, 50),
]

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

TIER_CONFIG_9 = [
    (9, 1,  50),
    (8, 9,  100),
    (7, 18, 150),
    (6, 28, 200),
    (5, 37, 300),
    (4, 47, 300),
    (3, 57, 200),
    (2, 68, 100),
    (1, 80, 50),
]

TIER_CONFIG_10 = [
    (10, 1,  50),
    (9,  9,  100),
    (8,  18, 150),
    (7,  27, 200),
    (6,  36, 300),
    (5,  46, 300),
    (4,  55, 200),
    (3,  64, 150),
    (2,  72, 100),
    (1,  80, 50),
]

TIER_CONFIG_12 = [
    (12, 1,  50),
    (11, 8,  75),
    (10, 15, 100),
    (9,  22, 125),
    (8,  30, 175),
    (7,  37, 250),
    (6,  44, 250),
    (5,  52, 175),
    (4,  59, 125),
    (3,  66, 100),
    (2,  73, 75),
    (1,  80, 50),
]

AFFIXES = [
    {
        "id":          "AddedPhysicalDamage",
        "type":        "Prefix",
        "group":       "Group.AddedDamage",
        "display_tag": "Damage, Physical",
        "attr":        "Attributes.Damage.AddedPhysical",
        "format":      "+{1} Added Physical Damage",
        "tier_config": TIER_CONFIG_7,
        "names": [
            "SCRATCHED",  # T7
            "ROUGH",      # T6
            "JAGGED",     # T5
            "SERRATED",   # T4
            "KEEN",       # T3
            "BRUTAL",     # T2
            "PHYSICAL",   # T1
        ],
        "ranges": [
            (1,  4,  1),  # T7
            (4,  9,  1),  # T6
            (9,  15, 1),  # T5
            (15, 22, 1),  # T4
            (22, 30, 2),  # T3
            (30, 36, 2),  # T2
            (36, 42, 2),  # T1
        ],
    },
    {
        "id":          "AddedEnergyDamage",
        "type":        "Prefix",
        "group":       "Group.AddedDamage",
        "display_tag": "Damage, Energy",
        "attr":        "Attributes.Damage.AddedEnergy",
        "format":      "+{1} Added Energy Damage",
        "tier_config": TIER_CONFIG_9,
        "names": [
            "TEPID",         # T9
            "THERMAL",       # T8
            "SCALDING",      # T7
            "CALORIC",       # T6
            "ENTROPIC",      # T5
            "CONVECTIVE",    # T4
            "EXOTHERMIC",    # T3
            "INCANDESCENT",  # T2
            "ENERGY",        # T1
        ],
        "ranges": [
            (1,  4,  1),  # T9
            (4,  9,  1),  # T8
            (9,  15, 1),  # T7
            (15, 21, 2),  # T6
            (21, 28, 1),  # T5
            (28, 34, 2),  # T4
            (34, 39, 1),  # T3
            (39, 43, 2),  # T2
            (43, 46, 1),  # T1
        ],
    },
    {
        "id":          "AddedChemicalDamage",
        "type":        "Prefix",
        "group":       "Group.AddedDamage",
        "display_tag": "Damage, Chemical",
        "attr":        "Attributes.Damage.AddedChemical",
        "format":      "+{1} Added Chemical Damage",
        "tier_config": TIER_CONFIG_8,
        "names": [
            "LEAKING",      # T8
            "GALVANIC",     # T7
            "REACTIVE",     # T6
            "ELECTROLYTIC", # T5
            "CAUSTIC",      # T4
            "CORROSIVE",    # T3
            "VOLATILE",     # T2
            "CHEMICAL",     # T1
        ],
        "ranges": [
            (1,  5,  1),  # T8
            (5,  10, 1),  # T7
            (10, 17, 1),  # T6
            (17, 25, 2),  # T5
            (25, 33, 2),  # T4
            (33, 39, 2),  # T3
            (39, 44, 1),  # T2
            (44, 48, 2),  # T1
        ],
    },
    {
        "id":          "AddedRadiationDamage",
        "type":        "Prefix",
        "group":       "Group.AddedDamage",
        "display_tag": "Damage, Radiation",
        "attr":        "Attributes.Damage.AddedRadiation",
        "format":      "+{1} Added Radiation Damage",
        "tier_config": TIER_CONFIG_12,
        "names": [
            "IRRADIATED",    # T12
            "GLOWING",       # T11
            "EMITTING",      # T10
            "RADIOACTIVE",   # T9
            "CONTAMINATING", # T8
            "DECAYING",      # T7
            "ISOTOPIC",      # T6
            "IONIZING",      # T5
            "FISSILE",       # T4
            "NUCLEAR",       # T3
            "FALLOUT",       # T2
            "RADIANT",       # T1
        ],
        "ranges": [
            (2,  2,  1),  # T12 — single value per tier
            (5,  5,  1),  # T11
            (9,  9,  1),  # T10
            (14, 14, 1),  # T9
            (19, 19, 1),  # T8
            (24, 24, 1),  # T7
            (29, 29, 1),  # T6
            (33, 33, 1),  # T5
            (37, 37, 1),  # T4
            (41, 41, 1),  # T3
            (46, 46, 1),  # T2
            (50, 50, 1),  # T1
        ],
    },
    {
        "id":          "AddedExplosiveDamage",
        "type":        "Prefix",
        "group":       "Group.AddedDamage",
        "display_tag": "Damage, Explosive",
        "attr":        "Attributes.Damage.AddedExplosive",
        "format":      "+{1} Added Explosive Damage",
        "tier_config": TIER_CONFIG_10,
        "names": [
            "SPARKING",     # T10
            "CRACKLING",    # T9
            "BURSTING",     # T8
            "BLASTING",     # T7
            "DETONATING",   # T6
            "IGNITING",     # T5
            "CONCUSSIVE",   # T4
            "SHATTERING",   # T3
            "CATACLYSMIC",  # T2
            "EXPLOSIVE",    # T1
        ],
        "ranges": [
            (1,  5,  1),  # T10
            (5,  11, 2),  # T9
            (11, 18, 1),  # T8
            (18, 26, 2),  # T7
            (26, 33, 1),  # T6
            (33, 39, 2),  # T5
            (39, 43, 2),  # T4
            (43, 46, 1),  # T3
            (46, 48, 1),  # T2
            (48, 50, 1),  # T1
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
                "Flat",                    # Stat1_DisplayAs
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
    added = 0
    skipped = 0
    for row in rows:
        if row[0] in existing:
            skipped += 1
        else:
            ws.append(row)
            added += 1

    wb.save(WORKBOOK_PATH)
    print(f"Done — added {added} rows, skipped {skipped} already-existing rows.")
    print(f"  Physical: 7 tiers | Energy: 9 tiers | Chemical: 8 tiers | Radiation: 12 tiers (single-value) | Explosive: 10 tiers")


if __name__ == "__main__":
    main()
