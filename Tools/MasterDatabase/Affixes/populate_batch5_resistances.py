"""
Batch 5 — Resistance affixes (all Suffix)

  Group.MaxResistance    — 6 types × 5 tiers, flat integer (+1 to +10% cap)
                           Weights heavily biased toward T3 (5-6), T1 (9-10) weight=10
  Group.Resistance       — 6 types × 6 tiers, percentage (6-40%)
  Group.HybridResistance — 10 pairs from 5 damage types × 6 tiers, percentage
                           Both stats identical per tier, both max at 25%
                           Overheat excluded from hybrid.
"""
import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

# Max resistance: 5 tiers, weights skewed toward T3 (5-6), T1 (9-10) extremely rare
TIER_CONFIG_MAXRES5 = [
    (5, 1,  100),
    (4, 20, 200),
    (3, 40, 350),
    (2, 60, 100),
    (1, 75, 10),
]

# Individual resistance: 6 tiers, ilvl 1-82, standard bell curve
TIER_CONFIG_RES6 = [
    (6, 1,  50),
    (5, 17, 100),
    (4, 33, 200),
    (3, 50, 300),
    (2, 66, 200),
    (1, 82, 100),
]

# Hybrid resistance: 6 tiers, ilvl 36-85 (mid-to-late game only)
TIER_CONFIG_HYBRID6 = [
    (6, 36,  50),
    (5, 46, 100),
    (4, 57, 200),
    (3, 67, 300),
    (2, 76, 200),
    (1, 85, 100),
]

# Max resistance ranges: same for all types (T5→T1)
MAX_RES_RANGES = [
    (1, 2, 1),  # T5
    (3, 4, 1),  # T4
    (5, 6, 1),  # T3
    (7, 8, 1),  # T2
    (9, 10, 1), # T1
]

# Individual resistance ranges: T6→T1, max 40%
INDIV_RES_RANGES = [
    (6,  10, 1),  # T6
    (11, 16, 1),  # T5
    (17, 22, 1),  # T4
    (23, 28, 1),  # T3
    (29, 33, 1),  # T2
    (34, 40, 1),  # T1
]

# Hybrid resistance ranges: both stats same per tier, max 25%
HYBRID_RES_RANGES = [
    (4,  6,  1),  # T6
    (7,  10, 1),  # T5
    (11, 14, 1),  # T4
    (15, 18, 1),  # T3
    (19, 21, 1),  # T2
    (22, 25, 1),  # T1
]


