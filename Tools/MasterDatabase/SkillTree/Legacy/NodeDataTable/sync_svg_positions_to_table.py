#!/usr/bin/env python3
# =============================================================================
# POWERSHELL — copy and paste one line at a time
# =============================================================================
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\NodeDataTable\sync_svg_positions_to_table.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Might\MightPassiveTree.svg" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Might\MightPassiveTree_NodeData.xlsx"
# =============================================================================
"""
sync_svg_positions_to_table.py

The other half of generate_svg_from_table.py's round trip. Run this after
fine-tuning node positions and/or drawing connector lines in Inkscape:

  - Writes each node's exact current (X, Y) — document-pixel units — into
    its row in the Nodes sheet.
  - Rebuilds the Connections sheet FROM SCRATCH from whatever connector
    paths currently exist in the SVG (the SVG is authoritative for
    connections once you've started drawing them by hand).
  - A node that exists in the SVG but has no row in the Nodes sheet yet
    (you drew it directly in Inkscape instead of adding it to the table
    first) gets a new stub row — NodeTag/X/Y only, everything else blank —
    so nothing is silently lost. Fill in its content in the table
    afterward.
  - A NodeTag that exists in the table but has no matching node in the SVG
    is left alone and reported — it just hasn't been placed yet; the next
    generate_svg_from_table.py run will scatter it near centre for you.

Usage:
  python sync_svg_positions_to_table.py <path_to_svg> <path_to_xlsx>
"""
import sys
import os
import re
import math
from lxml import etree
from openpyxl import load_workbook

SVG_NS = "http://www.w3.org/2000/svg"
INKSCAPE_NS = "http://www.inkscape.org/namespaces/inkscape"


def get_viewbox_scale(root):
    vb = root.get('viewBox', '')
    if not vb:
        return 1.0, 1.0
    parts = re.findall(r'[-+]?(?:\d+\.?\d*|\.\d+)(?:[eE][-+]?\d+)?', vb)
    if len(parts) < 4:
        return 1.0, 1.0
    vb_w, vb_h = float(parts[2]), float(parts[3])

    def dim(attr):
        raw = root.get(attr, '')
        m = re.match(r'([\d.eE+\-]+)', raw)
        return float(m.group(1)) if m else None

    vp_w, vp_h = dim('width'), dim('height')
    if not vb_w or not vb_h or vp_w is None or vp_h is None:
        return 1.0, 1.0
    return vp_w / vb_w, vp_h / vb_h


def parse_transform_matrix(transform_str):
    a, b, c, d, e, f = 1.0, 0.0, 0.0, 1.0, 0.0, 0.0
    if not transform_str:
        return a, b, c, d, e, f

    m = (a, b, c, d, e, f)
    for fn_m in re.finditer(r'(\w+)\s*\(([^)]*)\)', transform_str):
        fn = fn_m.group(1)
        args = [float(x) for x in re.findall(r'[-+]?(?:\d+\.?\d*|\.\d+)(?:[eE][-+]?\d+)?', fn_m.group(2))]
        if fn == 'translate':
            tx = args[0] if args else 0.0
            ty = args[1] if len(args) > 1 else 0.0
            mn = (1.0, 0.0, 0.0, 1.0, tx, ty)
        elif fn == 'scale':
            sx = args[0] if args else 1.0
            sy = args[1] if len(args) > 1 else sx
            mn = (sx, 0.0, 0.0, sy, 0.0, 0.0)
        elif fn == 'matrix':
            if len(args) < 6:
                continue
            mn = tuple(args[:6])
        elif fn == 'rotate':
            ang = math.radians(args[0]) if args else 0.0
            cos_, sin_ = math.cos(ang), math.sin(ang)
            rx = args[1] if len(args) > 1 else 0.0
            ry = args[2] if len(args) > 2 else 0.0
            mn = (cos_, sin_, -sin_, cos_,
                  rx - rx * cos_ + ry * sin_, ry - rx * sin_ - ry * cos_)
        else:
            continue
        a1, b1, c1, d1, e1, f1 = m
        a2, b2, c2, d2, e2, f2 = mn
        m = (a1 * a2 + c1 * b2, b1 * a2 + d1 * b2,
             a1 * c2 + c1 * d2, b1 * c2 + d1 * d2,
             a1 * e2 + c1 * f2 + e1, b1 * e2 + d1 * f2 + f1)
    return m


def apply_matrix(m, x, y):
    a, b, c, d, e, f = m
    return a * x + c * y + e, b * x + d * y + f


def resolve_absolute_position(elem, parent_map):
    """Walk from the element up through every ancestor, applying each level's
    FULL transform (not just its translation) to the running point in turn —
    matches SVG's actual nested-transform composition. A naive "element's own
    transform + sum of ancestor translations" (the old approach here) only
    works when every ancestor is a pure translate; it silently produces wrong
    positions the moment a rotate/scale/skew lands on a wrapping <g> (e.g.
    grouping nodes in Inkscape and rotating the group)."""
    tag = etree.QName(elem).localname
    if tag in ('circle', 'ellipse'):
        x = float(elem.get('cx', 0) or 0)
        y = float(elem.get('cy', 0) or 0)
    else:
        x, y = 0.0, 0.0
    node = elem
    while node is not None:
        m = parse_transform_matrix(node.get('transform', ''))
        x, y = apply_matrix(m, x, y)
        node = parent_map.get(node)
    return x, y


