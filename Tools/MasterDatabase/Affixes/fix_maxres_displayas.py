"""
Fixes MaxXxxResistance rows: sets Stat1_DisplayAs from "Flat" to "Percent".
Stored values remain 1-10; engine divides by 100 → 0.01-0.10 applied to cap.
Display: +1% to +10% Maximum X Resistance (0 decimals).
"""
import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

COL_AFFIXID      = 1
COL_STAT1DISPLAY = 14  # Stat1_DisplayAs


def main():
    if not os.path.exists(WORKBOOK_PATH):
        print(f"ERROR: Could not find workbook at {WORKBOOK_PATH}")
        return

    wb = openpyxl.load_workbook(WORKBOOK_PATH, keep_vba=True)
    ws = wb.active

    fixed = 0
    for row_cells in ws.iter_rows(min_row=2, values_only=False):
        affix_id = row_cells[COL_AFFIXID].value
        if not affix_id or not str(affix_id).startswith("Max"):
            continue
        if not str(affix_id).endswith("Resistance"):
            continue

        cell = row_cells[COL_STAT1DISPLAY]
        if cell.value != "Percent":
            cell.value = "Percent"
            fixed += 1
            print(f"  {affix_id}")

    wb.save(WORKBOOK_PATH)
    print(f"\nDone — updated {fixed} rows to DisplayAs=Percent.")


if __name__ == "__main__":
    main()
