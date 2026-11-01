#!/usr/bin/env python3
# =============================================================================
# POWERSHELL — copy and paste one line at a time
# =============================================================================
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\NodeDataTable\export_svg_to_xlsx.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Might\MightPassiveTree.svg"
# =============================================================================
"""
export_svg_to_xlsx.py

Reads every node's Inkscape description block, position, and connector out of
a passive/skill tree SVG and writes it to a three-sheet .xlsx workbook next
to the SVG:

  Nodes sheet       — one row per node (NodeTag, NodeType, NodeName,
                       NodeDescription, NodeIcon, IsStartNode, X, Y).
                       X/Y are in document-pixel units (0-3000 on the
                       standard canvas) — the same units the final JSON's
                       NodePosition uses.
  Effects sheet      — one row per effect (NodeTag, EffectOrder, EffectType,
                       and whichever of EffectStatModAttributeType/Op/
                       Magnitude, EffectFlagType, GrantSkillTag,
                       EffectSkillPassiveTag apply). A node with 2 effects
                       gets 2 rows sharing its NodeTag.
  Connections sheet  — one row per connector edge (NodeTagA, NodeTagB).

Run this ONCE to bootstrap the table from an existing SVG, or any time you
want to re-flatten the SVG's current state into the table (this OVERWRITES
the .xlsx — export before you have un-synced table edits you care about
losing; if you've been editing content/position/connections in the table
since the last sync, use sync_svg_positions_to_table.py instead so you don't
lose the position/connection *edits already made in Inkscape*).

See NodeDataTable_howto.txt for the full round-trip workflow.
"""
import sys
import os
import math
import re
from lxml import etree
from openpyxl import Workbook

SVG_NS = "http://www.w3.org/2000/svg"
INKSCAPE_NS = "http://www.inkscape.org/namespaces/inkscape"
SODIPODI_NS = "http://sodipodi.sourceforge.net/DTD/sodipodi-0.0.dtd"


def strip_ue_reference(value):
    """'/Script/Engine.Texture2D'/Game/Foo.Foo'' -> '/Game/Foo.Foo' — the table stores the bare path."""
    value = value.strip()
    m = re.search(r"'([^']+)'", value)
    return m.group(1) if m else value


# Excel (General format) reads a leading +, -, =, or @ as "this cell is a
# formula" — most commonly hit on NodeDescription values like "+2 Armour
# Penetration". Prefixing with an apostrophe is the standard Excel escape:
# it forces text and Excel hides the apostrophe in the cell display. The
# read side (table_to_json.py / svg_to_skilltree_json.py's load_node_table)
# strips it back off.
FORMULA_TRIGGER_CHARS = ("+", "-", "=", "@")


def excel_safe_text(value):
    if isinstance(value, str) and value.startswith(FORMULA_TRIGGER_CHARS):
        return "'" + value
    return value

NODE_HEADERS = ["NodeTag", "NodeType", "NodeName", "NodeDescription", "NodeIcon", "IsStartNode", "X", "Y"]
EFFECT_HEADERS = [
    "NodeTag", "EffectOrder", "EffectType",
    "EffectStatModAttributeType", "EffectStatModifierOp", "EffectStatMagnitude",
    "EffectFlagType", "GrantSkillTag", "EffectSkillPassiveTag",
]
CONNECTION_HEADERS = ["NodeTagA", "NodeTagB"]


def parse_description(text):
    node = {
        "NodeTag": "", "NodeType": "Minor", "IsStartNode": False,
        "NodeName": "", "NodeDescription": "", "NodeIcon": "",
        "Effects": [],
    }
    current = None
    last_key = None
    for raw_line in text.splitlines():
        line = raw_line.strip()
        if not line:
            last_key = None
            continue
        if ":" not in line:
            # A line with no "Key:" is a continuation of a real (not "\n"-escaped)
            # embedded newline inside the previous field's value — only
            # NodeDescription supports multi-line content today. Anything else
            # with no recognized key is genuinely unparseable and dropped, same
            # as before.
            if last_key == "NodeDescription":
                node["NodeDescription"] += "\n" + line
            continue
        key, _, val = line.partition(":")
        key, val = key.strip(), val.strip()
        last_key = key
        if key == "NodeTag":
            node["NodeTag"] = val
        elif key == "NodeType":
            node["NodeType"] = val
        elif key == "IsStartNode":
            node["IsStartNode"] = val.lower() == "true"
        elif key == "NodeName":
            node["NodeName"] = val
        elif key == "NodeDescription":
            node["NodeDescription"] = val.replace("\\n", "\n")
        elif key == "NodeIcon":
            node["NodeIcon"] = strip_ue_reference(val) if val else ""
        elif key == "EffectType":
            if current is not None:
                node["Effects"].append(current)
            current = {"EffectType": val}
        elif current is not None:
            if not val:
                continue
            if key in ("EffectSkillPassiveTag", "EffectFlagType", "GrantSkillTag",
                       "EffectStatModAttributeType", "EffectStatModifierOp"):
                current[key] = val
            elif key == "EffectStatMagnitude":
                try:
                    current[key] = float(val)
                except ValueError:
                    current[key] = val
    if current is not None:
        node["Effects"].append(current)
    return node if node["NodeTag"] else None


