#!/usr/bin/env python3
# =============================================================================
# POWERSHELL — copy and paste one line at a time
# =============================================================================
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\NodeDataTable\generate_svg_from_table.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Might\MightPassiveTree.svg" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Might\MightPassiveTree_NodeData.xlsx"
# =============================================================================
"""
generate_svg_from_table.py

Regenerates the node markers and connector lines in a passive/skill tree SVG
from its .xlsx NodeDataTable — surgically: every existing node circle/ellipse
(anything with a "NodeTag:" desc) and every existing connector path (anything
with inkscape:connector-type) is removed and rebuilt from the table. Anything
else in the SVG (the decorative boundary circle, grid, namedview, etc.) is
left completely alone.

Run this after adding/editing rows in the table — new nodes appear at their
X/Y (or, if X/Y are blank, scattered near canvas centre so you can find and
drag them into place). Open the result in Inkscape to fine-tune positions
and draw/adjust connector lines, then run sync_svg_positions_to_table.py to
write your visual edits back into the table.

A NodeTag ends up in the SVG desc as "NodeTag:X" — position and connections
live in the SVG, everything else lives in the table. That's the whole point.

Before touching the SVG at all, runs validate_node_table.py's checks
(duplicate NodeTag in Nodes, duplicate EffectOrder per NodeTag in Effects)
and aborts without writing anything if either fails.

A .bak of the SVG is written alongside it before overwriting.
"""
import sys
import os
import re
import random
from lxml import etree
from openpyxl import load_workbook

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from validate_node_table import run_validation

SVG_NS = "http://www.w3.org/2000/svg"
INKSCAPE_NS = "http://www.inkscape.org/namespaces/inkscape"

MINOR_R = "5.8208332"
MAJOR_R = "9.7"
MASTERY_R = "16.536458"  # 125px diameter on the standard 3000px canvas (125/2 * 793.75/3000)


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


def generate_palette(n):
    import colorsys
    if n == 0:
        return []
    colours = []
    for i in range(n):
        hue = i / n
        r, g, b = colorsys.hsv_to_rgb(hue, 0.65, 0.88)
        colours.append("#{:02x}{:02x}{:02x}".format(int(r * 255), int(g * 255), int(b * 255)))
    return colours


def load_table(xlsx_path):
    wb = load_workbook(xlsx_path, data_only=True)

    ws_nodes = wb["Nodes"]
    headers = [c.value for c in next(ws_nodes.iter_rows(min_row=1, max_row=1))]
    col = {name: i for i, name in enumerate(headers)}
    nodes = []
    for row in ws_nodes.iter_rows(min_row=2, values_only=True):
        if not row or not row[col["NodeTag"]]:
            continue
        tag = str(row[col["NodeTag"]]).strip()
        x = row[col["X"]] if "X" in col else None
        y = row[col["Y"]] if "Y" in col else None
        node_type = row[col["NodeType"]] or "Minor"
        nodes.append({
            "tag": tag, "type": node_type,
            "x": float(x) if isinstance(x, (int, float)) else None,
            "y": float(y) if isinstance(y, (int, float)) else None,
        })

    connections = []
    if "Connections" in wb.sheetnames:
        ws_conn = wb["Connections"]
        cheaders = [c.value for c in next(ws_conn.iter_rows(min_row=1, max_row=1))]
        ccol = {name: i for i, name in enumerate(cheaders)}
        for row in ws_conn.iter_rows(min_row=2, values_only=True):
            if not row or not row[ccol["NodeTagA"]] or not row[ccol["NodeTagB"]]:
                continue
            a = str(row[ccol["NodeTagA"]]).strip()
            b = str(row[ccol["NodeTagB"]]).strip()
            connections.append((a, b))

    return nodes, connections