def main():
    if len(sys.argv) < 3:
        print("Usage: python sync_svg_positions_to_table.py <path_to_svg> <path_to_xlsx>")
        sys.exit(1)

    svg_path = os.path.abspath(sys.argv[1])
    xlsx_path = os.path.abspath(sys.argv[2])
    if not os.path.isfile(svg_path):
        print(f"Error: SVG not found: {svg_path}")
        sys.exit(1)
    if not os.path.isfile(xlsx_path):
        print(f"Error: table not found: {xlsx_path}")
        sys.exit(1)

    tree = etree.parse(svg_path)
    root = tree.getroot()
    vb_sx, vb_sy = get_viewbox_scale(root)

    parent_map = {}
    for parent in root.iter():
        for child in parent:
            parent_map[child] = parent

    id_to_elem = {}
    svg_pos_by_tag = {}   # tag -> (x_px, y_px)
    tag_by_id = {}
    for elem in root.iter():
        eid = elem.get('id')
        if eid:
            id_to_elem[eid] = elem
        desc_el = elem.find(f"{{{SVG_NS}}}desc")
        if desc_el is None or not desc_el.text:
            continue
        text = desc_el.text.strip()
        if not text.startswith("NodeTag:") or not eid:
            continue
        tag = text.splitlines()[0].split(":", 1)[1].strip()
        x, y = resolve_absolute_position(elem, parent_map)
        svg_pos_by_tag[tag] = (round(x * vb_sx, 2), round(y * vb_sy, 2))
        tag_by_id[eid] = tag

    def resolve_to_tag(svg_id):
        if svg_id in tag_by_id:
            return tag_by_id[svg_id]
        elem = id_to_elem.get(svg_id)
        if elem is None:
            return None
        current = parent_map.get(elem)
        while current is not None:
            cid = current.get('id', '')
            if cid in tag_by_id:
                return tag_by_id[cid]
            current = parent_map.get(current)
        return None

    connections = []
    seen_pairs = set()
    for elem in root.iter():
        if etree.QName(elem).localname != 'path':
            continue
        start_ref = elem.get(f'{{{INKSCAPE_NS}}}connection-start', '')
        end_ref = elem.get(f'{{{INKSCAPE_NS}}}connection-end', '')
        if not start_ref or not end_ref:
            continue
        a = resolve_to_tag(start_ref.lstrip('#'))
        b = resolve_to_tag(end_ref.lstrip('#'))
        if not a or not b or a == b:
            continue
        pair = tuple(sorted([a, b]))
        if pair in seen_pairs:
            continue
        seen_pairs.add(pair)
        connections.append(pair)

    # ---- update the workbook ----
    wb = load_workbook(xlsx_path)
    ws_nodes = wb["Nodes"]
    headers = [c.value for c in next(ws_nodes.iter_rows(min_row=1, max_row=1))]
    col = {name: i + 1 for i, name in enumerate(headers)}  # 1-based

    existing_rows = {}
    for r in range(2, ws_nodes.max_row + 1):
        tag = ws_nodes.cell(row=r, column=col["NodeTag"]).value
        if tag:
            existing_rows[str(tag).strip()] = r

    updated = 0
    stub_added = 0
    for tag, (x, y) in svg_pos_by_tag.items():
        if tag in existing_rows:
            r = existing_rows[tag]
            ws_nodes.cell(row=r, column=col["X"]).value = x
            ws_nodes.cell(row=r, column=col["Y"]).value = y
            updated += 1
        else:
            new_row = [None] * len(headers)
            new_row[col["NodeTag"] - 1] = tag
            new_row[col["NodeType"] - 1] = "Minor"
            new_row[col["IsStartNode"] - 1] = False
            new_row[col["X"] - 1] = x
            new_row[col["Y"] - 1] = y
            ws_nodes.append(new_row)
            stub_added += 1

    not_yet_placed = sorted(set(existing_rows) - set(svg_pos_by_tag))

    # rebuild Connections sheet from scratch
    if "Connections" not in wb.sheetnames:
        ws_conn = wb.create_sheet("Connections")
        ws_conn.append(["NodeTagA", "NodeTagB"])
        ws_conn.column_dimensions["A"].width = 42
        ws_conn.column_dimensions["B"].width = 42
        ws_conn.freeze_panes = "A2"
    else:
        ws_conn = wb["Connections"]
        for r in range(ws_conn.max_row, 1, -1):
            ws_conn.delete_rows(r)
    for a, b in connections:
        ws_conn.append([a, b])

    wb.save(xlsx_path)

    print(f"Updated position for {updated} existing node(s).")
    if stub_added:
        print(f"Added {stub_added} stub row(s) for node(s) drawn in Inkscape but missing from the table "
              f"— fill in their content:")
        for tag in svg_pos_by_tag:
            if tag not in existing_rows:
                print(f"  {tag}")
    print(f"Rebuilt Connections sheet: {len(connections)} connection(s).")
    if not_yet_placed:
        print(f"{len(not_yet_placed)} table row(s) have no node in the SVG yet (not placed/generated):")
        for tag in not_yet_placed[:20]:
            print(f"  {tag}")
        if len(not_yet_placed) > 20:
            print(f"  ... and {len(not_yet_placed) - 20} more")
    print(f"Saved: {xlsx_path}")


if __name__ == "__main__":
    main()
