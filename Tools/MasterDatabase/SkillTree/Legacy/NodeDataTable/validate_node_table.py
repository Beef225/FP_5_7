#!/usr/bin/env python3
# =============================================================================
# POWERSHELL — copy and paste one line at a time
# =============================================================================
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\NodeDataTable\validate_node_table.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Might\MightPassiveTree_NodeData.xlsx"
# =============================================================================
"""
validate_node_table.py

Read-only sanity check for a NodeDataTable workbook. Does NOT renumber or
write anything — NodeTag numbering is manual (do it directly in the Nodes
sheet). Exits non-zero if either hard-fail check below trips; both are also
run automatically by generate_svg_from_table.py before it touches the SVG,
so a bad table never gets synced out.

HARD FAILS (exit 1):
  - Two or more rows in the Nodes sheet share the same NodeTag.
  - A NodeTag has two or more Effects-sheet rows with the same EffectOrder
    (ambiguous — can't tell which effect comes first).

WARNINGS (printed, does not fail):
  - An Effects or Connections row references a NodeTag not present in the
    Nodes sheet (dangling reference).

Usage:
  python validate_node_table.py <path_to_xlsx>
"""
import sys
import os
from collections import defaultdict
from openpyxl import load_workbook


def main():
    if len(sys.argv) < 2:
        print("Usage: python validate_node_table.py <path_to_xlsx>")
        sys.exit(1)

    xlsx_path = os.path.abspath(sys.argv[1])
    if not os.path.isfile(xlsx_path):
        print(f"ERROR: file not found: {xlsx_path}")
        sys.exit(1)

    ok = run_validation(xlsx_path, verbose=True)
    sys.exit(0 if ok else 1)


def run_validation(xlsx_path, verbose=False):
    """Returns True if the table passes both hard-fail checks. Used standalone
    and imported by generate_svg_from_table.py as a pre-flight gate."""
    wb = load_workbook(xlsx_path, data_only=True)
    hard_fail = False
    warnings = []

    # ---- Nodes: no duplicate NodeTag ----
    ws_nodes = wb["Nodes"]
    headers = [c.value for c in next(ws_nodes.iter_rows(min_row=1, max_row=1))]
    col = {name: i for i, name in enumerate(headers)}

    tag_rows = defaultdict(list)
    node_count = 0
    for r in range(2, ws_nodes.max_row + 1):
        tag = ws_nodes.cell(row=r, column=col["NodeTag"] + 1).value
        if not tag:
            continue
        tag_rows[str(tag).strip()].append(r)
        node_count += 1

    duplicate_tags = {t: rs for t, rs in tag_rows.items() if len(rs) > 1}
    if duplicate_tags:
        hard_fail = True
        if verbose:
            print("HARD FAIL: duplicate NodeTag in Nodes sheet:")
            for t, rs in duplicate_tags.items():
                print(f"  '{t}' used on rows {rs}")

    final_tags = set(tag_rows.keys())

    # ---- Effects: EffectOrder unique per NodeTag ----
    ws_eff = wb["Effects"] if "Effects" in wb.sheetnames else None
    effect_count = 0
    if ws_eff is not None:
        eheaders = [c.value for c in next(ws_eff.iter_rows(min_row=1, max_row=1))]
        ecol = {name: i for i, name in enumerate(eheaders)}
        order_by_tag = defaultdict(dict)  # tag -> {EffectOrder: [row, ...]}
        for r in range(2, ws_eff.max_row + 1):
            tag = ws_eff.cell(row=r, column=ecol["NodeTag"] + 1).value
            if not tag:
                continue
            tag = str(tag).strip()
            effect_count += 1
            order = ws_eff.cell(row=r, column=ecol["EffectOrder"] + 1).value
            order_by_tag[tag].setdefault(order, []).append(r)
            if tag not in final_tags:
                warnings.append(f"Effects row {r}: NodeTag '{tag}' not found in Nodes sheet")

        dup_order_errors = []
        for tag, orders in order_by_tag.items():
            for order, rows in orders.items():
                if len(rows) > 1:
                    dup_order_errors.append((tag, order, rows))
        if dup_order_errors:
            hard_fail = True
            if verbose:
                print("HARD FAIL: duplicate EffectOrder within the same NodeTag in Effects sheet:")
                for tag, order, rows in dup_order_errors:
                    print(f"  '{tag}' has EffectOrder={order!r} on rows {rows}")

    # ---- Connections: dangling references (warning only) ----
    ws_conn = wb["Connections"] if "Connections" in wb.sheetnames else None
    connection_count = 0
    if ws_conn is not None:
        cheaders = [c.value for c in next(ws_conn.iter_rows(min_row=1, max_row=1))]
        ccol = {name: i for i, name in enumerate(cheaders)}
        for r in range(2, ws_conn.max_row + 1):
            a = ws_conn.cell(row=r, column=ccol["NodeTagA"] + 1).value
            b = ws_conn.cell(row=r, column=ccol["NodeTagB"] + 1).value
            if not a and not b:
                continue
            connection_count += 1
            a = str(a).strip() if a else ""
            b = str(b).strip() if b else ""
            if a and a not in final_tags:
                warnings.append(f"Connections row {r}: NodeTagA '{a}' not found in Nodes sheet")
            if b and b not in final_tags:
                warnings.append(f"Connections row {r}: NodeTagB '{b}' not found in Nodes sheet")

    if verbose:
        if warnings:
            print("WARNINGS (dangling references):")
            for w in warnings:
                print(f"  {w}")
        if not hard_fail:
            print(f"OK — {node_count} node(s), {effect_count} effect row(s), {connection_count} connection row(s). "
                  f"No duplicate NodeTags, no duplicate EffectOrder per tag.")

    return not hard_fail


if __name__ == "__main__":
    main()