def main():
    if len(sys.argv) < 3:
        print("Usage: python generate_svg_from_table.py <path_to_svg> <path_to_xlsx>")
        sys.exit(1)

    svg_path = os.path.abspath(sys.argv[1])
    xlsx_path = os.path.abspath(sys.argv[2])
    if not os.path.isfile(svg_path):
        print(f"Error: SVG not found: {svg_path}")
        sys.exit(1)
    if not os.path.isfile(xlsx_path):
        print(f"Error: table not found: {xlsx_path}")
        sys.exit(1)

    print("Validating table before touching the SVG...")
    if not run_validation(xlsx_path, verbose=True):
        print("\nABORTED: table failed validation — SVG was not touched. Fix the table and re-run.")
        sys.exit(1)
    print()

    nodes, connections = load_table(xlsx_path)

    parser = etree.XMLParser(remove_blank_text=False)
    tree = etree.parse(svg_path, parser)
    root = tree.getroot()
    vb_sx, vb_sy = get_viewbox_scale(root)
    layer = root.find(f"{{{SVG_NS}}}g[@id='layer1']")
    if layer is None:
        layer = root

    # ---- strip existing node markers + connector paths ----
    removed_nodes = 0
    removed_conns = 0
    for elem in list(layer.iter()):
        tag = etree.QName(elem).localname
        if tag in ("circle", "ellipse"):
            desc_el = elem.find(f"{{{SVG_NS}}}desc")
            if desc_el is not None and desc_el.text and desc_el.text.strip().startswith("NodeTag:"):
                elem.getparent().remove(elem)
                removed_nodes += 1
        elif tag == "path":
            if elem.get(f"{{{INKSCAPE_NS}}}connector-type"):
                elem.getparent().remove(elem)
                removed_conns += 1

    # ---- colour palette by group (2nd tag segment) ----
    def group_of(tag):
        parts = tag.split(".")
        if parts[0] == "SkillTree":
            return parts[2] if len(parts) > 2 else parts[-1]
        return parts[1] if len(parts) > 1 else parts[-1]

    groups = sorted({group_of(n["tag"]) for n in nodes})
    palette = dict(zip(groups, generate_palette(len(groups))))

    # ---- place + write node markers ----
    rng = random.Random(1234)  # deterministic scatter for unpositioned nodes
    CX_PX, CY_PX = 1500.0, 1500.0  # canvas centre in document-pixel units
    id_by_tag = {}
    unpositioned = 0

    for n in nodes:
        tag = n["tag"]
        x_px, y_px = n["x"], n["y"]
        if x_px is None or y_px is None:
            x_px = CX_PX + rng.uniform(-120, 120)
            y_px = CY_PX + rng.uniform(-120, 120)
            unpositioned += 1
        cx = x_px / vb_sx
        cy = y_px / vb_sy

        elem_id = tag.replace(".", "_")
        id_by_tag[tag] = elem_id
        node_type_lower = str(n["type"]).lower()
        if node_type_lower == "mastery":
            radius = MASTERY_R
        elif node_type_lower == "major":
            radius = MAJOR_R
        else:
            radius = MINOR_R

        el = etree.SubElement(layer, f"{{{SVG_NS}}}circle")
        fill = palette.get(group_of(tag), "#cccccc")
        el.set("style", f"opacity:0.85;fill:{fill};stroke-width:107.527;paint-order:stroke fill markers")
        el.set("id", elem_id)
        el.set("cx", f"{cx:.5f}")
        el.set("cy", f"{cy:.5f}")
        el.set("r", radius)

        desc_el = etree.SubElement(el, f"{{{SVG_NS}}}desc")
        desc_el.set("id", f"desc_{elem_id}")
        desc_el.text = f"NodeTag:{tag}"

    # ---- write connector paths ----
    pos_by_tag = {n["tag"]: (
        (n["x"] if n["x"] is not None else CX_PX),
        (n["y"] if n["y"] is not None else CY_PX),
    ) for n in nodes}

    conn_written = 0
    conn_skipped = 0
    for i, (a, b) in enumerate(connections, start=1):
        if a not in id_by_tag or b not in id_by_tag:
            conn_skipped += 1
            continue
        ax_px, ay_px = pos_by_tag[a]
        bx_px, by_px = pos_by_tag[b]
        ax, ay = ax_px / vb_sx, ay_px / vb_sy
        bx, by = bx_px / vb_sx, by_px / vb_sy

        path = etree.SubElement(layer, f"{{{SVG_NS}}}path")
        path.set("style", "fill:none;fill-rule:evenodd;stroke:#000000;stroke-width:0.264583px;"
                           "stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1")
        path.set("d", f"M {ax:.5f},{ay:.5f} {bx:.5f},{by:.5f}")
        path.set("id", f"pathTable{i}")
        path.set(f"{{{INKSCAPE_NS}}}connector-type", "polyline")
        path.set(f"{{{INKSCAPE_NS}}}connector-curvature", "0")
        path.set(f"{{{INKSCAPE_NS}}}connection-start", f"#{id_by_tag[a]}")
        path.set(f"{{{INKSCAPE_NS}}}connection-end", f"#{id_by_tag[b]}")
        conn_written += 1

    bak_path = svg_path + ".bak"
    tree_orig = etree.parse(svg_path)  # re-read untouched original for the backup
    tree_orig.write(bak_path, pretty_print=True, xml_declaration=True, encoding="utf-8")

    tree.write(svg_path, pretty_print=True, xml_declaration=True, encoding="utf-8")

    print(f"Removed {removed_nodes} old node marker(s), {removed_conns} old connector(s).")
    print(f"Wrote {len(nodes)} node(s) ({unpositioned} scattered near centre — no X/Y in table), "
          f"{conn_written} connector(s).")
    if conn_skipped:
        print(f"WARNING: skipped {conn_skipped} connection row(s) referencing a NodeTag not in the Nodes sheet.")
    print(f"Backup of prior SVG: {bak_path}")
    print(f"Saved: {svg_path}")


if __name__ == "__main__":
    main()
