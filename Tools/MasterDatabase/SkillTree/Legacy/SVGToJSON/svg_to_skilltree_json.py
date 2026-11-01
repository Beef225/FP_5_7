#!/usr/bin/env python3
# =============================================================================
# POWERSHELL — copy and paste one line at a time
# =============================================================================
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\SVGToJSON\svg_to_skilltree_json.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Might\MightPassiveTree.svg"
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\SVGToJSON\svg_to_skilltree_json.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Resonance\ResonancePassiveTree.svg"
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\SVGToJSON\svg_to_skilltree_json.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Agility\AgilityPassiveTree.svg"
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\SVGToJSON\svg_to_skilltree_json.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Fortitude\FortitudePassiveTree.svg"
#
# With the optional .xlsx node-data table (see Tools\MasterDatabase\SkillTree\NodeDataTable):
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\SVGToJSON\svg_to_skilltree_json.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Might\MightPassiveTree.svg" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Might\MightPassiveTree_NodeData.xlsx"
# =============================================================================
"""
svg_to_skilltree_json.py
Reads an Inkscape SVG skill tree and writes a JSON file to the same folder.

Node data   — read from the <desc> element on each shape (Object Properties -> Description),
              UNLESS a .xlsx node-data table is passed, in which case the table is the
              source of truth for any NodeTag it defines (see NodeDataTable\\README below).
Connections — read from Inkscape connector paths (inkscape:connection-start/end).
Position    — read from each shape's canvas centre, in document pixel coordinates.

Usage:
    python svg_to_skilltree_json.py path/to/tree.svg
    python svg_to_skilltree_json.py path/to/tree.svg path/to/tree_NodeData.xlsx

NodeDataTable workflow (recommended for heavy content-editing passes):
    1. NodeDataTable\\export_svg_to_xlsx.py <svg>   — bootstrap/refresh the table
       from the SVG's current inline descriptions (one-time, or whenever you want
       to flatten fresh inline edits into the table).
    2. Simplify each migrated node's SVG description down to just "NodeTag:X" —
       Inkscape is now only responsible for position and connections.
    3. Edit NodeName/NodeDescription/NodeIcon/Effects in the .xlsx instead —
       filter, sort, bulk-edit magnitudes, whatever Excel is good at.
    4. Run this script with the .xlsx as the second argument to merge them back
       into one JSON. Nodes not yet migrated (still fully inline in the SVG)
       keep working as before — migrate incrementally, no big-bang required.
"""

import sys
import os
import json
import math
import re
import xml.etree.ElementTree as ET

INKSCAPE_NS = 'http://www.inkscape.org/namespaces/inkscape'
SVG_NS      = 'http://www.w3.org/2000/svg'
SODIPODI_NS = 'http://sodipodi.sourceforge.net/DTD/sodipodi-0.0.dtd'


# ---------------------------------------------------------------------------
# Optional .xlsx node-data table (NodeDataTable\export_svg_to_xlsx.py)
#
# When a table is passed on the command line, it is the source of truth for
# node CONTENT (NodeType/NodeName/NodeDescription/NodeIcon/IsStartNode/
# Effects) for any NodeTag it defines. The SVG remains the source of truth
# for POSITION and CONNECTIONS regardless. A node whose tag isn't in the
# table falls back to its inline SVG description — this is what makes
# incremental migration possible (simplify SVG descriptions to "NodeTag:X"
# as you move each node's content into the table; nodes you haven't
# migrated yet keep working from their full inline description).
# ---------------------------------------------------------------------------

def unescape_excel_text(value):
    """Undo export_svg_to_xlsx.py's excel_safe_text() — a leading apostrophe was
    added to keep Excel from reading a leading +/-/=/@ as a formula."""
    if isinstance(value, str) and value.startswith("'"):
        return value[1:]
    return value


