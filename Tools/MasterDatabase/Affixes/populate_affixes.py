import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

# Shared tier config: (Tier, RequiredItemLevel, Weight)
# T8 = weakest/lowest ilvl, T1 = best/highest ilvl
# Weights form a bell curve — mid tiers most common
TIER_CONFIG = [
    (8, 1,  50),
    (7, 10, 100),
    (6, 20, 200),
    (5, 32, 300),
    (4, 44, 300),
    (3, 57, 200),
    (2, 68, 100),
    (1, 80, 50),
]

# Each entry defines one affix across all tiers.
# names[0] = T8 name, names[7] = T1 name (the "true" name, reserved for best tier)
# ranges[i] = (min, max) for that tier index
AFFIXES = [
    {
        "id":          "AddedMight",
        "type":        "Suffix",
        "group":       "Group.PrimaryMight",
        "display_tag": "Primary, Might",
        "attr":        "Attributes.Primary.Might",
        "format":      "+{1} Might",
        "names": [
            "OF BRAWN",       # T8
            "OF THE BRAWLER", # T7
            "OF FORCE",       # T6
            "OF VIGOR",       # T5
            "OF STRENGTH",    # T4
            "OF POWER",       # T3
            "OF DOMINANCE",   # T2
            "OF MIGHT",       # T1
        ],
        "ranges": [
            (1,  2),   # T8
            (2,  4),   # T7
            (4,  6),   # T6
            (6,  9),   # T5
            (9,  12),  # T4
            (12, 16),  # T3
            (16, 20),  # T2
            (20, 25),  # T1
        ],
    },
    {
        "id":          "AddedResonance",
        "type":        "Suffix",
        "group":       "Group.PrimaryResonance",
        "display_tag": "Primary, Resonance",
        "attr":        "Attributes.Primary.Resonance",
        "format":      "+{1} Resonance",
        "names": [
            "OF SPARKS",     # T8
            "OF STATIC",     # T7
            "OF CURRENT",    # T6
            "OF SIGNAL",     # T5
            "OF FREQUENCY",  # T4
            "OF ATTUNEMENT", # T3
            "OF HARMONY",    # T2
            "OF RESONANCE",  # T1
        ],
        "ranges": [
            (1,  2),
            (2,  4),
            (4,  6),
            (6,  9),
            (9,  12),
            (12, 16),
            (16, 20),
            (20, 25),
        ],
    },
    {
        "id":          "AddedAgility",
        "type":        "Suffix",
        "group":       "Group.PrimaryAgility",
        "display_tag": "Primary, Agility",
        "attr":        "Attributes.Primary.Agility",
        "format":      "+{1} Agility",
        "names": [
            "OF HASTE",       # T8
            "OF THE SWIFT",   # T7
            "OF QUICKNESS",   # T6
            "OF NIMBLENESS",  # T5
            "OF DEXTERITY",   # T4
            "OF CELERITY",    # T3
            "OF SWIFTNESS",   # T2
            "OF AGILITY",     # T1
        ],
        "ranges": [
            (1,  2),
            (2,  4),
            (4,  6),
            (6,  9),
            (9,  12),
            (12, 16),
            (16, 20),
            (20, 25),
        ],
    },
    {
        "id":          "AddedFortitude",
        "type":        "Suffix",
        "group":       "Group.PrimaryFortitude",
        "display_tag": "Primary, Fortitude",
        "attr":        "Attributes.Primary.Fortitude",
        "format":      "+{1} Fortitude",
        "names": [
            "OF GRIT",          # T8
            "OF ENDURANCE",     # T7
            "OF RESILIENCE",    # T6
            "OF TOUGHNESS",     # T5
            "OF HARDINESS",     # T4
            "OF TENACITY",      # T3
            "OF CONSTITUTION",  # T2
            "OF FORTITUDE",     # T1
        ],
        "ranges": [
            (1,  2),
            (2,  4),
            (4,  6),
            (6,  9),
            (9,  12),
            (12, 16),
            (16, 20),
            (20, 25),
        ],
    },
]

def build_rows():
    rows = []
    for affix in AFFIXES:
        for i, (tier, ilvl, weight) in enumerate(TIER_CONFIG):
            min_val, max_val = affix["ranges"][i]
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
                1,                         # Stat1_Step
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

    rows = build_rows()
    for row in rows:
        ws.append(row)

    wb.save(WORKBOOK_PATH)
    print(f"Done — wrote {len(rows)} rows to {WORKBOOK_PATH}")

if __name__ == "__main__":
    main()
