#!/usr/bin/env python3
# =============================================================================
# POWERSHELL — copy and paste one line at a time
# =============================================================================
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\ValidatePassiveTree\Validate_Passive_Tree.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Might\MightPassiveTree.svg"
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\ValidatePassiveTree\Validate_Passive_Tree.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Resonance\ResonancePassiveTree.svg"
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\ValidatePassiveTree\Validate_Passive_Tree.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Agility\AgilityPassiveTree.svg"
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\ValidatePassiveTree\Validate_Passive_Tree.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Fortitude\FortitudePassiveTree.svg"
# =============================================================================
"""
Validate_Passive_Tree.py

  1. Renumbers passive tree nodes so every NodeTag is unique.
     Nodes sharing the same prefix (everything before the last number) are
     numbered 1, 2, 3 ... in document order.

  2. Renames each node's SVG element id to its final NodeTag
     (dots replaced with underscores, e.g. PassiveTree_Might_IncreasedArmour_1).
     Connector inkscape:connection-start/end references are updated to match.

  3. Colors nodes by their group (second tag segment: Might, Fortitude, etc.)

  4. Validates that all final NodeTag values are unique.

Tag format:  PassiveTree.<Group>.<EffectName>.<Index>
             SkillTree.<SkillName>.<EffectName>.<Index>

  The last segment is the auto-assigned number — leave any placeholder there.
  If the entire description is "ignore" (case-insensitive) the node is skipped.

Usage:
  python Validate_Passive_Tree.py <path_to_svg>
  python Validate_Passive_Tree.py <path_to_svg> --dry-run
"""

import sys
import os
import re
import math
import argparse
import colorsys
from collections import defaultdict

try:
    from lxml import etree
except ImportError:
    print("ERROR: lxml is required.  Install with:  pip install lxml")
    sys.exit(1)

# ---------------------------------------------------------------------------
# Namespaces
# ---------------------------------------------------------------------------
SVG_NS      = "http://www.w3.org/2000/svg"
SODIPODI_NS = "http://sodipodi.sourceforge.net/DTD/sodipodi-0.0.dtd"
INKSCAPE_NS = "http://www.inkscape.org/namespaces/inkscape"

NSMAP = {
    "svg":      SVG_NS,
    "sodipodi": SODIPODI_NS,
    "inkscape": INKSCAPE_NS,
}

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def tag_to_id(node_tag):
    return node_tag.replace(".", "_")


def get_desc_element(element):
    return element.find(f"{{{SVG_NS}}}desc")


def get_desc_text(element):
    desc_el = get_desc_element(element)
    if desc_el is not None and desc_el.text:
        return desc_el.text
    return None


def parse_node_tag(desc_text):
    stripped = desc_text.strip()
    if stripped.lower() == "ignore":
        return "ignore"
    for line in stripped.splitlines():
        line = line.strip()
        if line.lower().startswith("nodetag:"):
            value = line[len("nodetag:"):].strip()
            return value if value else None
    return None


def replace_node_tag_line(desc_text, new_tag):
    lines = desc_text.splitlines(keepends=True)
    result = []
    for line in lines:
        if line.strip().lower().startswith("nodetag:"):
            leading = line[: len(line) - len(line.lstrip())]
            ending  = "\n" if line.endswith("\n") else ""
            result.append(f"{leading}NodeTag:{new_tag}{ending}")
        else:
            result.append(line)
    return "".join(result)


def is_node_element(element):
    local = etree.QName(element.tag).localname
    if local in ("ellipse", "circle"):
        return True
    if local == "path":
        return element.get(f"{{{SODIPODI_NS}}}type") == "arc"
    return False


def set_fill(element, color):
    style = element.get("style", "")
    if re.search(r'\bfill\s*:', style):
        style = re.sub(r'\bfill\s*:[^;]*', f"fill:{color}", style)
    else:
        style = f"fill:{color};" + style if style else f"fill:{color}"
    element.set("style", style)


def generate_palette(n):
    if n == 0:
        return []
    colours = []
    for i in range(n):
        hue = i / n
        r, g, b = colorsys.hsv_to_rgb(hue, 0.65, 0.88)
        colours.append("#{:02x}{:02x}{:02x}".format(
            int(r * 255), int(g * 255), int(b * 255)))
    return colours


# ---------------------------------------------------------------------------
# Connector sanitization
# ---------------------------------------------------------------------------

def parse_path_endpoints(d_str):
    """
    Extract the first (start) and last (end) (x, y) coordinate pair from
    an SVG path d attribute.  Works for any Inkscape polyline connector.
    """
    coords = [float(n) for n in re.findall(
        r'[-+]?(?:\d+\.?\d*|\.\d+)(?:[eE][-+]?\d+)?', d_str)]
    if len(coords) < 4:
        return None, None
    return (coords[0], coords[1]), (coords[-2], coords[-1])