def load_node_table(xlsx_path):
    from openpyxl import load_workbook
    wb = load_workbook(xlsx_path, data_only=True)

    ws_nodes = wb['Nodes']
    headers = [c.value for c in next(ws_nodes.iter_rows(min_row=1, max_row=1))]
    col = {name: i for i, name in enumerate(headers)}

    table = {}
    for row in ws_nodes.iter_rows(min_row=2, values_only=True):
        if row is None or not row[col['NodeTag']]:
            continue
        tag = str(row[col['NodeTag']]).strip()
        is_start = row[col['IsStartNode']]
        node_desc = unescape_excel_text(row[col['NodeDescription']]) or ''
        table[tag] = {
            'NodeTag':         tag,
            'NodeType':        (row[col['NodeType']] or 'Minor'),
            'IsStartNode':     bool(is_start) if not isinstance(is_start, str) else is_start.strip().lower() == 'true',
            'NodeName':        unescape_excel_text(row[col['NodeName']]) or '',
            'NodeDescription': node_desc.replace('\\n', '\n'),
            'NodeIcon':        strip_ue_reference(unescape_excel_text(row[col['NodeIcon']])) if row[col['NodeIcon']] else '',
            'Effects':         [],
        }

    ws_eff = wb['Effects']
    eheaders = [c.value for c in next(ws_eff.iter_rows(min_row=1, max_row=1))]
    ecol = {name: i for i, name in enumerate(eheaders)}

    rows = list(ws_eff.iter_rows(min_row=2, values_only=True))
    rows = [r for r in rows if r and r[ecol['NodeTag']]]
    rows.sort(key=lambda r: (str(r[ecol['NodeTag']]), r[ecol['EffectOrder']] or 0))

    for row in rows:
        tag = str(row[ecol['NodeTag']]).strip()
        if tag not in table:
            continue
        effect = {'EffectType': row[ecol['EffectType']] or 'Stat Mod'}
        for key in ('EffectStatModAttributeType', 'EffectStatModifierOp',
                    'EffectFlagType', 'GrantSkillTag', 'EffectSkillPassiveTag'):
            val = unescape_excel_text(row[ecol[key]])
            if val not in (None, ''):
                effect[key] = val
        mag = row[ecol['EffectStatMagnitude']]
        if mag not in (None, ''):
            effect['EffectStatMagnitude'] = float(mag) if isinstance(mag, (int, float)) else mag
        table[tag]['Effects'].append(effect)

    return table

# ---------------------------------------------------------------------------
# SVG transform helpers
# ---------------------------------------------------------------------------

def parse_transform_matrix(transform_str):
    """
    Parse an SVG transform string into a 2D affine matrix (a,b,c,d,e,f).
    Handles translate, scale, matrix, and rotate (chained in any order).
    """
    a, b, c, d, e, f = 1.0, 0.0, 0.0, 1.0, 0.0, 0.0
    if not transform_str:
        return a, b, c, d, e, f

    def mul(m1, m2):
        a1, b1, c1, d1, e1, f1 = m1
        a2, b2, c2, d2, e2, f2 = m2
        return (
            a1*a2 + c1*b2,  b1*a2 + d1*b2,
            a1*c2 + c1*d2,  b1*c2 + d1*d2,
            a1*e2 + c1*f2 + e1,  b1*e2 + d1*f2 + f1,
        )

    m = (a, b, c, d, e, f)
    for fn_m in re.finditer(r'(\w+)\s*\(([^)]*)\)', transform_str):
        fn   = fn_m.group(1)
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
            ang  = math.radians(args[0]) if args else 0.0
            cos_ = math.cos(ang)
            sin_ = math.sin(ang)
            rx   = args[1] if len(args) > 1 else 0.0
            ry   = args[2] if len(args) > 2 else 0.0
            mn = (
                cos_, sin_, -sin_, cos_,
                rx - rx*cos_ + ry*sin_,
                ry - rx*sin_ - ry*cos_,
            )
        else:
            continue

        m = mul(m, mn)

    return m


def apply_matrix(m, x, y):
    a, b, c, d, e, f = m
    return a*x + c*y + e, b*x + d*y + f


# ---------------------------------------------------------------------------
# ViewBox scale — convert SVG user-unit coords to document pixel coords
# ---------------------------------------------------------------------------

def get_viewbox_scale(root):
    """
    Return (scale_x, scale_y) to multiply SVG user-unit coordinates by
    to get document pixel coordinates matching the Inkscape canvas.
    """
    vb = root.get('viewBox', '')
    if not vb:
        return 1.0, 1.0
    parts = re.findall(r'[-+]?(?:\d+\.?\d*|\.\d+)(?:[eE][-+]?\d+)?', vb)
    if len(parts) < 4:
        return 1.0, 1.0
    vb_w = float(parts[2])
    vb_h = float(parts[3])
    if vb_w == 0 or vb_h == 0:
        return 1.0, 1.0

    def dim(attr):
        raw = root.get(attr, '')
        m = re.match(r'([\d.eE+\-]+)', raw)
        return float(m.group(1)) if m else None

    vp_w = dim('width')
    vp_h = dim('height')
    if vp_w is None or vp_h is None:
        return 1.0, 1.0
    return vp_w / vb_w, vp_h / vb_h


