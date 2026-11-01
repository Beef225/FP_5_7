import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

# 10-tier config: (Tier, RequiredItemLevel, Weight)
# Bell-curve weighting — mid tiers most common
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

# ranges: (Stat1_Min, Stat1_Max, Stat1_Step) per tier index (index 0 = T10, index 9 = T1)
AFFIXES = [
    {
        "id":          "MaxHitPoints",
        "type":        "Prefix",
        "group":       "Group.VitalMaxHitPoints",
        "display_tag": "Vital, HitPoints",
        "attr":        "Attributes.Vital.MaxHitPoints",
        "format":      "+{1} Maximum Hit Points",
        "tier_config": TIER_CONFIG_10,
        "names": [
            "HALE",       # T10
            "HEALTHY",    # T9
            "VITAL",      # T8
            "ROBUST",     # T7
            "VIGOROUS",   # T6
            "HEARTY",     # T5
            "RESILIENT",  # T4
            "ENDURING",   # T3
            "STALWART",   # T2
            "UNDYING",    # T1
        ],
        "ranges": [
            (1,   9,   1),  # T10
            (9,   21,  3),  # T9
            (21,  37,  4),  # T8
            (37,  57,  5),  # T7
            (57,  77,  5),  # T6
            (77,  97,  5),  # T5
            (97,  113, 4),  # T4
            (113, 125, 3),  # T3
            (125, 137, 3),  # T2
            (138, 150, 3),  # T1
        ],
    },
    {
        "id":          "MaxHeatThreshold",
        "type":        "Prefix",
        "group":       "Group.VitalMaxHeat",
        "display_tag": "Vital, Heat",
        "attr":        "Attributes.Vital.MaxHeatThreshold",
        "format":      "+{1} Maximum Heat Threshold",
        "tier_config": TIER_CONFIG_10,
        "names": [
            "WARM",           # T10
            "HEATED",         # T9
            "TEMPERED",       # T8
            "SEARING",        # T7
            "BLAZING",        # T6
            "SCORCHING",      # T5
            "FORGE-TEMPERED", # T4
            "INFERNAL",       # T3
            "VOLCANIC",       # T2
            "PYROCLASTIC",    # T1
        ],
        "ranges": [
            (1,   12,  1),  # T10
            (12,  30,  3),  # T9
            (30,  50,  5),  # T8
            (50,  75,  5),  # T7
            (75,  100, 5),  # T6
            (100, 125, 5),  # T5
            (125, 150, 5),  # T4
            (150, 170, 5),  # T3
            (170, 190, 5),  # T2
            (190, 200, 2),  # T1
        ],
    },
    {
        "id":          "MinHeatThreshold",
        "type":        "Prefix",
        "group":       "Group.VitalMinHeat",
        "display_tag": "Vital, Heat",
        "attr":        "Attributees.VitalMinHeatThreshold",
        "format":      "{1} Minimum Heat Threshold",
        "tier_config": TIER_CONFIG_10,
        "names": [
            "COOL",        # T10
            "CHILLED",     # T9
            "FROSTED",     # T8
            "GLACIAL",     # T7
            "ARCTIC",      # T6
            "FROZEN",      # T5
            "CRYOGENIC",   # T4
            "ABSOLUTE",    # T3
            "ZERO-POINT",  # T2
            "SUPERCOOLED", # T1
        ],
        "ranges": [
            (-10,  -1,   1),  # T10
            (-30,  -12,  3),  # T9
            (-50,  -30,  5),  # T8
            (-75,  -50,  5),  # T7
            (-100, -75,  5),  # T6
            (-125, -100, 5),  # T5
            (-150, -125, 5),  # T4
            (-170, -150, 5),  # T3
            (-190, -170, 5),  # T2
            (-200, -190, 2),  # T1
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


if __name__ == "__main__":
    main()
