# ==============================================================================
# HOW TO USE
# ==============================================================================
#
# 1. Set one or more FIND_ variables below to the exact current value of that
#    field (the text after the colon, whitespace is ignored when matching).
#    Set the matching REPLACE_ variable to the new value.
#    Any pair where FIND_ is empty is skipped entirely.
#
# 2. Run the script — it edits every SVG listed in SVG_FILES in-place.
#    Only nodes whose field value exactly matches FIND_ are changed.
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Passive Tree Replacement\replace_node_fields.py"

#
# NOTES
#   - Values are matched exactly (case-sensitive) after stripping leading/
#     trailing whitespace.
#   - To replace a tag: set FIND_NodeTag to the tag WITHOUT the "NodeTag:"
#     prefix — e.g. "PassiveTree.IncreasedArmour.1".
#   - To replace a description with a newline: use \n in REPLACE_NodeDescription
#     e.g. "+5 Armour\n+5 to Concussed Threshold"
#   - Comment out SVG paths in SVG_FILES to restrict which trees are modified.
# ==============================================================================


# ==============================================================================
# FIND → REPLACE  (set FIND_ to the current value; leave FIND_ blank to skip)
# ==============================================================================

# True  → only replace other fields on nodes whose NodeTag contains FIND_NodeTag
# False → replace other fields on any node that matches, regardless of NodeTag
SCOPE_TO_NODE_TAG = True

#can be partial match for NodeTag
FIND_NodeTag                       = "PassiveTree.AllThreshold"
REPLACE_NodeTag                    = "PassiveTree.AllThreshold"

FIND_NodeType                      = ""
REPLACE_NodeType                   = ""

FIND_IsStartNode                   = ""
REPLACE_IsStartNode                = ""

FIND_NodeName                      = ""
REPLACE_NodeName                   = ""

FIND_NodeDescription               = ""
REPLACE_NodeDescription            = ""

FIND_NodeIcon                      = "/Script/Engine.Texture2D'/Engine/EngineMaterials/DefaultCalibrationColor.DefaultCalibrationColor'"
REPLACE_NodeIcon                   = "/Script/Engine.Texture2D'/Game/Assets/UIElements/Icons/game-icons_net_png/icons/ffffff/000000/1x1/lorc/shield-echoes.shield-echoes'"

FIND_EffectType                    = ""
REPLACE_EffectType                 = ""

FIND_EffectFlagType                = ""
REPLACE_EffectFlagType             = ""

FIND_GrantSkillTag                 = ""
REPLACE_GrantSkillTag              = ""

FIND_EffectStatModAttributeType    = ""
REPLACE_EffectStatModAttributeType = ""

FIND_EffectStatModifierOp          = ""
REPLACE_EffectStatModifierOp       = ""

FIND_EffectStatMagnitude           = "3"
REPLACE_EffectStatMagnitude        = "2"


# ==============================================================================
# SVG FILES TO PROCESS  (comment out trees you do not want modified)
# ==============================================================================

SVG_FILES = [
 #   r"C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Might\MightPassiveTree.svg",
 #   r"C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Resonance\ResonancePassiveTree.svg",
 #   r"C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Agility\AgilityPassiveTree.svg",
    r"C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Fortitude\FortitudePassiveTree.svg",
]


# ==============================================================================
# SCRIPT  (no edits needed below this line)
# ==============================================================================

import os
import sys
from lxml import etree

SVG_NS = "http://www.w3.org/2000/svg"

_RAW = {
    "NodeTag":                    (FIND_NodeTag,                    REPLACE_NodeTag),
    "NodeType":                   (FIND_NodeType,                   REPLACE_NodeType),
    "IsStartNode":                (FIND_IsStartNode,                REPLACE_IsStartNode),
    "NodeName":                   (FIND_NodeName,                   REPLACE_NodeName),
    "NodeDescription":            (FIND_NodeDescription,            REPLACE_NodeDescription),
    "NodeIcon":                   (FIND_NodeIcon,                   REPLACE_NodeIcon),
    "EffectType":                 (FIND_EffectType,                 REPLACE_EffectType),
    "EffectFlagType":             (FIND_EffectFlagType,             REPLACE_EffectFlagType),
    "GrantSkillTag":              (FIND_GrantSkillTag,              REPLACE_GrantSkillTag),
    "EffectStatModAttributeType": (FIND_EffectStatModAttributeType, REPLACE_EffectStatModAttributeType),
    "EffectStatModifierOp":       (FIND_EffectStatModifierOp,       REPLACE_EffectStatModifierOp),
    "EffectStatMagnitude":        (FIND_EffectStatMagnitude,        REPLACE_EffectStatMagnitude),
}

# Only process fields where FIND_ is non-empty
ACTIVE = {k: (f.strip(), r) for k, (f, r) in _RAW.items() if f.strip()}


def process_desc_text(text):
    """
    Walk each line of a node desc block, replace matching field values.
    Returns (new_text, list_of_change_strings).
    """
    if not text:
        return text, []

    lines = text.split("\n")

    # When SCOPE_TO_NODE_TAG is True, skip non-NodeTag replacements unless this
    # desc block's NodeTag contains FIND_NodeTag.
    node_tag_matches = True
    if SCOPE_TO_NODE_TAG and "NodeTag" in ACTIVE:
        find_tag = ACTIVE["NodeTag"][0]
        node_tag_matches = any(
            line[line.find(":") + 1:].strip().find(find_tag) != -1
            for line in lines
            if line.find(":") != -1 and line[:line.find(":")].strip() == "NodeTag"
        )

    result  = []
    changes = []

    for line in lines:
        colon = line.find(":")
        if colon == -1:
            result.append(line)
            continue

        field = line[:colon].strip()
        rest  = line[colon + 1:]
        value = rest.strip()
        sep   = ": " if rest.startswith(" ") else ":"

        if field in ACTIVE:
            find_val, replace_val = ACTIVE[field]

            if field == "NodeTag" and find_val in value:
                new_value = value.replace(find_val, replace_val, 1)
                result.append(f"{field}{sep}{new_value}")
                changes.append(f"    {field}: '{value}'  →  '{new_value}'")
                continue

            elif field != "NodeTag" and value == find_val and node_tag_matches:
                result.append(f"{field}{sep}{replace_val}")
                changes.append(f"    {field}: '{value}'  →  '{replace_val}'")
                continue

        result.append(line)

    return "\n".join(result), changes


def process_svg(svg_path):
    if not os.path.isfile(svg_path):
        print(f"[SKIP] File not found: {svg_path}")
        return

    print(f"\n[FILE] {svg_path}")

    parser = etree.XMLParser(remove_blank_text=False)
    tree   = etree.parse(svg_path, parser)
    root   = tree.getroot()

    total = 0
    for desc in root.iter(f"{{{SVG_NS}}}desc"):
        if desc.text is None:
            continue
        new_text, changes = process_desc_text(desc.text)
        if changes:
            desc.text = new_text
            total += len(changes)
            for c in changes:
                print(c)

    if total == 0:
        print("  No matching nodes found.")
    else:
        tree.write(svg_path, encoding="utf-8", xml_declaration=True, pretty_print=False)
        print(f"  Saved — {total} replacement(s).")


def main():
    if not ACTIVE:
        print("No FIND_ variables are set. Edit the script and fill in at least one FIND/REPLACE pair.")
        sys.exit(0)

    print("Active replacements:")
    for field, (f, r) in ACTIVE.items():
        print(f"  {field}: '{f}'  →  '{r}'")

    for path in SVG_FILES:
        process_svg(path)

    print("\nDone.")


if __name__ == "__main__":
    main()