def element_center_raw(elem):
    """
    Return the raw (cx, cy) of a node element in its own coordinate space.
    Does not account for ancestor transforms — sufficient when nodes and
    connectors share the same parent layer (the common Inkscape layout).
    """
    local = etree.QName(elem.tag).localname
    if local in ('circle', 'ellipse'):
        return float(elem.get('cx', 0) or 0), float(elem.get('cy', 0) or 0)
    if local == 'rect':
        x = float(elem.get('x', 0) or 0)
        y = float(elem.get('y', 0) or 0)
        w = float(elem.get('width',  0) or 0)
        h = float(elem.get('height', 0) or 0)
        return x + w / 2, y + h / 2
    if local == 'path':
        scx = elem.get(f'{{{SODIPODI_NS}}}cx')
        scy = elem.get(f'{{{SODIPODI_NS}}}cy')
        if scx and scy:
            return float(scx), float(scy)
    return None, None


def sanitize_connectors(root, node_id_to_elem, snap_distance=150.0):
    """
    Scan every connector path in the SVG.  For each endpoint whose
    inkscape:connection-start / inkscape:connection-end attribute is
    missing or points to an ID that no longer exists, attempt to repair
    it by snapping to the nearest node element centre.

    snap_distance — maximum SVG-unit distance to accept a snap (default 80).
                    Raise this value if nodes are spread very far apart.

    Returns (repaired_count, still_broken_count).
    """
    conn_start_attr = f"{{{INKSCAPE_NS}}}connection-start"
    conn_end_attr   = f"{{{INKSCAPE_NS}}}connection-end"
    conn_type_attr  = f"{{{INKSCAPE_NS}}}connector-type"

    # Build centre map: id -> (cx, cy)
    centres = {}
    for nid, elem in node_id_to_elem.items():
        pos = element_center_raw(elem)
        if pos[0] is not None:
            centres[nid] = pos

    def nearest(point):
        if point is None or not centres:
            return None, float('inf')
        px, py = point
        best_id, best_dist = None, float('inf')
        for nid, (nx, ny) in centres.items():
            d = math.sqrt((px - nx) ** 2 + (py - ny) ** 2)
            if d < best_dist:
                best_dist, best_id = d, nid
        return best_id, best_dist

    repaired = 0
    broken   = 0

    for elem in root.iter():
        if etree.QName(elem.tag).localname != 'path':
            continue
        if not elem.get(conn_type_attr):
            continue  # not a connector at all

        s_ref = elem.get(conn_start_attr, '').lstrip('#')
        e_ref = elem.get(conn_end_attr,   '').lstrip('#')
        s_ok  = bool(s_ref and s_ref in node_id_to_elem)
        e_ok  = bool(e_ref and e_ref in node_id_to_elem)

        if s_ok and e_ok:
            continue  # both endpoints are valid — nothing to do

        pt_s, pt_e = parse_path_endpoints(elem.get('d', ''))
        changed = False

        if not s_ok:
            nid, dist = nearest(pt_s)
            if nid and dist <= snap_distance:
                elem.set(conn_start_attr, f'#{nid}')
                changed = True
            else:
                broken += 1

        if not e_ok:
            nid, dist = nearest(pt_e)
            if nid and dist <= snap_distance:
                elem.set(conn_end_attr, f'#{nid}')
                changed = True
            else:
                broken += 1

        if changed:
            repaired += 1

    return repaired, broken


# ---------------------------------------------------------------------------
# Main processing
# ---------------------------------------------------------------------------

