"""
Adds Group.DebuffDuration (all 5 types) to all 3 weapon pool sheets.
Brings weapons from 4 suffix groups → 5, meeting the minimum threshold.
Rows already present are skipped — safe to re-run.
"""
import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

WEAPON_SHEETS = ["Weapon_Ranged", "Weapon_Melee", "Weapon_Utility"]

DEBUFF_DURATION_ROWS = [
    ("Group.DebuffDuration", "BruisedDuration",    75, ""),
    ("Group.DebuffDuration", "ConcussedDuration",  75, ""),
    ("Group.DebuffDuration", "IrradiatedDuration", 75, ""),
    ("Group.DebuffDuration", "CorrodedDuration",   75, ""),
    ("Group.DebuffDuration", "ThermalInsulation",  75, ""),
]


def main():
    if not os.path.exists(WORKBOOK_PATH):
        print(f"ERROR: Could not find workbook at {WORKBOOK_PATH}")
        return

    wb = openpyxl.load_workbook(WORKBOOK_PATH, keep_vba=True)
    total_added = 0

    for sheet_name in WEAPON_SHEETS:
        if sheet_name not in wb.sheetnames:
            print(f"  MISSING  {sheet_name}")
            continue

        ws = wb[sheet_name]
        existing = {str(r[1]) for r in ws.iter_rows(min_row=2, values_only=True) if r[1]}

        added = 0
        for row in DEBUFF_DURATION_ROWS:
            if row[1] not in existing:
                ws.append(list(row))
                added += 1

        print(f"  {sheet_name:22s}  +{added} row(s)  ({5 - added} already present)")
        total_added += added

    wb.save(WORKBOOK_PATH)
    print(f"\nDone — {total_added} rows added.")


if __name__ == "__main__":
    main()