# ---------------------------------------------------------------------------
# Element centre in parent coordinate space
# ---------------------------------------------------------------------------

def element_center(elem):
    """
    Return (cx, cy) of a shape in its parent coordinate space,
    fully accounting for the element's own transform (translate, scale, matrix, etc.).
    """
    raw_tag = elem.tag.split('}')[-1] if '}' in elem.tag else elem.tag
    m = parse_transform_matrix(elem.get('transform', ''))

    if raw_tag == 'rect':
        x = float(elem.get('x', 0) or 0)
        y = float(elem.get('y', 0) or 0)
        w = float(elem.get('width',  0) or 0)
        h = float(elem.get('height', 0) or 0)
        return apply_matrix(m, x + w / 2, y + h / 2)

    if raw_tag in ('circle', 'ellipse'):
        cx = float(elem.get('cx', 0) or 0)
        cy = float(elem.get('cy', 0) or 0)
        return apply_matrix(m, cx, cy)

    if raw_tag == 'path':
        scx = elem.get(f'{{{SODIPODI_NS}}}cx')
        scy = elem.get(f'{{{SODIPODI_NS}}}cy')
        if scx is not None and scy is not None:
            return apply_matrix(m, float(scx), float(scy))

    return apply_matrix(m, 0.0, 0.0)


def parent_offset(elem, parent_map):
    """Sum translate transforms from all ancestor elements up to the SVG root."""
    tx, ty = 0.0, 0.0
    node = parent_map.get(elem)
    while node is not None:
        m = parse_transform_matrix(node.get('transform', ''))
        # Extract translation component (e, f) — sufficient for layer-only translate parents
        tx += m[4]
        ty += m[5]
        node = parent_map.get(node)
    return tx, ty


# ---------------------------------------------------------------------------
# NodeIcon path helper
# ---------------------------------------------------------------------------

def strip_ue_reference(value):
    value = value.strip()
    m = re.search(r"'([^']+)'", value)
    if m:
        return m.group(1)
    return value


# ---------------------------------------------------------------------------
# Description block parser
# ---------------------------------------------------------------------------

def parse_description(text):
    node = {
        'NodeTag':         '',
        'NodeType':        'Minor',
        'IsStartNode':     False,
        'NodeName':        '',
        'NodeDescription': '',
        'NodeIcon':        '',
        'Effects':         [],
    }

    current_effect = None
    last_key = None

    for raw_line in text.splitlines():
        line = raw_line.strip()
        if not line:
            last_key = None
            continue
        if ':' not in line:
            # A line with no "Key:" is a continuation of a real (not "\n"-escaped)
            # embedded newline inside the previous field's value — only
            # NodeDescription supports multi-line content today.
            if last_key == 'NodeDescription':
                node['NodeDescription'] += '\n' + line
            continue

        key, _, val = line.partition(':')
        key = key.strip()
        val = val.strip()
        last_key = key

        if key == 'NodeTag':
            node['NodeTag'] = val
        elif key == 'NodeType':
            node['NodeType'] = val
        elif key == 'IsStartNode':
            node['IsStartNode'] = val.lower() == 'true'
        elif key == 'NodeName':
            node['NodeName'] = val
        elif key == 'NodeDescription':
            node['NodeDescription'] = val.replace('\\n', '\n')
        elif key == 'NodeIcon':
            node['NodeIcon'] = strip_ue_reference(val) if val else ''
        elif key == 'EffectType':
            if current_effect is not None:
                node['Effects'].append(current_effect)
            current_effect = {'EffectType': val}
        elif current_effect is not None:
            if not val:
                continue
            if key == 'EffectSkillPassiveTag':
                current_effect['EffectSkillPassiveTag'] = val
            elif key == 'EffectFlagType':
                current_effect['EffectFlagType'] = val
            elif key == 'GrantSkillTag':
                current_effect['GrantSkillTag'] = val
            elif key == 'EffectStatModAttributeType':
                current_effect['EffectStatModAttributeType'] = val
            elif key == 'EffectStatModifierOp':
                current_effect['EffectStatModifierOp'] = val
            elif key == 'EffectStatMagnitude':
                try:
                    current_effect['EffectStatMagnitude'] = float(val)
                except ValueError:
                    current_effect['EffectStatMagnitude'] = val

    if current_effect is not None:
        node['Effects'].append(current_effect)

    return node if node['NodeTag'] else None


# ---------------------------------------------------------------------------
# SVG connector resolver
# ---------------------------------------------------------------------------

