"""
Batch 8 — Heat system affixes (all Prefix, non-exclusive — each has its own group)

  Group.ThermalMass          flat,    4T  ilvl 40-80   max 50       "+{1} TO THERMAL MASS"
  Group.AmbientTemperature   flat,    1T  ilvl 1       range 90-110  legendary placeholder
  Group.CoolingCoefficient   flat,2dec 8T  ilvl 10-70  max 2.00     step 0.01
"""
import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

TIER_4_40_80 = [
    (4, 40, 150),
    (3, 53, 250),
    (2, 67, 250),
    (1, 80, 150),
]

TIER_1_AMBIENT = [
    (1, 1, 1000),  # single tier, ilvl 1 — reserved for legendary use
]

TIER_8_10_70 = [
    (8, 10,  50),
    (7, 19, 100),
    (6, 28, 150),
    (5, 37, 200),
    (4, 46, 300),
    (3, 54, 200),
    (2, 62, 150),
    (1, 70, 100),
]

AFFIXES = [

    # ── Thermal Mass ──────────────────────────────────────────────────────────
    {
        "id":          "ThermalMass",
        "type":        "Prefix",
        "group":       "Group.ThermalMass",
        "display_tag": "Heat, Thermal Mass",
        "attr":        "Attributes.Heat.ThermalMass",
        "display_as":  "Flat",
        "decimal":     0,
        "step":        1,
        "format":      "+{1} TO THERMAL MASS",
        "tier_config": TIER_4_40_80,
        "names": [
            "DENSE",    # T4
            "MASSIVE",  # T3
            "THERMAL",  # T2
            "THERMIC",  # T1
        ],
        "ranges": [
            (1,  15, 1),  # T4
            (16, 30, 1),  # T3
            (31, 42, 1),  # T2
            (43, 50, 1),  # T1
        ],
    },

    # ── Ambient Temperature ───────────────────────────────────────────────────
    # Single-tier legendary placeholder. Rolls 90-110 at ilvl 1.
    {
        "id":          "AmbientTemperature",
        "type":        "Prefix",
        "group":       "Group.AmbientTemperature",
        "display_tag": "Heat, Ambient",
        "attr":        "Attributes.Heat.AmbientTemperature",
        "display_as":  "Flat",
        "decimal":     0,
        "step":        1,
        "format":      "+{1} Ambient Temperature",
        "tier_config": TIER_1_AMBIENT,
        "names": [
            "AMBIENT",  # T1
        ],
        "ranges": [
            (90, 110, 1),  # T1
        ],
    },

    # ── Cooling Coefficient ───────────────────────────────────────────────────
    # Step 0.01 — 2-decimal float values, range 0.10-2.00.
    {
        "id":          "CoolingCoefficient",
        "type":        "Prefix",
        "group":       "Group.CoolingCoefficient",
        "display_tag": "Heat, Cooling",
        "attr":        "Attributes.Heat.CoolingCoefficient",
        "display_as":  "Flat",
        "decimal":     2,
        "step":        0.01,
        "format":      "+{1} Cooling Coefficient",
        "tier_config": TIER_8_10_70,
        "names": [
            "MILD",       # T8
            "BREEZY",     # T7
            "COOLING",    # T6
            "CHILLING",   # T5
            "FRIGID",     # T4
            "GLACIAL",    # T3
            "CRYOGENIC",  # T2
            "ABSOLUTE",   # T1
        ],
        "ranges": [
            (0.10, 0.30, 0.01),  # T8
            (0.31, 0.55, 0.01),  # T7
            (0.56, 0.80, 0.01),  # T6
            (0.81, 1.10, 0.01),  # T5
            (1.11, 1.40, 0.01),  # T4
            (1.41, 1.65, 0.01),  # T3
            (1.66, 1.85, 0.01),  # T2
            (1.86, 2.00, 0.01),  # T1
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
    print(f"  ThermalMass:          4 rows  (1-50, flat)")
    print(f"  AmbientTemperature:   1 row   (90-110, legendary placeholder)")
    print(f"  CoolingCoefficient:   8 rows  (0.10-2.00, step 0.01, 2 decimals)")
    print(f"  Total: 13 rows")


if __name__ == "__main__":
    main()
