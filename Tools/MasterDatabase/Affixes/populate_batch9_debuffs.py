"""
Batch 9 — Debuff affixes (15 affixes, 100 rows total)

Groups (mutually exclusive within set):
  Group.DebuffEffectiveness  — 5 types (Prefix, %, 8T ilvl 20-82, max 50%)
  Group.DebuffThreshold      — 5 types (Prefix, flat, 6T ilvl 30-82, max +200)
  Group.DebuffDuration       — 4 durations + ThermalInsulation (Suffix, 6T ilvl 25-80)

Duration: fixed single values, 1 decimal, negative (-0.5 → -3.0 step -0.5)
ThermalInsulation: % in place of ThermalDestabilization duration, max 50%
"""
import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

# Effectiveness: 8 tiers, ilvl 20-82
TIER_8_20_82 = [
    (8, 20,  50),
    (7, 30, 100),
    (6, 40, 150),
    (5, 50, 200),
    (4, 60, 300),
    (3, 68, 200),
    (2, 75, 150),
    (1, 82, 100),
]

# Threshold: 6 tiers, ilvl 30-82
TIER_6_30_82 = [
    (6, 30,  50),
    (5, 44, 100),
    (4, 56, 200),
    (3, 64, 300),
    (2, 73, 200),
    (1, 82, 100),
]

# Duration / ThermalInsulation: 6 tiers, ilvl 25-80
TIER_6_25_80 = [
    (6, 25,  50),
    (5, 38, 100),
    (4, 51, 200),
    (3, 61, 300),
    (2, 70, 200),
    (1, 80, 100),
]

# Shared effectiveness ranges (all 5 types identical, max 50%)
EFFECTIVENESS_RANGES = [
    ( 4,  7, 1),  # T8
    ( 8, 12, 1),  # T7
    (13, 18, 1),  # T6
    (19, 24, 1),  # T5
    (25, 31, 1),  # T4
    (32, 38, 1),  # T3
    (39, 44, 1),  # T2
    (45, 50, 1),  # T1
]

# Shared threshold ranges (all 5 types identical, flat +10 → +200)
THRESHOLD_RANGES = [
    ( 10,  40, 1),  # T6
    ( 41,  75, 1),  # T5
    ( 76, 110, 1),  # T4
    (111, 145, 1),  # T3
    (146, 175, 1),  # T2
    (176, 200, 1),  # T1
]

# Shared duration ranges (fixed single values, negative)
DURATION_RANGES = [
    (-0.5, -0.5, 0.5),  # T6
    (-1.0, -1.0, 0.5),  # T5
    (-1.5, -1.5, 0.5),  # T4
    (-2.0, -2.0, 0.5),  # T3
    (-2.5, -2.5, 0.5),  # T2
    (-3.0, -3.0, 0.5),  # T1
]

