"""
Adds all resistance affixes to every non-weapon pool sheet.

Resistance types added per sheet:
  Max resistances  (6):  MaxPhysical/Energy/Chemical/Radiation/Explosive/Overheat
  Individual       (6):  Physical/Energy/Chemical/Radiation/Explosive/Overheat
  Hybrid           (10): all pairs from {Physical, Energy, Chemical, Radiation, Explosive}

Rows already present (matched on AffixID) are skipped — safe to re-run.

Weight tiers by slot:
  BodyArmour  — individual 200 / max 150 / hybrid 100  (primary defence slot)
  Headwear    — individual 150 / max 100 / hybrid 75
  Boots       — individual 150 / max 100 / hybrid 75
  Gloves      — individual 125 / max  75 / hybrid 75
  Belt        — individual 125 / max  75 / hybrid 75
  Ring/Amulet — individual 100 / max  75 / hybrid 50
"""
import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

# ── Resistance affix definitions ───────────────────────────────────────────
# (GroupTag, AffixID)

MAX_RESISTANCES = [
    ("Group.MaxResistance", "MaxPhysicalResistance"),
    ("Group.MaxResistance", "MaxEnergyResistance"),
    ("Group.MaxResistance", "MaxChemicalResistance"),
    ("Group.MaxResistance", "MaxRadiationResistance"),
    ("Group.MaxResistance", "MaxExplosiveResistance"),
    ("Group.MaxResistance", "MaxOverheatResistance"),
]

INDIVIDUAL_RESISTANCES = [
    ("Group.Resistance", "PhysicalResistance"),
    ("Group.Resistance", "EnergyResistance"),
    ("Group.Resistance", "ChemicalResistance"),
    ("Group.Resistance", "RadiationResistance"),
    ("Group.Resistance", "ExplosiveResistance"),
    ("Group.Resistance", "OverheatResistance"),
]

HYBRID_RESISTANCES = [
    ("Group.HybridResistance", "HybridPhysicalEnergyResistance"),
    ("Group.HybridResistance", "HybridPhysicalChemicalResistance"),
    ("Group.HybridResistance", "HybridPhysicalRadiationResistance"),
    ("Group.HybridResistance", "HybridPhysicalExplosiveResistance"),
    ("Group.HybridResistance", "HybridEnergyChemicalResistance"),
    ("Group.HybridResistance", "HybridEnergyRadiationResistance"),
    ("Group.HybridResistance", "HybridEnergyExplosiveResistance"),
    ("Group.HybridResistance", "HybridChemicalRadiationResistance"),
    ("Group.HybridResistance", "HybridChemicalExplosiveResistance"),
    ("Group.HybridResistance", "HybridRadiationExplosiveResistance"),
]

# ── Weight tiers ───────────────────────────────────────────────────────────
# (individual_w, max_w, hybrid_w)
WEIGHT_TIERS = {
    "BodyArmour_Might":    (200, 150, 100),
    "BodyArmour_Agility":  (200, 150, 100),
    "Headwear_Might":      (150, 100,  75),
    "Headwear_Agility":    (150, 100,  75),
    "Boots_Might":         (150, 100,  75),
    "Boots_Agility":       (150, 100,  75),
    "Gloves_Might":        (125,  75,  75),
    "Gloves_Agility":      (125,  75,  75),
    "Belt":                (125,  75,  75),
    "Ring":                (100,  75,  50),
    "Amulet":              (100,  75,  50),
}


def build_resistance_rows(ind_w, max_w, hyb_w):
    """Returns all 22 resistance rows with the given weights."""
    rows = []
    for group, affix_id in INDIVIDUAL_RESISTANCES:
        rows.append((group, affix_id, ind_w, ""))
    for group, affix_id in MAX_RESISTANCES:
        rows.append((group, affix_id, max_w, ""))
    for group, affix_id in HYBRID_RESISTANCES:
        rows.append((group, affix_id, hyb_w, ""))
    return rows


def main():
    if not os.path.exists(WORKBOOK_PATH):
        print(f"ERROR: Could not find workbook at {WORKBOOK_PATH}")
        return

    wb = openpyxl.load_workbook(WORKBOOK_PATH, keep_vba=True)

    total_added = 0

    for sheet_name, (ind_w, max_w, hyb_w) in WEIGHT_TIERS.items():
        if sheet_name not in wb.sheetnames:
            print(f"  MISSING  {sheet_name} — sheet not found, skipping")
            continue

        ws = wb[sheet_name]

        # Collect AffixIDs already in this sheet (column B = index 2)
        existing_affix_ids = set()
        for row in ws.iter_rows(min_row=2, values_only=True):
            if row[1]:
                existing_affix_ids.add(str(row[1]))

        new_rows = build_resistance_rows(ind_w, max_w, hyb_w)
        added = 0
        for group, affix_id, weight, affinity in new_rows:
            if affix_id not in existing_affix_ids:
                ws.append([group, affix_id, weight, affinity])
                added += 1

        print(f"  {sheet_name:22s}  +{added:2d} rows  ({22 - added} already present)")
        total_added += added

    wb.save(WORKBOOK_PATH)
    print(f"\nDone — {total_added} resistance rows added across {len(WEIGHT_TIERS)} sheets.")


if __name__ == "__main__":
    main()