def process_svg(svg_path, dry_run=False):
    print(f"Processing: {svg_path}\n")

    parser = etree.XMLParser(remove_blank_text=False)
    tree   = etree.parse(svg_path, parser)
    root   = tree.getroot()

    candidates = (
        root.xpath(".//svg:ellipse",                    namespaces=NSMAP) +
        root.xpath(".//svg:circle",                     namespaces=NSMAP) +
        root.xpath('.//svg:path[@sodipodi:type="arc"]', namespaces=NSMAP)
    )

    data_nodes = []
    errors     = []

    for element in candidates:
        desc_el = get_desc_element(element)
        if desc_el is None or not desc_el.text:
            continue

        desc_text = desc_el.text
        node_tag  = parse_node_tag(desc_text)

        if node_tag is None:
            continue
        if node_tag.lower() == "ignore":
            continue

        parts = node_tag.split(".")
        is_skill_tree = parts[0] == "SkillTree"
        min_parts = 4 if is_skill_tree else 3
        if len(parts) < min_parts:
            errors.append(
                f"  Tag too short '{node_tag}' on element id='{element.get('id', '<no id>')}'"
                + (" (SkillTree tags need at least 4 segments: SkillTree.Skill.Group.#)" if is_skill_tree else "")
            )
            continue

        # PassiveTree.Might.IncreasedArmour.1  → group = parts[1] = "Might"
        # SkillTree.LightningCoil.Damage.1     → group = parts[2] = "Damage"
        group = parts[2] if is_skill_tree else parts[1]

        data_nodes.append({
            "element":   element,
            "desc_el":   desc_el,
            "desc_text": desc_text,
            "original":  node_tag,
            "old_id":    element.get("id", ""),
            "prefix":    ".".join(parts[:-1]),
            "group":     group,
        })

    if errors:
        print("ERRORS:")
        for msg in errors:
            print(msg)
        print()

    if not data_nodes:
        print("No valid passive tree nodes found.")
        return True

    # Assign sequential numbers within each prefix group
    prefix_counters = defaultdict(int)
    for node in data_nodes:
        prefix_counters[node["prefix"]] += 1
        node["new_index"] = prefix_counters[node["prefix"]]
        node["new_tag"]   = f"{node['prefix']}.{node['new_index']}"
        node["new_id"]    = tag_to_id(node["new_tag"])

    # Validate uniqueness
    tag_check = defaultdict(list)
    for node in data_nodes:
        tag_check[node["new_tag"]].append(node)
    for tag, nodes in tag_check.items():
        if len(nodes) > 1:
            errors.append(f"  Post-renumber collision on '{tag}' — this is a bug.")
    if errors:
        print("ERRORS (post-renumber):")
        for msg in errors:
            print(msg)
        return False

    # Report
    prefix = "[DRY RUN] " if dry_run else ""
    tag_changes = [n for n in data_nodes if n["new_tag"] != n["original"]]
    id_changes  = [n for n in data_nodes if n["new_id"]  != n["old_id"]]

    if tag_changes:
        print(f"{prefix}Renumbering {len(tag_changes)} NodeTag(s):")
        for n in tag_changes:
            print(f"  {n['original']}  ->  {n['new_tag']}")
        print()

    if id_changes:
        print(f"{prefix}Renaming {len(id_changes)} element id(s):")
        for n in id_changes:
            print(f"  {n['old_id'] or '<none>'}  ->  {n['new_id']}")
        print()

    if not tag_changes and not id_changes:
        print("All node tags and ids are already correct.\n")

    # Colour assignments
    group_nodes = defaultdict(list)
    for node in data_nodes:
        group_nodes[node["group"]].append(node)
    groups       = sorted(group_nodes.keys())
    palette      = generate_palette(len(groups))
    group_colour = dict(zip(groups, palette))

    # Detect whether this SVG contains SkillTree or PassiveTree nodes (or both).
    has_skill_tree   = any(n["original"].startswith("SkillTree.")   for n in data_nodes)
    has_passive_tree = any(not n["original"].startswith("SkillTree.") for n in data_nodes)
    mixed = has_skill_tree and has_passive_tree

    print(f"{prefix}Group colour assignments:")
    for group, colour in group_colour.items():
        count = len(group_nodes[group])
        if mixed:
            label = group
        elif has_skill_tree:
            label = f"SkillTree.*.{group}.*"
        else:
            label = f"*.{group}.*"
        print(f"  {label}  ->  {colour}  ({count} node{'s' if count != 1 else ''})")

    # Apply
    if not dry_run:
        id_remap = {
            n["old_id"]: n["new_id"]
            for n in data_nodes
            if n["old_id"] and n["old_id"] != n["new_id"]
        }

        for node in data_nodes:
            node["desc_el"].text = replace_node_tag_line(node["desc_text"], node["new_tag"])
            node["element"].set("id", node["new_id"])
            set_fill(node["element"], group_colour[node["group"]])

        if id_remap:
            conn_start = f"{{{INKSCAPE_NS}}}connection-start"
            conn_end   = f"{{{INKSCAPE_NS}}}connection-end"
            updated    = 0
            for element in root.iter():
                changed = False
                for attr in (conn_start, conn_end):
                    ref = element.get(attr, "")
                    if ref.startswith("#"):
                        old_id = ref[1:]
                        if old_id in id_remap:
                            element.set(attr, f"#{id_remap[old_id]}")
                            changed = True
                if changed:
                    updated += 1
            if updated:
                print(f"\nUpdated connector references on {updated} connector(s).")

        # Sanitize broken / detached connector references
        node_id_map = {n["new_id"]: n["element"] for n in data_nodes if n["new_id"]}
        repaired, still_broken = sanitize_connectors(root, node_id_map)
        if repaired:
            print(f"Connector sanitization: repaired {repaired} broken connector(s).")
        if still_broken:
            print(f"Connector sanitization: {still_broken} connector endpoint(s) "
                  f"could not be matched (no node within snap distance). Check in Inkscape.")

        tree.write(svg_path, pretty_print=True, xml_declaration=True, encoding="utf-8")
        print(f"\nSaved: {svg_path}")

    print(f"\n{'Would process' if dry_run else 'Processed'} {len(data_nodes)} nodes "
          f"across {len(groups)} group{'s' if len(groups) != 1 else ''}.")
    return True


def main():
    parser = argparse.ArgumentParser(
        description="Renumber, rename IDs, colour, and validate passive tree nodes."
    )
    parser.add_argument("svg_file", help="Path to the Inkscape SVG file.")
    parser.add_argument("--dry-run", action="store_true",
                        help="Print all changes without modifying the file.")
    args = parser.parse_args()

    if not os.path.isfile(args.svg_file):
        print(f"ERROR: File not found: {args.svg_file}")
        sys.exit(1)

    success = process_svg(args.svg_file, dry_run=args.dry_run)
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
