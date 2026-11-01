"""
Scales all Increased* affix Stat1_Min and Stat1_Max values so launcher T1 max reaches 150%.

Original launcher T1 max was 54  →  scale factor = 150 / 54 ≈ 2.778

Run fix_tier_boundaries.py afterwards to clean up any rounding-induced overlaps.
"""
import openpyxl
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

ORIGINAL_LAUNCHER_T1_MAX = 54
TARGET_MAX               = 150
SCALE                    = TARGET_MAX / ORIGINAL_LAUNCHER_T1_MAX  # ~2.7778

COL_AFFIXID  = 1
COL_TIER     = 6
COL_STAT1MIN = 10
COL_STAT1MAX = 11


def main():
    if not os.path.exists(WORKBOOK_PATH):
        print(f"ERROR: Could not find workbook at {WORKBOOK_PATH}")
        return

    wb = openpyxl.load_workbook(WORKBOOK_PATH, keep_vba=True)
    ws = wb.active

    updated = 0
    for row_cells in ws.iter_rows(min_row=2, values_only=False):
        affix_id = row_cells[COL_AFFIXID].value
        if not affix_id or not str(affix_id).startswith("Increased"):
            continue

        min_cell = row_cells[COL_STAT1MIN]
        max_cell = row_cells[COL_STAT1MAX]
        tier     = row_cells[COL_TIER].value

        old_min = min_cell.value
        old_max = max_cell.value

        if not isinstance(old_min, (int, float)) or not isinstance(old_max, (int, float)):
            continue

        # Single-value tiers (min == max) scale together — no special case needed.
        new_min = round(old_min * SCALE)
        new_max = round(old_max * SCALE)

        # Safety: ensure min <= max after rounding
        if new_min > new_max:
            new_min = new_max

        min_cell.value = new_min
        max_cell.value = new_max
        updated += 1
        print(f"  {affix_id:40s} T{tier:<3} {old_min:>3}-{old_max:<3}  →  {new_min:>3}-{new_max}")

    wb.save(WORKBOOK_PATH)
    print(f"\nDone — scaled {updated} rows (×{SCALE:.4f}, launcher T1 max → {TARGET_MAX}%)")
    print("Run fix_tier_boundaries.py next to correct any rounding-induced overlaps.")


if __name__ == "__main__":
    main()
