#!/usr/bin/env python3
# =============================================================================
# POWERSHELL — copy and paste one line at a time
# =============================================================================
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\NodeDataTable\table_to_json.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Might\MightPassiveTree_NodeData.xlsx"
# =============================================================================
"""
table_to_json.py

The final step once a tree is fully managed through NodeDataTable — builds
the JSON UE import file directly from the .xlsx (Nodes + Effects +
Connections sheets). No SVG involved; position comes straight from the
table's X/Y (document-pixel units), already the units NodePosition uses.

Run validate_node_table.py first if you've added/removed/reordered nodes
since the last sync, so tags are correctly numbered and Effects/Connections
references are in sync.

Output: <xlsx name minus "_NodeData">.json next to the workbook — same file
ImportTreeTags\\import_tree_tags.py and the UE importer already expect, so
this is a drop-in replacement for SVGToJSON\\svg_to_skilltree_json.py once
a tree no longer needs the SVG at all.

Usage:
    python table_to_json.py <path_to_xlsx>
"""
import sys
import os
import re
import json
from collections import defaultdict
from openpyxl import load_workbook


def strip_ue_reference(value):
    """Defensive: the table should already store bare paths, but accept a pasted
    '/Script/Engine.Texture2D'/Game/Foo.Foo'' reference too."""
    value = str(value).strip()
    m = re.search(r"'([^']+)'", value)
    return m.group(1) if m else value


def unescape_excel_text(value):
    """Undo export_svg_to_xlsx.py's excel_safe_text() — a leading apostrophe was
    added to keep Excel from reading a leading +/-/=/@ as a formula."""
    if isinstance(value, str) and value.startswith("'"):
        return value[1:]
    return value


def main():
    if len(sys.argv) < 2:
        print("Usage: python table_to_json.py <path_to_xlsx>")
        sys.exit(1)

    xlsx_path = os.path.abspath(sys.argv[1])
    if not os.path.isfile(xlsx_path):
        print(f"Error: file not found: {xlsx_path}")
        sys.exit(1)

    wb = load_workbook(xlsx_path, data_only=True)

    ws_nodes = wb["Nodes"]
    headers = [c.value for c in next(ws_nodes.iter_rows(min_row=1, max_row=1))]
    col = {name: i for i, name in enumerate(headers)}

    nodes_by_tag = {}
    order = []
    warnings = []
    for row in ws_nodes.iter_rows(min_row=2, values_only=True):
        if not row or not row[col["NodeTag"]]:
            continue
        tag = str(row[col["NodeTag"]]).strip()
        if tag in nodes_by_tag:
            warnings.append(f"Duplicate NodeTag '{tag}' in Nodes sheet — later row wins")
        x = row[col["X"]] if "X" in col else None
        y = row[col["Y"]] if "Y" in col else None
        if not isinstance(x, (int, float)) or not isinstance(y, (int, float)):
            warnings.append(f"Node '{tag}' has no numeric X/Y — defaulting to 0,0 "
                             f"(run generate_svg_from_table.py + sync back, or set X/Y directly)")
            x, y = x or 0, y or 0
        is_start = row[col["IsStartNode"]]
        node_desc = unescape_excel_text(row[col["NodeDescription"]]) or ""
        nodes_by_tag[tag] = {
            "NodeTag":           tag,
            "NodeType":          row[col["NodeType"]] or "Minor",
            "IsStartNode":       bool(is_start) if not isinstance(is_start, str) else is_start.strip().lower() == "true",
            "NodeName":          unescape_excel_text(row[col["NodeName"]]) or "",
            "NodeDescription":   node_desc.replace("\\n", "\n"),
            "NodeIcon":          strip_ue_reference(unescape_excel_text(row[col["NodeIcon"]])) if row[col["NodeIcon"]] else "",
            "NodePosition":      {"X": round(float(x), 2), "Y": round(float(y), 2)},
            "ConnectedNodeTags": [],
            "Effects":           [],
        }
        order.append(tag)

    ws_eff = wb["Effects"]
    eheaders = [c.value for c in next(ws_eff.iter_rows(min_row=1, max_row=1))]
    ecol = {name: i for i, name in enumerate(eheaders)}
    eff_rows = [r for r in ws_eff.iter_rows(min_row=2, values_only=True) if r and r[ecol["NodeTag"]]]
    eff_rows.sort(key=lambda r: (str(r[ecol["NodeTag"]]), r[ecol["EffectOrder"]] or 0))

    for row in eff_rows:
        tag = str(row[ecol["NodeTag"]]).strip()
        if tag not in nodes_by_tag:
            warnings.append(f"Effects row references NodeTag '{tag}' not found in Nodes sheet — skipped")
            continue
        effect = {"EffectType": row[ecol["EffectType"]] or "Stat Mod"}
        for key in ("EffectStatModAttributeType", "EffectStatModifierOp",
                    "EffectFlagType", "GrantSkillTag", "EffectSkillPassiveTag"):
            val = unescape_excel_text(row[ecol[key]])
            if val not in (None, ""):
                effect[key] = val
        mag = row[ecol["EffectStatMagnitude"]]
        if mag not in (None, ""):
            effect["EffectStatMagnitude"] = float(mag) if isinstance(mag, (int, float)) else mag
        nodes_by_tag[tag]["Effects"].append(effect)

    connection_count = 0
    if "Connections" in wb.sheetnames:
        ws_conn = wb["Connections"]
        cheaders = [c.value for c in next(ws_conn.iter_rows(min_row=1, max_row=1))]
        ccol = {name: i for i, name in enumerate(cheaders)}
        seen_pairs = set()
        for row in ws_conn.iter_rows(min_row=2, values_only=True):
            if not row or not row[ccol["NodeTagA"]] or not row[ccol["NodeTagB"]]:
                continue
            a = str(row[ccol["NodeTagA"]]).strip()
            b = str(row[ccol["NodeTagB"]]).strip()
            if a not in nodes_by_tag or b not in nodes_by_tag:
                warnings.append(f"Connections row '{a}' <-> '{b}' references a missing NodeTag — skipped")
                continue
            if a == b:
                continue
            pair = tuple(sorted([a, b]))
            if pair in seen_pairs:
                continue
            seen_pairs.add(pair)
            connection_count += 1
            if b not in nodes_by_tag[a]["ConnectedNodeTags"]:
                nodes_by_tag[a]["ConnectedNodeTags"].append(b)
            if a not in nodes_by_tag[b]["ConnectedNodeTags"]:
                nodes_by_tag[b]["ConnectedNodeTags"].append(a)

    output = {"nodes": [nodes_by_tag[t] for t in order]}
    base = os.path.basename(xlsx_path)
    if base.endswith("_NodeData.xlsx"):
        out_name = base[: -len("_NodeData.xlsx")] + ".json"
    else:
        out_name = os.path.splitext(base)[0] + ".json"
    out_path = os.path.join(os.path.dirname(xlsx_path), out_name)

    with open(out_path, "w", encoding="utf-8") as f:
        json.dump(output, f, indent=2, ensure_ascii=False)

    print(f"Done.  {len(order)} node(s), {connection_count} connection(s)")
    print(f"       -> {out_path}")
    for w in warnings:
        print(f"WARNING: {w}")


if __name__ == "__main__":
    main()