AFFIXES = [

    # ── Maximum Resistance ────────────────────────────────────────────────────
    # DisplayAs="Flat": stored 1-10, format string adds %, engine uses value directly as % cap bonus

    {
        "id":          "MaxPhysicalResistance",
        "type":        "Suffix",
        "group":       "Group.MaxResistance",
        "display_tag": "Resistance, Max Physical",
        "attr":        "Attributes.Resistance.MaxPhysical",
        "stat2_attr":  "",
        "display_as":  "Percent",
        "format":      "+{1}% Maximum Physical Resistance",
        "tier_config": TIER_CONFIG_MAXRES5,
        "names": [
            "CALLOUSED",  # T5
            "TOUGHENED",  # T4
            "HARDENED",   # T3
            "FORTIFIED",  # T2
            "IMPERVIOUS", # T1
        ],
        "ranges": MAX_RES_RANGES,
    },
    {
        "id":          "MaxEnergyResistance",
        "type":        "Suffix",
        "group":       "Group.MaxResistance",
        "display_tag": "Resistance, Max Energy",
        "attr":        "Attributes.Resistance.MaxEnergy",
        "stat2_attr":  "",
        "display_as":  "Percent",
        "format":      "+{1}% Maximum Energy Resistance",
        "tier_config": TIER_CONFIG_MAXRES5,
        "names": [
            "INSULATING",   # T5
            "TEMPERING",    # T4
            "FIREPROOFING", # T3
            "HEATPROOFING", # T2
            "PYROPROOF",    # T1
        ],
        "ranges": MAX_RES_RANGES,
    },
    {
        "id":          "MaxChemicalResistance",
        "type":        "Suffix",
        "group":       "Group.MaxResistance",
        "display_tag": "Resistance, Max Chemical",
        "attr":        "Attributes.Resistance.MaxChemical",
        "stat2_attr":  "",
        "display_as":  "Percent",
        "format":      "+{1}% Maximum Chemical Resistance",
        "tier_config": TIER_CONFIG_MAXRES5,
        "names": [
            "TREATING",    # T5
            "BUFFERING",   # T4
            "NEUTRALIZING",# T3
            "INERTING",    # T2
            "CHEMPROOF",   # T1
        ],
        "ranges": MAX_RES_RANGES,
    },
    {
        "id":          "MaxRadiationResistance",
        "type":        "Suffix",
        "group":       "Group.MaxResistance",
        "display_tag": "Resistance, Max Radiation",
        "attr":        "Attributes.Resistance.MaxRadiation",
        "stat2_attr":  "",
        "display_as":  "Percent",
        "format":      "+{1}% Maximum Radiation Resistance",
        "tier_config": TIER_CONFIG_MAXRES5,
        "names": [
            "SCREENING",       # T5
            "SHIELDING",       # T4
            "LEADED",          # T3
            "DECONTAMINATING", # T2
            "RADPROOF",        # T1
        ],
        "ranges": MAX_RES_RANGES,
    },
    {
        "id":          "MaxExplosiveResistance",
        "type":        "Suffix",
        "group":       "Group.MaxResistance",
        "display_tag": "Resistance, Max Explosive",
        "attr":        "Attributes.Resistance.MaxExplosive",
        "stat2_attr":  "",
        "display_as":  "Percent",
        "format":      "+{1}% Maximum Explosive Resistance",
        "tier_config": TIER_CONFIG_MAXRES5,
        "names": [
            "PADDING",         # T5
            "REINFORCING",     # T4
            "BLAST-DAMPENING", # T3
            "BLAST-PROOFING",  # T2
            "SHOCKPROOF",      # T1
        ],
        "ranges": MAX_RES_RANGES,
    },
    {
        "id":          "MaxOverheatResistance",
        "type":        "Suffix",
        "group":       "Group.MaxResistance",
        "display_tag": "Resistance, Max Overheat",
        "attr":        "Attributes.Resistance.MaxOverheat",
        "stat2_attr":  "",
        "display_as":  "Percent",
        "format":      "+{1}% Maximum Overheat Resistance",
        "tier_config": TIER_CONFIG_MAXRES5,
        "names": [
            "REGULATING",     # T5
            "COOLING",        # T4
            "CONTROLLING",    # T3
            "STABILIZING",    # T2
            "THERMOREGULATED",# T1
        ],
        "ranges": MAX_RES_RANGES,
    },

    # ── Individual Resistance ─────────────────────────────────────────────────

    {
        "id":          "PhysicalResistance",
        "type":        "Suffix",
        "group":       "Group.Resistance",
        "display_tag": "Resistance, Physical",
        "attr":        "Attributes.Resistance.Physical",
        "stat2_attr":  "",
        "display_as":  "Percent",
        "format":      "+{1}% Physical Resistance",
        "tier_config": TIER_CONFIG_RES6,
        "names": [
            "SCUFFED",  # T6
            "SCARRED",  # T5
            "TOUGH",    # T4
            "ARMORED",  # T3
            "PLATED",   # T2
            "PHYSICAL", # T1
        ],
        "ranges": INDIV_RES_RANGES,
    },
    {
        "id":          "EnergyResistance",
        "type":        "Suffix",
        "group":       "Group.Resistance",
        "display_tag": "Resistance, Energy",
        "attr":        "Attributes.Resistance.Energy",
        "stat2_attr":  "",
        "display_as":  "Percent",
        "format":      "+{1}% Energy Resistance",
        "tier_config": TIER_CONFIG_RES6,
        "names": [
            "LUKEWARM",    # T6
            "INSULATED",   # T5
            "TEMPERED",    # T4
            "HEAT-TREATED",# T3
            "FIREPROOFED", # T2
            "ENERGY",      # T1
        ],
        "ranges": INDIV_RES_RANGES,
    },
    {
        "id":          "ChemicalResistance",
        "type":        "Suffix",
        "group":       "Group.Resistance",
        "display_tag": "Resistance, Chemical",
        "attr":        "Attributes.Resistance.Chemical",
        "stat2_attr":  "",
        "display_as":  "Percent",
        "format":      "+{1}% Chemical Resistance",
        "tier_config": TIER_CONFIG_RES6,
        "names": [
            "TREATED",    # T6
            "BUFFERED",   # T5
            "NEUTRALIZED",# T4
            "PURIFIED",   # T3
            "INERTED",    # T2
            "CHEMICAL",   # T1
        ],
        "ranges": INDIV_RES_RANGES,
    },
    {
        "id":          "RadiationResistance",
        "type":        "Suffix",
        "group":       "Group.Resistance",
        "display_tag": "Resistance, Radiation",
        "attr":        "Attributes.Resistance.Radiation",
        "stat2_attr":  "",
        "display_as":  "Percent",
        "format":      "+{1}% Radiation Resistance",
        "tier_config": TIER_CONFIG_RES6,
        "names": [
            "SHIELDED",       # T6
            "SCREENED",       # T5
            "LEAD-LINED",     # T4
            "DECONTAMINATED", # T3
            "HARDENED",       # T2
            "RADIATION",      # T1
        ],
        "ranges": INDIV_RES_RANGES,
    },
    {
        "id":          "ExplosiveResistance",
        "type":        "Suffix",
        "group":       "Group.Resistance",
        "display_tag": "Resistance, Explosive",
        "attr":        "Attributes.Resistance.Explosive",
        "stat2_attr":  "",
        "display_as":  "Percent",
        "format":      "+{1}% Explosive Resistance",
        "tier_config": TIER_CONFIG_RES6,
        "names": [
            "PADDED",       # T6
            "DAMPENED",     # T5
            "REINFORCED",   # T4
            "FORTIFIED",    # T3
            "BLAST-PROOFED",# T2
            "EXPLOSIVE",    # T1
        ],
        "ranges": INDIV_RES_RANGES,
    },
    {
        "id":          "OverheatResistance",
        "type":        "Suffix",
        "group":       "Group.Resistance",
        "display_tag": "Resistance, Overheat",
        "attr":        "Attributes.Resistance.Overheat",
        "stat2_attr":  "",
        "display_as":  "Percent",
        "format":      "+{1}% Overheat Resistance",
        "tier_config": TIER_CONFIG_RES6,
        "names": [
            "COOLED",       # T6
            "REGULATED",    # T5
            "STABILIZED",   # T4
            "CONTROLLED",   # T3
            "THERMOSTATIC", # T2
            "OVERHEAT",     # T1
        ],
        "ranges": INDIV_RES_RANGES,
    },

    # ── Hybrid Resistance — 10 pairs from 5 damage types (no overheat) ────────
    # Both stats roll identically per tier. T6-T2 share generic names; T1 is unique.

    {
        "id":          "HybridPhysicalEnergyResistance",
        "type":        "Suffix",
        "group":       "Group.HybridResistance",
        "display_tag": "Resistance, Physical, Energy",
        "attr":        "Attributes.Resistance.Physical",
        "stat2_attr":  "Attributes.Resistance.Energy",
        "display_as":  "Percent",
        "format":      "+{1}% Physical and +{2}% Energy Resistance",
        "tier_config": TIER_CONFIG_HYBRID6,
        "names": ["WARDED", "DEFENDED", "PROTECTED", "GUARDED", "SHIELDED", "THERMOPLATED"],
        "ranges": HYBRID_RES_RANGES,
    },
    {
        "id":          "HybridPhysicalChemicalResistance",
        "type":        "Suffix",
        "group":       "Group.HybridResistance",
        "display_tag": "Resistance, Physical, Chemical",
        "attr":        "Attributes.Resistance.Physical",
        "stat2_attr":  "Attributes.Resistance.Chemical",
        "display_as":  "Percent",
        "format":      "+{1}% Physical and +{2}% Chemical Resistance",
        "tier_config": TIER_CONFIG_HYBRID6,
        "names": ["WARDED", "DEFENDED", "PROTECTED", "GUARDED", "SHIELDED", "BIOPLATED"],
        "ranges": HYBRID_RES_RANGES,
    },
    {
        "id":          "HybridPhysicalRadiationResistance",
        "type":        "Suffix",
        "group":       "Group.HybridResistance",
        "display_tag": "Resistance, Physical, Radiation",
        "attr":        "Attributes.Resistance.Physical",
        "stat2_attr":  "Attributes.Resistance.Radiation",
        "display_as":  "Percent",
        "format":      "+{1}% Physical and +{2}% Radiation Resistance",
        "tier_config": TIER_CONFIG_HYBRID6,
        "names": ["WARDED", "DEFENDED", "PROTECTED", "GUARDED", "SHIELDED", "RADIOPLATED"],
        "ranges": HYBRID_RES_RANGES,
    },
    {
        "id":          "HybridPhysicalExplosiveResistance",
        "type":        "Suffix",
        "group":       "Group.HybridResistance",
        "display_tag": "Resistance, Physical, Explosive",
        "attr":        "Attributes.Resistance.Physical",
        "stat2_attr":  "Attributes.Resistance.Explosive",
        "display_as":  "Percent",
        "format":      "+{1}% Physical and +{2}% Explosive Resistance",
        "tier_config": TIER_CONFIG_HYBRID6,
        "names": ["WARDED", "DEFENDED", "PROTECTED", "GUARDED", "SHIELDED", "BLASTPLATED"],
        "ranges": HYBRID_RES_RANGES,
    },
    {
        "id":          "HybridEnergyChemicalResistance",
        "type":        "Suffix",
        "group":       "Group.HybridResistance",
        "display_tag": "Resistance, Energy, Chemical",
        "attr":        "Attributes.Resistance.Energy",
        "stat2_attr":  "Attributes.Resistance.Chemical",
        "display_as":  "Percent",
        "format":      "+{1}% Energy and +{2}% Chemical Resistance",
        "tier_config": TIER_CONFIG_HYBRID6,
        "names": ["WARDED", "DEFENDED", "PROTECTED", "GUARDED", "SHIELDED", "THERMOACTIVE"],
        "ranges": HYBRID_RES_RANGES,
    },
    {
        "id":          "HybridEnergyRadiationResistance",
        "type":        "Suffix",
        "group":       "Group.HybridResistance",
        "display_tag": "Resistance, Energy, Radiation",
        "attr":        "Attributes.Resistance.Energy",
        "stat2_attr":  "Attributes.Resistance.Radiation",
        "display_as":  "Percent",
        "format":      "+{1}% Energy and +{2}% Radiation Resistance",
        "tier_config": TIER_CONFIG_HYBRID6,
        "names": ["WARDED", "DEFENDED", "PROTECTED", "GUARDED", "SHIELDED", "THERMORADIANT"],
        "ranges": HYBRID_RES_RANGES,
    },
    {
        "id":          "HybridEnergyExplosiveResistance",
        "type":        "Suffix",
        "group":       "Group.HybridResistance",
        "display_tag": "Resistance, Energy, Explosive",
        "attr":        "Attributes.Resistance.Energy",
        "stat2_attr":  "Attributes.Resistance.Explosive",
        "display_as":  "Percent",
        "format":      "+{1}% Energy and +{2}% Explosive Resistance",
        "tier_config": TIER_CONFIG_HYBRID6,
        "names": ["WARDED", "DEFENDED", "PROTECTED", "GUARDED", "SHIELDED", "PYROGUARDED"],
        "ranges": HYBRID_RES_RANGES,
    },
    {
        "id":          "HybridChemicalRadiationResistance",
        "type":        "Suffix",
        "group":       "Group.HybridResistance",
        "display_tag": "Resistance, Chemical, Radiation",
        "attr":        "Attributes.Resistance.Chemical",
        "stat2_attr":  "Attributes.Resistance.Radiation",
        "display_as":  "Percent",
        "format":      "+{1}% Chemical and +{2}% Radiation Resistance",
        "tier_config": TIER_CONFIG_HYBRID6,
        "names": ["WARDED", "DEFENDED", "PROTECTED", "GUARDED", "SHIELDED", "TOXIRADIANT"],
        "ranges": HYBRID_RES_RANGES,
    },
    {
        "id":          "HybridChemicalExplosiveResistance",
        "type":        "Suffix",
        "group":       "Group.HybridResistance",
        "display_tag": "Resistance, Chemical, Explosive",
        "attr":        "Attributes.Resistance.Chemical",
        "stat2_attr":  "Attributes.Resistance.Explosive",
        "display_as":  "Percent",
        "format":      "+{1}% Chemical and +{2}% Explosive Resistance",
        "tier_config": TIER_CONFIG_HYBRID6,
        "names": ["WARDED", "DEFENDED", "PROTECTED", "GUARDED", "SHIELDED", "VOLATILEGUARDED"],
        "ranges": HYBRID_RES_RANGES,
    },
    {
        "id":          "HybridRadiationExplosiveResistance",
        "type":        "Suffix",
        "group":       "Group.HybridResistance",
        "display_tag": "Resistance, Radiation, Explosive",
        "attr":        "Attributes.Resistance.Radiation",
        "stat2_attr":  "Attributes.Resistance.Explosive",
        "display_as":  "Percent",
        "format":      "+{1}% Radiation and +{2}% Explosive Resistance",
        "tier_config": TIER_CONFIG_HYBRID6,
        "names": ["WARDED", "DEFENDED", "PROTECTED", "GUARDED", "SHIELDED", "NUCLEARGUARDED"],
        "ranges": HYBRID_RES_RANGES,
    },
]


