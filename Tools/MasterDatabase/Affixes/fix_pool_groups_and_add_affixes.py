"""
Two operations in one pass:

1. FIX resistance GroupTags in all pool sheets (wrong per-type tags → correct shared tags):
      Group.MaxXxxResistance    → Group.MaxResistance
      Group.XxxResistance       → Group.Resistance        (individual, not hybrid/crit)
      Group.HybridXxxResistance → Group.HybridResistance
   (Group.CriticalHitResistance is already correct — left alone)

2. ADD affixes to pool sheets:
      Group.VitalMaxHitPoints (MaxHitPoints, Prefix)
          → all non-weapon sheets
      Group.AmbientTemperature (AmbientTemperature, Prefix)
          → Headwear_Might, Headwear_Agility, Belt
      Group.DebuffThreshold (all 5 types, Prefix)
          → Headwear_Agility, BodyArmour_Agility, Boots_Might, Boots_Agility, Belt
      Group.ReducedDamageFromProjectiles / FromMelee (Prefix)
          → Boots_Agility only  (currently has just 1 prefix — ChanceToDodge)

All additions skip rows already present (matched on AffixID).
"""
import re
import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

NON_WEAPON_SHEETS = [
    "Headwear_Might", "Headwear_Agility",
    "BodyArmour_Might", "BodyArmour_Agility",
    "Gloves_Might", "Gloves_Agility",
    "Boots_Might", "Boots_Agility",
    "Belt",
    "Ring", "Amulet",
]

# ── Resistance GroupTag fix map ────────────────────────────────────────────

def corrected_resistance_group(group_tag):
    """Returns the correct shared group tag, or None if no change needed."""
    if group_tag == "Group.CriticalHitResistance":
        return None  # already correct
    if re.match(r"Group\.Max\w+Resistance$", group_tag):
        return "Group.MaxResistance"
    if re.match(r"Group\.Hybrid\w+Resistance$", group_tag):
        return "Group.HybridResistance"
    if re.match(r"Group\.\w+Resistance$", group_tag):
        return "Group.Resistance"
    return None

# ── New rows to add ────────────────────────────────────────────────────────
# (GroupTag, AffixID, PoolWeight, AffinityTag)

VITAL_MAX_HP = ("Group.VitalMaxHitPoints", "MaxHitPoints", 200, "")

AMBIENT_TEMP = ("Group.AmbientTemperature", "AmbientTemperature", 75, "")

DEBUFF_THRESHOLD_ROWS = [
    ("Group.DebuffThreshold", "BruisedThreshold",                    100, "Physical"),
    ("Group.DebuffThreshold", "ConcussedThreshold",                  100, "Explosive"),
    ("Group.DebuffThreshold", "IrradiatedThreshold",                 100, "Radiation"),
    ("Group.DebuffThreshold", "CorrodedThreshold",                   100, "Chemical"),
    ("Group.DebuffThreshold", "ThermalDestabilizationThreshold",     100, "Energy"),
]

BOOTS_AGILITY_EXTRA = [
    ("Group.ReducedDamageFromProjectiles", "ReducedDamageFromProjectiles", 150, ""),
    ("Group.ReducedDamageFromMelee",       "ReducedDamageFromMelee",       125, ""),
]

# Map: sheet_name → list of rows to add
ADDITIONS = {sheet: [VITAL_MAX_HP] for sheet in NON_WEAPON_SHEETS}

for sheet in ["Headwear_Might", "Headwear_Agility", "Belt"]:
    ADDITIONS[sheet].append(AMBIENT_TEMP)

for sheet in ["Headwear_Agility", "BodyArmour_Agility", "Boots_Might", "Boots_Agility", "Belt"]:
    ADDITIONS[sheet].extend(DEBUFF_THRESHOLD_ROWS)

ADDITIONS["Boots_Agility"].extend(BOOTS_AGILITY_EXTRA)


def process_sheet(ws):
    group_fixes = 0
    # Fix GroupTags in column A (index 0)
    for row_cells in ws.iter_rows(min_row=2, values_only=False):
        cell_group = row_cells[0]
        if not cell_group.value:
            continue
        corrected = corrected_resistance_group(str(cell_group.value))
        if corrected:
            cell_group.value = corrected
            group_fixes += 1
    return group_fixes


def add_rows(ws, rows_to_add):
    existing = set()
    for row in ws.iter_rows(min_row=2, values_only=True):
        if row[1]:
            existing.add(str(row[1]))

    added = 0
    for row in rows_to_add:
        affix_id = row[1]
        if affix_id not in existing:
            ws.append(list(row))
            existing.add(affix_id)
            added += 1
    return added


def main():
    if not os.path.exists(WORKBOOK_PATH):
        print(f"ERROR: Could not find workbook at {WORKBOOK_PATH}")
        return

    wb = openpyxl.load_workbook(WORKBOOK_PATH, keep_vba=True)

    all_sheets = set(wb.sheetnames)
    total_fixes = 0
    total_added = 0

    # Fix GroupTags on every pool sheet (weapons too — they have no resistance rows
    # so this is harmless, but covers them if any get added later)
    all_pool_sheets = [s for s in wb.sheetnames if s != "MasterList"]
    for name in all_pool_sheets:
        fixes = process_sheet(wb[name])
        if fixes:
            print(f"  FIX   {name:22s}  {fixes} GroupTag(s) corrected")
            total_fixes += fixes

    print()

    # Add new rows to non-weapon sheets
    for sheet_name, rows in ADDITIONS.items():
        if sheet_name not in all_sheets:
            print(f"  SKIP  {sheet_name} — sheet not found")
            continue
        added = add_rows(wb[sheet_name], rows)
        print(f"  ADD   {sheet_name:22s}  +{added} row(s)")
        total_added += added

    wb.save(WORKBOOK_PATH)
    print(f"\nDone — {total_fixes} GroupTag fixes, {total_added} rows added.")


if __name__ == "__main__":
    main()
