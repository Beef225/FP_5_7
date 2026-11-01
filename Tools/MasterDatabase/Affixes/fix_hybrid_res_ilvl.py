"""
Rebalances hybrid resistance tier ilvl requirements to start at 36, cap at 85.

New TIER_CONFIG_HYBRID6:
  T6 ilvl 36  weight  50
  T5 ilvl 46  weight 100
  T4 ilvl 57  weight 200
  T3 ilvl 67  weight 300
  T2 ilvl 76  weight 200
  T1 ilvl 85  weight 100
"""
import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

COL_AFFIXID = 1
COL_TIER    = 6
COL_ILVL    = 7
COL_WEIGHT  = 8

NEW_CONFIG = {
    6: (36,  50),
    5: (46, 100),
    4: (57, 200),
    3: (67, 300),
    2: (76, 200),
    1: (85, 100),
}


def main():
    if not os.path.exists(WORKBOOK_PATH):
        print(f"ERROR: Could not find workbook at {WORKBOOK_PATH}")
        return

    wb = openpyxl.load_workbook(WORKBOOK_PATH, keep_vba=True)
    ws = wb.active

    updated = 0
    for row_cells in ws.iter_rows(min_row=2, values_only=False):
        affix_id = row_cells[COL_AFFIXID].value
        if not affix_id or not str(affix_id).startswith("Hybrid"):
            continue

        tier = row_cells[COL_TIER].value
        if tier not in NEW_CONFIG:
            continue

        new_ilvl, new_weight = NEW_CONFIG[tier]
        row_cells[COL_ILVL].value   = new_ilvl
        row_cells[COL_WEIGHT].value = new_weight
        updated += 1
        print(f"  {affix_id:50s} T{tier}  ilvl={new_ilvl}  weight={new_weight}")

    wb.save(WORKBOOK_PATH)
    print(f"\nDone — updated {updated} hybrid resistance rows.")


if __name__ == "__main__":
    main()
