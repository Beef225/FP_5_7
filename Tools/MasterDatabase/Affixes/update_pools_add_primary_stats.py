"""
Adds the 4 primary stat suffixes to every pool sheet.
Rows already present (matched on AffixID) are skipped — safe to re-run.
"""
import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

PRIMARY_STATS = [
    ("Group.PrimaryMight",     "AddedMight",      150, ""),
    ("Group.PrimaryResonance", "AddedResonance",  150, ""),
    ("Group.PrimaryAgility",   "AddedAgility",    150, ""),
    ("Group.PrimaryFortitude", "AddedFortitude",  150, ""),
]

ALL_POOL_SHEETS = [
    "Weapon_Ranged", "Weapon_Melee", "Weapon_Utility",
    "Headwear_Might", "Headwear_Agility",
    "BodyArmour_Might", "BodyArmour_Agility",
    "Gloves_Might", "Gloves_Agility",
    "Boots_Might", "Boots_Agility",
    "Belt",
    "Ring", "Amulet",
]


def main():
    if not os.path.exists(WORKBOOK_PATH):
        print(f"ERROR: Could not find workbook at {WORKBOOK_PATH}")
        return

    wb = openpyxl.load_workbook(WORKBOOK_PATH, keep_vba=True)
    total_added = 0

    for sheet_name in ALL_POOL_SHEETS:
        if sheet_name not in wb.sheetnames:
            print(f"  MISSING  {sheet_name}")
            continue

        ws = wb[sheet_name]
        existing = {str(row[1]) for row in ws.iter_rows(min_row=2, values_only=True) if row[1]}

        added = 0
        for row in PRIMARY_STATS:
            if row[1] not in existing:
                ws.append(list(row))
                added += 1

        print(f"  {sheet_name:22s}  +{added} row(s)  ({4 - added} already present)")
        total_added += added

    wb.save(WORKBOOK_PATH)
    print(f"\nDone — {total_added} rows added across {len(ALL_POOL_SHEETS)} sheets.")


if __name__ == "__main__":
    main()
