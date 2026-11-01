"""
Fixes two issues across all affixes in AffixDatabase.xlsm:
  1. Overlapping tier boundaries — tier N+1 min must be strictly greater than tier N max.
     (Skipped when min == max, i.e. single-value tiers like radiation.)
  2. Step values — sets all Stat1_Step to 1.
"""
import openpyxl
import os
from collections import defaultdict

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")

# Column indices (0-based), matching header:
# RowName AffixID ItemName Type GroupTag DisplayTag Tier RequiredItemLevel Weight
# Stat1_Attr Stat1_Min Stat1_Max Stat1_Step Stat1_DecimalCount Stat1_DisplayAs
# Stat2_Attr Stat2_Min Stat2_Max Stat2_Step Stat2_DecimalCount Stat2_DisplayAs DisplayFormat
COL_AFFIXID  = 1
COL_TIER     = 6
COL_STAT1MIN = 10
COL_STAT1MAX = 11
COL_STAT1STP = 12


def main():
    if not os.path.exists(WORKBOOK_PATH):
        print(f"ERROR: Could not find workbook at {WORKBOOK_PATH}")
        return

    wb = openpyxl.load_workbook(WORKBOOK_PATH, keep_vba=True)
    ws = wb.active

    # Gather all data rows as cell-object references so we can write back.
    entries = []
    for row_cells in ws.iter_rows(min_row=2, values_only=False):
        affix_id = row_cells[COL_AFFIXID].value
        if not affix_id:
            continue
        entries.append({
            "affix_id": affix_id,
            "tier":     row_cells[COL_TIER].value,
            "min_cell": row_cells[COL_STAT1MIN],
            "max_cell": row_cells[COL_STAT1MAX],
            "stp_cell": row_cells[COL_STAT1STP],
        })

    # Group by AffixID, sort each group weakest → strongest (highest tier number first).
    groups = defaultdict(list)
    for e in entries:
        groups[e["affix_id"]].append(e)

    steps_fixed    = 0
    overlaps_fixed = 0

    for affix_id, group in sorted(groups.items()):
        group.sort(key=lambda e: e["tier"], reverse=True)  # T-max first, T1 last

        for i, entry in enumerate(group):
            # Fix step
            if entry["stp_cell"].value != 1:
                entry["stp_cell"].value = 1
                steps_fixed += 1

            if i == 0:
                continue

            prev_max = group[i - 1]["max_cell"].value
            curr_min = entry["min_cell"].value
            curr_max = entry["max_cell"].value

            # Skip single-value tiers (radiation) — min == max, no range to shift.
            if curr_min == curr_max:
                continue

            if isinstance(prev_max, (int, float)) and isinstance(curr_min, (int, float)):
                if curr_min <= prev_max:
                    old = curr_min
                    entry["min_cell"].value = int(prev_max) + 1
                    overlaps_fixed += 1
                    print(f"  {affix_id} T{entry['tier']}: min {old} → {entry['min_cell'].value}  (prev tier max was {prev_max})")

    wb.save(WORKBOOK_PATH)
    print(f"\nDone — fixed {overlaps_fixed} overlapping boundaries, {steps_fixed} step values set to 1.")


if __name__ == "__main__":
    main()
