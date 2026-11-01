#!/usr/bin/env python3
# =============================================================================
# POWERSHELL — copy and paste one line at a time
# =============================================================================
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\NodeDataTable\add_attribute_tags_sheet.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Might\MightPassiveTree_NodeData.xlsx"
# =============================================================================
"""
add_attribute_tags_sheet.py

Adds (or refreshes) an "AttributeTags" reference sheet in a NodeDataTable
workbook, parsed from ..\\AttributeTagMap.txt — every valid tag for
EffectStatModAttributeType, one per row, with its category and description
for context. Point an Excel Data Validation dropdown (Data > Data Validation
> List, source = AttributeTags!$A$2:$A$<last row>) at column A so
EffectStatModAttributeType cells in the Effects sheet can only be set to a
tag that actually exists.

Safe to re-run any time AttributeTagMap.txt changes (new weapon type added,
etc.) — it replaces the AttributeTags sheet wholesale, doesn't touch Nodes/
Effects/Connections.

MetaAttribute.* tags are excluded — AttributeTagMap.txt marks that section
"internal use — avoid in node effects".

Usage:
    python add_attribute_tags_sheet.py <path_to_xlsx>
"""
import sys
import os
import re
from openpyxl import load_workbook

DEFAULT_MAP_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "AttributeTagMap.txt")

SKIP_CATEGORIES = {"META"}


def parse_attribute_tag_map(map_path):
    rows = []
    category = ""
    in_examples = False
    with open(map_path, "r", encoding="utf-8") as f:
        for raw_line in f:
            line = raw_line.rstrip("\n")
            stripped = line.strip()
            if not stripped:
                continue
            if stripped.startswith("===") :
                continue
            if "EFFECT_STAT QUICK EXAMPLES" in stripped:
                in_examples = True
                continue
            if in_examples:
                continue
            if stripped.startswith("---") and stripped.endswith("---"):
                category = stripped.strip("- ").strip()
                continue
            if stripped.startswith("EFFECT_STAT:"):
                continue
            if not category:
                continue  # still in the file's decorative header, before the first "--- CATEGORY ---"

            category_key = category.split()[0] if category else ""
            if category_key in SKIP_CATEGORIES:
                continue

            parts = stripped.split(None, 1)
            tag = parts[0]
            rest = parts[1].strip() if len(parts) > 1 else ""

            op = ""
            m = re.search(r"\(([^)]+)\)\s*$", rest)
            if m:
                op = m.group(1).strip()
                rest = rest[:m.start()].strip()

            rows.append((tag, category, rest, op))
    return rows


def main():
    if len(sys.argv) < 2:
        print("Usage: python add_attribute_tags_sheet.py <path_to_xlsx>")
        sys.exit(1)

    xlsx_path = os.path.abspath(sys.argv[1])
    if not os.path.isfile(xlsx_path):
        print(f"Error: file not found: {xlsx_path}")
        sys.exit(1)

    map_path = os.path.abspath(DEFAULT_MAP_PATH)
    if not os.path.isfile(map_path):
        print(f"Error: AttributeTagMap.txt not found at: {map_path}")
        sys.exit(1)

    rows = parse_attribute_tag_map(map_path)
    if not rows:
        print("Error: parsed zero tags out of AttributeTagMap.txt — check the file format.")
        sys.exit(1)

    dupes = {t for t in (r[0] for r in rows) if [r[0] for r in rows].count(t) > 1}
    if dupes:
        print(f"WARNING: duplicate tag(s) in AttributeTagMap.txt: {sorted(dupes)}")

    try:
        wb = load_workbook(xlsx_path)
    except PermissionError:
        print(f"Error: {xlsx_path} is open in another program (likely Excel). Close it and re-run.")
        sys.exit(1)

    if "AttributeTags" in wb.sheetnames:
        del wb["AttributeTags"]
    ws = wb.create_sheet("AttributeTags")
    ws.append(["AttributeTag", "Category", "Description", "Op"])
    for tag, category, desc, op in rows:
        ws.append([tag, category, desc, op])

    widths = [42, 30, 46, 18]
    for col, w in zip("ABCD", widths):
        ws.column_dimensions[col].width = w
    ws.freeze_panes = "A2"

    try:
        wb.save(xlsx_path)
    except PermissionError:
        print(f"Error: could not save — {xlsx_path} is open in another program (likely Excel). Close it and re-run.")
        sys.exit(1)

    print(f"Done. Wrote {len(rows)} tag(s) to the AttributeTags sheet.")
    print(f"Data Validation source range: AttributeTags!$A$2:$A${len(rows) + 1}")
    print(f"Saved: {xlsx_path}")


if __name__ == "__main__":
    main()
