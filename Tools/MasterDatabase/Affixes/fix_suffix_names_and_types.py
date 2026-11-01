"""
Two fixes in one pass:

1. TYPE — promotes four Batch 7 affixes from Prefix → Suffix:
      CriticalHitResistance, HealthRegeneration,
      DeferredDamageAmount, DeferredDamageTime

2. NAMES — rewrites ItemName for every suffix affix to use "OF X" convention
   (e.g. WARDED → OF WARDING, CALLOUSED → OF GRIT).
   Batch 1 primary-stat suffixes (AddedMight etc.) are already correct and excluded.
"""
import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

COL_AFFIXID  = 1
COL_ITEMNAME = 2
COL_TYPE     = 3
COL_TIER     = 6

# Affixes to flip from Prefix → Suffix
TYPE_TO_SUFFIX = {
    "CriticalHitResistance",
    "HealthRegeneration",
    "DeferredDamageAmount",
    "DeferredDamageTime",
}

# {AffixID: {tier_number: "OF X NAME"}}
SUFFIX_NAMES = {

    # ── Max Resistance (T5 weakest → T1 rarest) ──────────────────────────────
    "MaxPhysicalResistance": {
        5: "OF GRIT",           4: "OF TOUGHENING",
        3: "OF HARDENING",      2: "OF FORTIFICATION",   1: "OF INVULNERABILITY",
    },
    "MaxEnergyResistance": {
        5: "OF INSULATION",     4: "OF TEMPERING",
        3: "OF FIREPROOFING",   2: "OF HEATPROOFING",    1: "OF PYROPROOFING",
    },
    "MaxChemicalResistance": {
        5: "OF TREATMENT",      4: "OF BUFFERING",
        3: "OF NEUTRALIZATION", 2: "OF INERTNESS",       1: "OF CHEMPROOFING",
    },
    "MaxRadiationResistance": {
        5: "OF SCREENING",      4: "OF SHIELDING",
        3: "OF LEAD-LINING",    2: "OF DECONTAMINATION", 1: "OF RADPROOFING",
    },
    "MaxExplosiveResistance": {
        5: "OF PADDING",        4: "OF REINFORCING",
        3: "OF BLAST-DAMPENING",2: "OF BLAST-PROOFING",  1: "OF SHOCKPROOFING",
    },
    "MaxOverheatResistance": {
        5: "OF REGULATION",     4: "OF COOLING",
        3: "OF CONTROL",        2: "OF STABILIZATION",   1: "OF THERMOREGULATION",
    },

    # ── Individual Resistance (T6 weakest → T1 best) ─────────────────────────
    "PhysicalResistance": {
        6: "OF THE ROUGH",   5: "OF CALLOUSNESS",
        4: "OF TOUGHNESS",   3: "OF ARMOR",
        2: "OF PLATING",     1: "OF PHYSICAL",
    },
    "EnergyResistance": {
        6: "OF WARMTH",      5: "OF INSULATION",
        4: "OF TEMPERING",   3: "OF HEAT-TREATING",
        2: "OF FIREPROOFING",1: "OF ENERGY",
    },
    "ChemicalResistance": {
        6: "OF TREATMENT",   5: "OF BUFFERING",
        4: "OF NEUTRALIZATION", 3: "OF PURIFICATION",
        2: "OF INERTNESS",   1: "OF CHEMICAL",
    },
    "RadiationResistance": {
        6: "OF SHIELDING",   5: "OF SCREENING",
        4: "OF LEAD-LINING", 3: "OF DECONTAMINATION",
        2: "OF HARDENING",   1: "OF RADIATION",
    },
    "ExplosiveResistance": {
        6: "OF PADDING",     5: "OF DAMPENING",
        4: "OF REINFORCING", 3: "OF FORTIFYING",
        2: "OF BLAST-PROOFING", 1: "OF EXPLOSIVE",
    },
    "OverheatResistance": {
        6: "OF COOLING",     5: "OF REGULATION",
        4: "OF STABILIZATION", 3: "OF CONTROL",
        2: "OF THERMOSTAT",  1: "OF OVERHEAT",
    },

    # ── Hybrid Resistance (T6 weakest → T1 unique) ───────────────────────────
    "HybridPhysicalEnergyResistance": {
        6: "OF WARDING", 5: "OF DEFENSE", 4: "OF PROTECTION",
        3: "OF GUARDING", 2: "OF SHIELDING", 1: "OF THERMOPLATING",
    },
    "HybridPhysicalChemicalResistance": {
        6: "OF WARDING", 5: "OF DEFENSE", 4: "OF PROTECTION",
        3: "OF GUARDING", 2: "OF SHIELDING", 1: "OF BIOPLATING",
    },
    "HybridPhysicalRadiationResistance": {
        6: "OF WARDING", 5: "OF DEFENSE", 4: "OF PROTECTION",
        3: "OF GUARDING", 2: "OF SHIELDING", 1: "OF RADIOPLATING",
    },
    "HybridPhysicalExplosiveResistance": {
        6: "OF WARDING", 5: "OF DEFENSE", 4: "OF PROTECTION",
        3: "OF GUARDING", 2: "OF SHIELDING", 1: "OF BLASTPLATING",
    },
    "HybridEnergyChemicalResistance": {
        6: "OF WARDING", 5: "OF DEFENSE", 4: "OF PROTECTION",
        3: "OF GUARDING", 2: "OF SHIELDING", 1: "OF THERMOACTIVITY",
    },
    "HybridEnergyRadiationResistance": {
        6: "OF WARDING", 5: "OF DEFENSE", 4: "OF PROTECTION",
        3: "OF GUARDING", 2: "OF SHIELDING", 1: "OF THERMORADIANCE",
    },
    "HybridEnergyExplosiveResistance": {
        6: "OF WARDING", 5: "OF DEFENSE", 4: "OF PROTECTION",
        3: "OF GUARDING", 2: "OF SHIELDING", 1: "OF PYROGUARDING",
    },
    "HybridChemicalRadiationResistance": {
        6: "OF WARDING", 5: "OF DEFENSE", 4: "OF PROTECTION",
        3: "OF GUARDING", 2: "OF SHIELDING", 1: "OF TOXIRADIANCE",
    },
    "HybridChemicalExplosiveResistance": {
        6: "OF WARDING", 5: "OF DEFENSE", 4: "OF PROTECTION",
        3: "OF GUARDING", 2: "OF SHIELDING", 1: "OF VOLATILITY",
    },
    "HybridRadiationExplosiveResistance": {
        6: "OF WARDING", 5: "OF DEFENSE", 4: "OF PROTECTION",
        3: "OF GUARDING", 2: "OF SHIELDING", 1: "OF NUCLEAR BLAST",
    },

    # ── Batch 7 — promoted to Suffix ─────────────────────────────────────────
    "CriticalHitResistance": {
        6: "OF BRACING",      5: "OF COMPOSURE",
        4: "OF STEADINESS",   3: "OF STOICISM",
        2: "OF STEADFASTNESS",1: "OF UNBREAKABILITY",
    },
    "HealthRegeneration": {
        8: "OF RECOVERY",       7: "OF HEALING",
        6: "OF MENDING",        5: "OF RESTORATION",
        4: "OF REGENERATION",   3: "OF REVITALIZATION",
        2: "OF REJUVENATION",   1: "OF RENEWAL",
    },
    "DeferredDamageAmount": {
        4: "OF ABSORPTION",  3: "OF DEFLECTION",
        2: "OF REDISTRIBUTION", 1: "OF DISPERSAL",
    },
    "DeferredDamageTime": {
        4: "OF DELAY",       3: "OF POSTPONEMENT",
        2: "OF DEFERRAL",    1: "OF SUSPENSION",
    },
}