def resolve_to_node_id(svg_id, nodes_by_id, id_to_elem, parent_map):
    if svg_id in nodes_by_id:
        return svg_id
    elem = id_to_elem.get(svg_id)
    if elem is None:
        return None
    current = parent_map.get(elem)
    while current is not None:
        cid = current.get('id', '')
        if cid in nodes_by_id:
            return cid
        current = parent_map.get(current)
    return None


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    if len(sys.argv) < 2:
        print("Usage: python svg_to_skilltree_json.py <path_to_svg> [path_to_NodeData.xlsx]")
        sys.exit(1)

    svg_path = os.path.abspath(sys.argv[1])
    if not os.path.isfile(svg_path):
        print(f"Error: file not found: {svg_path}")
        sys.exit(1)

    node_table = None
    if len(sys.argv) >= 3:
        xlsx_path = os.path.abspath(sys.argv[2])
        if not os.path.isfile(xlsx_path):
            print(f"Error: table file not found: {xlsx_path}")
            sys.exit(1)
        node_table = load_node_table(xlsx_path)

    tree = ET.parse(svg_path)
    root = tree.getroot()

    vb_sx, vb_sy = get_viewbox_scale(root)

    parent_map = {}
    for parent in root.iter():
        for child in parent:
            parent_map[child] = parent

    id_to_elem = {}
    for elem in root.iter():
        eid = elem.get('id')
        if eid:
            id_to_elem[eid] = elem

    nodes_by_id = {}
    warnings    = []

    for elem in root.iter():
        desc_text = None
        for child in elem:
            child_tag = child.tag.split('}')[-1] if '}' in child.tag else child.tag
            if child_tag == 'desc' and child.text:
                desc_text = child.text.strip()
                break

        if not desc_text or not desc_text.startswith('NodeTag:'):
            continue

        node = parse_description(desc_text)
        if not node:
            warnings.append(f"Skipped element with empty NodeTag (id={elem.get('id', '?')})")
            continue

        if node_table is not None:
            table_entry = node_table.get(node['NodeTag'])
            if table_entry is not None:
                node = dict(table_entry)  # table content wins; position/connections added below
            else:
                warnings.append(f"Node '{node['NodeTag']}' not found in table — using inline SVG description")

        svg_id = elem.get('id', '')
        if not svg_id:
            warnings.append(f"Node '{node['NodeTag']}' has no SVG id — skipped")
            continue

        cx, cy = element_center(elem)
        px, py = parent_offset(elem, parent_map)
        node['NodePosition'] = {
            'X': round((cx + px) * vb_sx, 2),
            'Y': round((cy + py) * vb_sy, 2),
        }
        node['ConnectedNodeTags'] = []

        nodes_by_id[svg_id] = node

    seen_pairs = set()

    for elem in root.iter():
        raw_tag = elem.tag.split('}')[-1] if '}' in elem.tag else elem.tag
        if raw_tag != 'path':
            continue

        start_ref = elem.get(f'{{{INKSCAPE_NS}}}connection-start', '')
        end_ref   = elem.get(f'{{{INKSCAPE_NS}}}connection-end',   '')
        if not start_ref or not end_ref:
            continue

        start_svg_id = start_ref.lstrip('#')
        end_svg_id   = end_ref.lstrip('#')

        a = resolve_to_node_id(start_svg_id, nodes_by_id, id_to_elem, parent_map)
        b = resolve_to_node_id(end_svg_id,   nodes_by_id, id_to_elem, parent_map)

        if not a or not b or a == b:
            continue

        pair = tuple(sorted([a, b]))
        if pair in seen_pairs:
            continue
        seen_pairs.add(pair)

        tag_a = nodes_by_id[a]['NodeTag']
        tag_b = nodes_by_id[b]['NodeTag']

        if tag_b not in nodes_by_id[a]['ConnectedNodeTags']:
            nodes_by_id[a]['ConnectedNodeTags'].append(tag_b)
        if tag_a not in nodes_by_id[b]['ConnectedNodeTags']:
            nodes_by_id[b]['ConnectedNodeTags'].append(tag_a)

    output   = {'nodes': list(nodes_by_id.values())}
    out_path = os.path.splitext(svg_path)[0] + '.json'

    with open(out_path, 'w', encoding='utf-8') as f:
        json.dump(output, f, indent=2, ensure_ascii=False)

    print(f"Done.  {len(output['nodes'])} node(s), {len(seen_pairs)} connection(s)")
    print(f"ViewBox scale applied: {vb_sx:.4f} x {vb_sy:.4f}")
    print(f"       -> {out_path}")

    for w in warnings:
        print(f"WARNING: {w}")


if __name__ == '__main__':
    main()