def build_rows():
    rows = []
    for affix in AFFIXES:
        has_stat2 = bool(affix.get("stat2_attr", ""))
        for i, (tier, ilvl, weight) in enumerate(affix["tier_config"]):
            min_val, max_val, step = affix["ranges"][i]
            rows.append([
                f"{affix['id']}_T{tier}",   # RowName
                affix["id"],                # AffixID
                affix["names"][i],          # ItemName
                affix["type"],              # Type
                affix["group"],             # GroupTag
                affix["display_tag"],       # DisplayTag
                tier,                       # Tier
                ilvl,                       # RequiredItemLevel
                weight,                     # Weight
                affix["attr"],              # Stat1_Attr
                min_val,                    # Stat1_Min
                max_val,                    # Stat1_Max
                step,                       # Stat1_Step
                0,                          # Stat1_DecimalCount
                affix["display_as"],        # Stat1_DisplayAs
                affix["stat2_attr"] if has_stat2 else "",  # Stat2_Attr
                min_val if has_stat2 else "",               # Stat2_Min (identical roll)
                max_val if has_stat2 else "",               # Stat2_Max
                step    if has_stat2 else "",               # Stat2_Step
                0       if has_stat2 else "",               # Stat2_DecimalCount
                affix["display_as"] if has_stat2 else "",  # Stat2_DisplayAs
                affix["format"],            # DisplayFormat
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
    print(f"  Max resistance:    6×5  = 30 rows")
    print(f"  Individual resist: 6×6  = 36 rows")
    print(f"  Hybrid resistance: 10×6 = 60 rows")
    print(f"  Total expected:          126 rows")


if __name__ == "__main__":
    main()