def main():
    if not os.path.exists(WORKBOOK_PATH):
        print(f"ERROR: Could not find workbook at {WORKBOOK_PATH}")
        return

    wb = openpyxl.load_workbook(WORKBOOK_PATH, keep_vba=True)
    ws = wb.active

    type_changes  = 0
    name_changes  = 0

    for row_cells in ws.iter_rows(min_row=2, values_only=False):
        affix_id = row_cells[COL_AFFIXID].value
        if not affix_id:
            continue

        affix_id = str(affix_id)
        tier     = row_cells[COL_TIER].value

        # ── Type change ───────────────────────────────────────────────────────
        if affix_id in TYPE_TO_SUFFIX:
            type_cell = row_cells[COL_TYPE]
            if type_cell.value != "Suffix":
                type_cell.value = "Suffix"
                type_changes += 1

        # ── Name change ───────────────────────────────────────────────────────
        if affix_id in SUFFIX_NAMES and tier in SUFFIX_NAMES[affix_id]:
            name_cell = row_cells[COL_ITEMNAME]
            new_name  = SUFFIX_NAMES[affix_id][tier]
            if name_cell.value != new_name:
                print(f"  {affix_id:45s} T{tier:<3}  {name_cell.value!r:30s} → {new_name!r}")
                name_cell.value = new_name
                name_changes += 1

    wb.save(WORKBOOK_PATH)
    print(f"\nDone — {type_changes} type changes (→ Suffix), {name_changes} name changes (→ OF X).")


if __name__ == "__main__":
    main()