# ---------------------------------------------------------------------------
# Position helpers (mirrors SVGToJSON\svg_to_skilltree_json.py)
# ---------------------------------------------------------------------------

def parse_transform_matrix(transform_str):
    a, b, c, d, e, f = 1.0, 0.0, 0.0, 1.0, 0.0, 0.0
    if not transform_str:
        return a, b, c, d, e, f

    def mul(m1, m2):
        a1, b1, c1, d1, e1, f1 = m1
        a2, b2, c2, d2, e2, f2 = m2
        return (
            a1 * a2 + c1 * b2, b1 * a2 + d1 * b2,
            a1 * c2 + c1 * d2, b1 * c2 + d1 * d2,
            a1 * e2 + c1 * f2 + e1, b1 * e2 + d1 * f2 + f1,
        )

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


def get_viewbox_scale(root):
    vb = root.get('viewBox', '')
    if not vb:
        return 1.0, 1.0
    parts = re.findall(r'[-+]?(?:\d+\.?\d*|\.\d+)(?:[eE][-+]?\d+)?', vb)
    if len(parts) < 4:
        return 1.0, 1.0
    vb_w, vb_h = float(parts[2]), float(parts[3])
    if vb_w == 0 or vb_h == 0:
        return 1.0, 1.0

    def dim(attr):
        raw = root.get(attr, '')
        m = re.match(r'([\d.eE+\-]+)', raw)
        return float(m.group(1)) if m else None

    vp_w, vp_h = dim('width'), dim('height')
    if vp_w is None or vp_h is None:
        return 1.0, 1.0
    return vp_w / vb_w, vp_h / vb_h


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
    elif tag == 'rect':
        rx = float(elem.get('x', 0) or 0)
        ry = float(elem.get('y', 0) or 0)
        w = float(elem.get('width', 0) or 0)
        h = float(elem.get('height', 0) or 0)
        x, y = rx + w / 2, ry + h / 2
    else:
        x, y = 0.0, 0.0
    node = elem
    while node is not None:
        m = parse_transform_matrix(node.get('transform', ''))
        x, y = apply_matrix(m, x, y)
        node = parent_map.get(node)
    return x, y