AFFIXES = [

    # ══════════════════════════════════════════════════════════════════════════
    # EFFECTIVENESS — Prefix, Group.DebuffEffectiveness, %, 8T ilvl 20-82
    # ══════════════════════════════════════════════════════════════════════════

    {
        "id":          "BruisedEffectiveness",
        "type":        "Prefix",
        "group":       "Group.DebuffEffectiveness",
        "display_tag": "Debuff_Effectiveness_Bruised",
        "attr":        "Attributes.Debuff.Bruised.Effectiveness",
        "display_as":  "Percent",
        "decimal":     0,
        "step":        1,
        "format":      "+{1}% Bruised Effectiveness",
        "tier_config": TIER_8_20_82,
        "names": [
            "SOFTENING",    # T8
            "BRUISING",     # T7
            "BATTERING",    # T6
            "CRIPPLING",    # T5
            "MANGLING",     # T4
            "MAIMING",      # T3
            "PULVERIZING",  # T2
            "TRAUMATIZING", # T1
        ],
        "ranges": EFFECTIVENESS_RANGES,
    },

    {
        "id":          "ConcussedEffectiveness",
        "type":        "Prefix",
        "group":       "Group.DebuffEffectiveness",
        "display_tag": "Debuff_Effectiveness_Concussed",
        "attr":        "Attributes.Debuff.Concussed.Effectiveness",
        "display_as":  "Percent",
        "decimal":     0,
        "step":        1,
        "format":      "+{1}% Concussed Effectiveness",
        "tier_config": TIER_8_20_82,
        "names": [
            "JARRING",        # T8
            "RATTLING",       # T7
            "DISORIENTING",   # T6
            "STAGGERING",     # T5
            "DAZING",         # T4
            "STUNNING",       # T3
            "CONCUSSING",     # T2
            "INCAPACITATING", # T1
        ],
        "ranges": EFFECTIVENESS_RANGES,
    },

    {
        "id":          "IrradiatedEffectiveness",
        "type":        "Prefix",
        "group":       "Group.DebuffEffectiveness",
        "display_tag": "Debuff_Effectiveness_Irradiated",
        "attr":        "Attributes.Debuff.Irradiated.Effectiveness",
        "display_as":  "Percent",
        "decimal":     0,
        "step":        1,
        "format":      "+{1}% Irradiated Effectiveness",
        "tier_config": TIER_8_20_82,
        "names": [
            "EXPOSING",      # T8
            "CONTAMINATING", # T7
            "IRRADIATING",   # T6
            "TAINTING",      # T5
            "INFECTING",     # T4
            "CORRUPTING",    # T3
            "MUTATING",      # T2
            "ANNIHILATING",  # T1
        ],
        "ranges": EFFECTIVENESS_RANGES,
    },

    {
        "id":          "CorrodedEffectiveness",
        "type":        "Prefix",
        "group":       "Group.DebuffEffectiveness",
        "display_tag": "Debuff_Effectiveness_Corroded",
        "attr":        "Attributes.Debuff.Corroded.Effectiveness",
        "display_as":  "Percent",
        "decimal":     0,
        "step":        1,
        "format":      "+{1}% Corroded Effectiveness",
        "tier_config": TIER_8_20_82,
        "names": [
            "TARNISHING",    # T8
            "ETCHING",       # T7
            "OXIDIZING",     # T6
            "CORRODING",     # T5
            "DISSOLVING",    # T4
            "DETERIORATING", # T3
            "DECOMPOSING",   # T2
            "ERODING",       # T1
        ],
        "ranges": EFFECTIVENESS_RANGES,
    },

    {
        "id":          "ThermalDestabilizationEffectiveness",
        "type":        "Prefix",
        "group":       "Group.DebuffEffectiveness",
        "display_tag": "Debuff_Effectiveness_ThermalDestabalization",
        "attr":        "Attributes.Debuff.ThermalDestabilization.Effectiveness",
        "display_as":  "Percent",
        "decimal":     0,
        "step":        1,
        "format":      "+{1}% Thermal Destabilization Effectiveness",
        "tier_config": TIER_8_20_82,
        "names": [
            "TEPID",        # T8
            "WARMING",      # T7
            "HEATING",      # T6
            "SCALDING",     # T5
            "SCORCHING",    # T4
            "SEARING",      # T3
            "INCINERATING", # T2
            "VAPORIZING",   # T1
        ],
        "ranges": EFFECTIVENESS_RANGES,
    },

    # ══════════════════════════════════════════════════════════════════════════
    # THRESHOLD — Prefix, Group.DebuffThreshold, flat, 6T ilvl 30-82, max +200
    # ══════════════════════════════════════════════════════════════════════════

    {
        "id":          "BruisedThreshold",
        "type":        "Prefix",
        "group":       "Group.DebuffThreshold",
        "display_tag": "Debuff_Threshold_Bruised",
        "attr":        "Attributes.Debuff.Bruised.Threshold",
        "display_as":  "Flat",
        "decimal":     0,
        "step":        1,
        "format":      "+{1} Bruised Threshold",
        "tier_config": TIER_6_30_82,
        "names": [
            "TENDERIZING", # T6
            "SOFTENING",   # T5
            "WEAKENING",   # T4
            "BRUTALIZING", # T3
            "CRUSHING",    # T2
            "SHATTERING",  # T1
        ],
        "ranges": THRESHOLD_RANGES,
    },

    {
        "id":          "ConcussedThreshold",
        "type":        "Prefix",
        "group":       "Group.DebuffThreshold",
        "display_tag": "Debuff_Threshold_Concussed",
        "attr":        "Attributes.Debuff.Concussed.Threshold",
        "display_as":  "Flat",
        "decimal":     0,
        "step":        1,
        "format":      "+{1} Concussed Threshold",
        "tier_config": TIER_6_30_82,
        "names": [
            "RATTLING",     # T6
            "DISORIENTING", # T5
            "STAGGERING",   # T4
            "DAZING",       # T3
            "BLUDGEONING",  # T2
            "OVERWHELMING", # T1
        ],
        "ranges": THRESHOLD_RANGES,
    },

    {
        "id":          "IrradiatedThreshold",
        "type":        "Prefix",
        "group":       "Group.DebuffThreshold",
        "display_tag": "Debuff_Threshold_Irradiated",
        "attr":        "Attributes.Debuff.Irradiated.Threshold",
        "display_as":  "Flat",
        "decimal":     0,
        "step":        1,
        "format":      "+{1} Irradiated Threshold",
        "tier_config": TIER_6_30_82,
        "names": [
            "EXPOSING",      # T6
            "CONTAMINATING", # T5
            "TAINTING",      # T4
            "INFECTING",     # T3
            "SATURATING",    # T2
            "PERMEATING",    # T1
        ],
        "ranges": THRESHOLD_RANGES,
    },

    {
        "id":          "CorrodedThreshold",
        "type":        "Prefix",
        "group":       "Group.DebuffThreshold",
        "display_tag": "Debuff_Threshold_Corroded",
        "attr":        "Attributes.Debuff.Corroded.Threshold",
        "display_as":  "Flat",
        "decimal":     0,
        "step":        1,
        "format":      "+{1} Corroded Threshold",
        "tier_config": TIER_6_30_82,
        "names": [
            "ETCHING",        # T6
            "TARNISHING",     # T5
            "CORRODING",      # T4
            "DISSOLVING",     # T3
            "DECOMPOSING",    # T2
            "DISINTEGRATING", # T1
        ],
        "ranges": THRESHOLD_RANGES,
    },

    {
        "id":          "ThermalDestabilizationThreshold",
        "type":        "Prefix",
        "group":       "Group.DebuffThreshold",
        "display_tag": "Debuff_Threshold_ThermalDestabalization",
        "attr":        "Attributes.Debuff.ThermalDestabilization.Threshold",
        "display_as":  "Flat",
        "decimal":     0,
        "step":        1,
        "format":      "+{1} Thermal Destabilization Threshold",
        "tier_config": TIER_6_30_82,
        "names": [
            "WARMING",      # T6
            "HEATING",      # T5
            "SCORCHING",    # T4
            "SEARING",      # T3
            "INCINERATING", # T2
            "VAPORIZING",   # T1
        ],
        "ranges": THRESHOLD_RANGES,
    },

    # ══════════════════════════════════════════════════════════════════════════
    # DURATION — Suffix, Group.DebuffDuration, fixed negatives, 1 dec, ilvl 25-80
    # Single values per tier: -0.5, -1.0, -1.5, -2.0, -2.5, -3.0
    # ══════════════════════════════════════════════════════════════════════════

    {
        "id":          "BruisedDuration",
        "type":        "Suffix",
        "group":       "Group.DebuffDuration",
        "display_tag": "Debuff_Duration_Bruised",
        "attr":        "Attributes.Debuff.Bruised.Duration",
        "display_as":  "Flat",
        "decimal":     1,
        "step":        0.5,
        "format":      "{1}s Bruised Duration",
        "tier_config": TIER_6_25_80,
        "names": [
            "OF RESILIENCE",      # T6
            "OF TOUGHENING",      # T5
            "OF HARDENING",       # T4
            "OF FORTIFICATION",   # T3
            "OF IMPERVIOUSNESS",  # T2
            "OF INVULNERABILITY", # T1
        ],
        "ranges": DURATION_RANGES,
    },

    {
        "id":          "ConcussedDuration",
        "type":        "Suffix",
        "group":       "Group.DebuffDuration",
        "display_tag": "Debuff_Duration_Concussed",
        "attr":        "Attributes.Debuff.Concussed.Duration",
        "display_as":  "Flat",
        "decimal":     1,
        "step":        0.5,
        "format":      "{1}s Concussed Duration",
        "tier_config": TIER_6_25_80,
        "names": [
            "OF CLARITY",    # T6
            "OF FOCUS",      # T5
            "OF COMPOSURE",  # T4
            "OF STEADINESS", # T3
            "OF ALERTNESS",  # T2
            "OF VIGILANCE",  # T1
        ],
        "ranges": DURATION_RANGES,
    },

    {
        "id":          "IrradiatedDuration",
        "type":        "Suffix",
        "group":       "Group.DebuffDuration",
        "display_tag": "Debuff_Duration_Irradiated",
        "attr":        "Attributes.Debuff.Irradiated.Duration",
        "display_as":  "Flat",
        "decimal":     1,
        "step":        0.5,
        "format":      "{1}s Irradiated Duration",
        "tier_config": TIER_6_25_80,
        "names": [
            "OF TREATMENT",       # T6
            "OF DECONTAMINATION", # T5
            "OF PURIFICATION",    # T4
            "OF CLEANSING",       # T3
            "OF NULLIFICATION",   # T2
            "OF IMMUNITY",        # T1
        ],
        "ranges": DURATION_RANGES,
    },

    {
        "id":          "CorrodedDuration",
        "type":        "Suffix",
        "group":       "Group.DebuffDuration",
        "display_tag": "Debuff_Duration_Corroded",
        "attr":        "Attributes.Debuff.Corroded.Duration",
        "display_as":  "Flat",
        "decimal":     1,
        "step":        0.5,
        "format":      "{1}s Corroded Duration",
        "tier_config": TIER_6_25_80,
        "names": [
            "OF BUFFERING",      # T6
            "OF NEUTRALIZATION", # T5
            "OF RESISTANCE",     # T4
            "OF INERTNESS",      # T3
            "OF NULLIFICATION",  # T2
            "OF CHEMPROOFING",   # T1
        ],
        "ranges": DURATION_RANGES,
    },

    # ── Thermal Insulation (replaces ThermalDestabilization duration) ─────────
    {
        "id":          "ThermalInsulation",
        "type":        "Suffix",
        "group":       "Group.DebuffDuration",
        "display_tag": "Debuff_ThermalInsulation",
        "attr":        "Attributes.Debuff.ThermalInsulation",
        "display_as":  "Percent",
        "decimal":     0,
        "step":        1,
        "format":      "+{1}% Thermal Insulation",
        "tier_config": TIER_6_25_80,
        "names": [
            "OF WARMTH",          # T6
            "OF INSULATION",      # T5
            "OF HEAT-PROOFING",   # T4
            "OF THERMOSTABILITY", # T3
            "OF THERMOPROOFING",  # T2
            "OF THERMOSHIELDING", # T1
        ],
        "ranges": [
            ( 4,  8, 1),  # T6
            ( 9, 15, 1),  # T5
            (16, 22, 1),  # T4
            (23, 31, 1),  # T3
            (32, 41, 1),  # T2
            (42, 50, 1),  # T1
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
    print(f"  Effectiveness ×5:     40 rows  (4-50%, Prefix, 8T)")
    print(f"  Threshold     ×5:     30 rows  (+10-+200 flat, Prefix, 6T)")
    print(f"  Duration      ×4:     24 rows  (-0.5→-3.0s, Suffix, 6T, fixed values)")
    print(f"  ThermalInsulation:     6 rows  (4-50%, Suffix, 6T)")
    print(f"  Total: 100 rows")


if __name__ == "__main__":
    main()