def main():
    force = "--force" in sys.argv
    args = [a for a in sys.argv[1:] if a != "--force"]
    if len(args) < 1:
        print("Usage: python export_svg_to_xlsx.py <path_to_svg> [--force]")
        sys.exit(1)

    svg_path = os.path.abspath(args[0])
    if not os.path.isfile(svg_path):
        print(f"Error: file not found: {svg_path}")
        sys.exit(1)

    out_path = os.path.splitext(svg_path)[0] + "_NodeData.xlsx"
    if os.path.isfile(out_path) and not force:
        # If descriptions are already minimal ("NodeTag:X" only), the SVG is no
        # longer the source of truth for content — this tree has migrated to the
        # table workflow, and overwriting the table from the SVG would DESTROY
        # NodeName/NodeDescription/Effects that only live in the table anymore.
        # (Use sync_svg_positions_to_table.py instead — it only touches
        # position/connections and never touches content columns.)
        check_tree = etree.parse(svg_path)
        minimal, total = 0, 0
        for elem in check_tree.getroot().iter():
            desc_el = elem.find(f"{{{SVG_NS}}}desc")
            if desc_el is None or not desc_el.text:
                continue
            text = desc_el.text.strip()
            if not text.startswith("NodeTag:"):
                continue
            total += 1
            if len(text.splitlines()) <= 1:
                minimal += 1
        if total > 0 and minimal / total > 0.5:
            print(f"REFUSING TO RUN: an existing table was found at:\n  {out_path}")
            print(f"and {minimal}/{total} node descriptions in the SVG are already minimal "
                  f"(\"NodeTag:X\" only) — this tree has migrated to the table workflow, and the "
                  f"SVG no longer holds NodeName/NodeDescription/Effects content.")
            print("Overwriting the table from this SVG would DESTROY that content.")
            print()
            print("If you're fine-tuning position/connections in Inkscape, run instead:")
            print("  sync_svg_positions_to_table.py <svg> <xlsx>")
            print()
            print("If you really mean to re-bootstrap the table from the SVG (discarding")
            print("table content), re-run this with --force.")
            sys.exit(1)

    tree = etree.parse(svg_path)
    root = tree.getroot()
    vb_sx, vb_sy = get_viewbox_scale(root)

    parent_map = {}
    for parent in root.iter():
        for child in parent:
            parent_map[child] = parent

    id_to_elem = {}
    nodes = []
    node_by_id = {}
    for elem in root.iter():
        eid = elem.get('id')
        if eid:
            id_to_elem[eid] = elem
        desc_el = elem.find(f"{{{SVG_NS}}}desc")
        if desc_el is None or not desc_el.text:
            continue
        text = desc_el.text.strip()
        if not text.startswith("NodeTag:"):
            continue
        node = parse_description(text)
        if not node or not eid:
            continue
        x, y = resolve_absolute_position(elem, parent_map)
        node["X"] = round(x * vb_sx, 2)
        node["Y"] = round(y * vb_sy, 2)
        nodes.append(node)
        node_by_id[eid] = node["NodeTag"]

    def resolve_to_tag(svg_id):
        if svg_id in node_by_id:
            return node_by_id[svg_id]
        elem = id_to_elem.get(svg_id)
        if elem is None:
            return None
        current = parent_map.get(elem)
        while current is not None:
            cid = current.get('id', '')
            if cid in node_by_id:
                return node_by_id[cid]
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

    wb = Workbook()
    ws_nodes = wb.active
    ws_nodes.title = "Nodes"
    ws_nodes.append(NODE_HEADERS)
    ws_effects = wb.create_sheet("Effects")
    ws_effects.append(EFFECT_HEADERS)
    ws_conn = wb.create_sheet("Connections")
    ws_conn.append(CONNECTION_HEADERS)

    for n in nodes:
        ws_nodes.append([
            n["NodeTag"], n["NodeType"], excel_safe_text(n["NodeName"]), excel_safe_text(n["NodeDescription"]),
            excel_safe_text(n["NodeIcon"]), n["IsStartNode"], n["X"], n["Y"],
        ])
        for i, eff in enumerate(n["Effects"], start=1):
            ws_effects.append([
                n["NodeTag"], i, eff.get("EffectType", ""),
                excel_safe_text(eff.get("EffectStatModAttributeType", "")),
                eff.get("EffectStatModifierOp", ""),
                eff.get("EffectStatMagnitude", ""),
                excel_safe_text(eff.get("EffectFlagType", "")),
                excel_safe_text(eff.get("GrantSkillTag", "")),
                excel_safe_text(eff.get("EffectSkillPassiveTag", "")),
            ])
    for a, b in connections:
        ws_conn.append([a, b])

    widths_nodes = [42, 10, 26, 40, 70, 12, 10, 10]
    for col, w in zip("ABCDEFGH", widths_nodes):
        ws_nodes.column_dimensions[col].width = w
    widths_effects = [42, 11, 12, 34, 18, 16, 20, 24, 26]
    for col, w in zip("ABCDEFGHI", widths_effects):
        ws_effects.column_dimensions[col].width = w
    ws_conn.column_dimensions["A"].width = 42
    ws_conn.column_dimensions["B"].width = 42

    ws_nodes.freeze_panes = "A2"
    ws_effects.freeze_panes = "A2"
    ws_conn.freeze_panes = "A2"

    out_path = os.path.splitext(svg_path)[0] + "_NodeData.xlsx"
    wb.save(out_path)

    total_effects = sum(len(n["Effects"]) for n in nodes)
    print(f"Done.  {len(nodes)} node(s), {total_effects} effect row(s), {len(connections)} connection(s)")
    print(f"       -> {out_path}")


if __name__ == "__main__":
    main()
