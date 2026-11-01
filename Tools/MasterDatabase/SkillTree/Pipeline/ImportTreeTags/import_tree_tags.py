#!/usr/bin/env python3
# =============================================================================
# POWERSHELL — copy and paste one line at a time
# =============================================================================
# Passive trees
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Pipeline\ImportTreeTags\import_tree_tags.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Trees\Passive\Might\Might.json"
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Pipeline\ImportTreeTags\import_tree_tags.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Trees\Passive\Resonance\Resonance.json"
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Pipeline\ImportTreeTags\import_tree_tags.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Trees\Passive\Agility\Agility.json"
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Pipeline\ImportTreeTags\import_tree_tags.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Trees\Passive\Fortitude\Fortitude.json"
# Skill trees
# python "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Pipeline\ImportTreeTags\import_tree_tags.py" "C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Skills\chemical\LightningCoil\LightningCoilTree.json"
# =============================================================================
"""
import_tree_tags.py
Reads a skill tree JSON and adds any missing tags to FP_GameplayTags.h/.cpp.

Two tag types are collected and inserted in separate C++ sections:

  NodeTags       — every node's NodeTag (e.g. SkillTree.LightningCoil.Damage.1)
                   Section determined by the first segment of the first NodeTag.

  SkillPassive   — EffectSkillPassiveTag from any effect where EffectType == "Skill Passive"
                   (e.g. SkillPassive.LightningCoil.IncreasedDamage)
                   Always inserted under the "SkillPassive" section.
                   Duplicates across nodes are collected only once.

Usage:
    python import_tree_tags.py <path_to_json>
    python import_tree_tags.py <path_to_json> --section SkillTree
    python import_tree_tags.py <path_to_json> --dry-run
"""

import sys, os, json, re, argparse

# ---------------------------------------------------------------------------
# Shared helpers
# ---------------------------------------------------------------------------
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
# add_gameplay_tag.py lives in Tools\. Search upward so moving this script doesn't break it.
TOOLS_DIR = SCRIPT_DIR
while not os.path.isfile(os.path.join(TOOLS_DIR, 'add_gameplay_tag.py')):
    parent = os.path.dirname(TOOLS_DIR)
    if parent == TOOLS_DIR:
        TOOLS_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, '..', '..', '..', '..'))
        break
    TOOLS_DIR = parent
sys.path.insert(0, TOOLS_DIR)

try:
    import add_gameplay_tag as agt
except ImportError:
    print('ERROR: cannot find add_gameplay_tag.py in', TOOLS_DIR)
    sys.exit(1)


# ---------------------------------------------------------------------------
# Tag collectors
# ---------------------------------------------------------------------------

def collect_node_tags(nodes):
    """Returns (tag_string, var_name, desc) for each unique NodeTag."""
    seen, tags = set(), []
    for node in nodes:
        tag = node.get('NodeTag', '').strip()
        if not tag or tag in seen:
            continue
        seen.add(tag)
        desc = node.get('NodeDescription') or node.get('NodeName') or tag
        tags.append((tag, agt.tag_to_var(tag), desc.replace('\n', ', ')))
    return tags


def collect_skill_passive_tags(nodes):
    """Returns (tag_string, var_name, desc) for each unique EffectSkillPassiveTag
    found in Skill Passive effects. Skips effects that have no EffectSkillPassiveTag."""
    seen, tags = set(), []
    for node in nodes:
        for effect in node.get('Effects', []):
            if effect.get('EffectType') != 'Skill Passive':
                continue
            sp_tag = effect.get('EffectSkillPassiveTag', '').strip()
            if not sp_tag or sp_tag in seen:
                continue
            seen.add(sp_tag)
            leaf = sp_tag.split('.')[-1]
            desc = 'Per-skill passive: ' + re.sub(r'(?<=[a-z])(?=[A-Z])', ' ', leaf)
            tags.append((sp_tag, agt.tag_to_var(sp_tag), desc))
    return tags


GATE_ALIASES = {'Ranged': 'RangedWeapon'}


def registered_tags(c_lines):
    """Every tag string registered via AddNativeGameplayTag(FName("...")) in the cpp."""
    return set(re.findall(r'FName\("([^"]+)"\)', ''.join(c_lines)))


def resolve_gated(tag, registered):
    """Gated-stat standard (see FP_GameplayTags.h): <BaseStatTag>.<Leaf>[.<Leaf>...] where every
    trailing Leaf is a SkillTagMod.<Leaf> and BaseStatTag is a registered stat tag.
    Returns (base_tag, [required SkillTagMod tags]) or (None, reason)."""
    segs, gates = tag.split('.'), []
    while len(segs) > 1:
        leaf = GATE_ALIASES.get(segs[-1], segs[-1])
        gate = 'SkillTagMod.' + leaf
        if gate not in registered:
            return None, f"'{segs[-1]}' is not a SkillTagMod leaf"
        gates.insert(0, gate)
        segs = segs[:-1]
        base = '.'.join(segs)
        if base in registered:
            return base, gates
    return None, 'no registered base stat tag'


def collect_gated_stat_tags(nodes, registered):
    """Stat Mod attribute tags that follow the gated standard but aren't registered yet.
    Only the TAG is registered; C++ maps it onto the base attribute with SkillTagMod requirements
    (UFP_AttributeSet::ResolveStatTag) — no new attribute is created."""
    seen, tags = set(), []
    for node in nodes:
        for effect in node.get('Effects', []):
            if effect.get('EffectType') != 'Stat Mod':
                continue
            tag = (effect.get('EffectStatModAttributeType') or '').strip()
            if not tag or tag in seen or tag in registered:
                continue
            seen.add(tag)
            base, gates = resolve_gated(tag, registered)
            if base is None:
                print(f'  [GatedStats] NOT GATED, skipped: {tag}  ({gates})')
                continue
            desc = f"Gated stat: {base} only for skills with {' + '.join(gates)}"
            tags.append((tag, agt.tag_to_var(tag), desc))
    return tags


# ---------------------------------------------------------------------------
# Insertion helper (operates on already-loaded line lists)
# ---------------------------------------------------------------------------

def insert_tags(h_lines, c_lines, tags, section_hint, dry_run, label):
    """Check for duplicates, then splice missing tags into h_lines / c_lines.
    Returns the (possibly modified) h_lines and c_lines."""

    if not tags:
        return h_lines, c_lines

    missing = []
    for tag_string, var_name, desc in tags:
        hdup = agt.check_header_duplicate(h_lines, var_name)
        cdup, _ = agt.check_cpp_duplicate(c_lines, var_name, tag_string)
        if hdup >= 0 or cdup >= 0:
            print(f'  [{label}] SKIP (exists): {tag_string}')
        else:
            missing.append((tag_string, var_name, desc))

    if not missing:
        print(f'  [{label}] All tags already present — nothing to add.')
        return h_lines, c_lines

    print(f'\n  [{label}] {len(missing)} tag(s) to add:')
    for ts, vn, _ in missing:
        print(f'    {ts}  ->  {vn}')

    # Locate insertion points
    h_sections = agt.scan_sections(h_lines)
    c_func_idx = agt.cpp_func_start(c_lines)
    c_sections = agt.scan_sections(c_lines, start_at=c_func_idx)

    h_match = agt.match_section(h_sections, section_hint)
    c_match = agt.match_section(c_sections, section_hint)

    if h_match:
        h_sec_idx, h_sec_name = h_match
        h_next      = agt.next_section_line(h_sections, h_sec_idx, len(h_lines))
        h_insert_at = agt.find_header_insert(h_lines, h_sec_idx, h_next) + 1
        h_prefix    = []
        print(f'  [{label}] Header: appending to section "{h_sec_name}" at line {h_insert_at}')
    else:
        h_insert_at = agt.header_private_line(h_lines)
        h_prefix    = [f'\n//{section_hint}\n']
        print(f'  [{label}] Header: creating new section "{section_hint}"')

    if c_match:
        c_sec_idx, c_sec_name = c_match
        c_next      = agt.next_section_line(c_sections, c_sec_idx, len(c_lines))
        c_insert_at = agt.find_cpp_insert(c_lines, c_sec_idx, c_next) + 1
        c_prefix    = []
        print(f'  [{label}] CPP:    appending to section "{c_sec_name}" at line {c_insert_at}')
    else:
        func_end    = agt.cpp_func_end(c_lines, c_func_idx)
        c_insert_at = func_end
        c_prefix    = [f'\n\t//{section_hint}\n']
        print(f'  [{label}] CPP:    creating new section "{section_hint}"')

    h_new = h_prefix + [f'FGameplayTag {vn};\n' for _, vn, _ in missing]
    c_new = c_prefix + [agt.make_cpp_block(v, t, d) for t, v, d in missing]

    if dry_run:
        print(f'\n  === DRY RUN [{label}] ===')
        print(f'  Header would insert at line {h_insert_at}:')
        for line in h_new:
            print('    ' + repr(line))
        print(f'  CPP would insert at line {c_insert_at}:')
        for line in c_new:
            print('    ' + repr(line))
        return h_lines, c_lines

    h_lines[h_insert_at:h_insert_at] = h_new
    c_lines[c_insert_at:c_insert_at] = c_new
    return h_lines, c_lines


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description='Add NodeTags and SkillPassive tags from a skill tree JSON to FP_GameplayTags'
    )
    parser.add_argument('json', help='Path to the skill tree JSON file')
    parser.add_argument('--section', default=None,
                        help='Section name override for NodeTags (default: first segment of first NodeTag)')
    parser.add_argument('--dry-run', action='store_true',
                        help='Print what would change without writing files')
    args = parser.parse_args()

    json_path = os.path.abspath(args.json)
    if not os.path.isfile(json_path):
        print(f'ERROR: JSON not found: {json_path}')
        sys.exit(1)

    with open(json_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    nodes = data.get('nodes', [])
    if not nodes:
        print('No nodes found in JSON.')
        sys.exit(0)

    node_tags    = collect_node_tags(nodes)
    passive_tags = collect_skill_passive_tags(nodes)

    node_section    = args.section or (node_tags[0][0].split('.')[0] if node_tags else 'SkillTree')
    passive_section = passive_tags[0][0].split('.')[0] if passive_tags else 'SkillPassive'

    print(f'JSON            : {json_path}')
    print(f'NodeTags        : {len(node_tags)} unique  (section: {node_section})')
    print(f'SkillPassive    : {len(passive_tags)} unique  (section: {passive_section})')
    print()

    for path, label in [(agt.HEADER_PATH, 'Header'), (agt.CPP_PATH, 'CPP')]:
        if not os.path.isfile(path):
            print(f'ERROR: {label} not found:\n  {path}')
            sys.exit(1)

    h_lines = agt.read_lines(agt.HEADER_PATH)
    c_lines = agt.read_lines(agt.CPP_PATH)

    # Pass 1 — NodeTags (e.g. SkillTree section)
    print('--- NodeTags ---')
    h_lines, c_lines = insert_tags(h_lines, c_lines, node_tags, node_section, args.dry_run, 'NodeTags')

    # Pass 2 — SkillPassive accumulator tags (SkillPassive section)
    print('\n--- SkillPassive Tags ---')
    h_lines, c_lines = insert_tags(h_lines, c_lines, passive_tags, passive_section, args.dry_run, 'SkillPassive')

    # Pass 3 — gated stat tags (e.g. Speed.Skill.Rifle): tag only, no attribute
    print('\n--- Gated Stat Tags ---')
    gated_tags = collect_gated_stat_tags(nodes, registered_tags(c_lines))
    h_lines, c_lines = insert_tags(h_lines, c_lines, gated_tags, 'GatedStats', args.dry_run, 'GatedStats')

    if args.dry_run:
        print('\n=== DRY RUN complete — no files written ===')
        return

    bak_h = agt.backup(agt.HEADER_PATH)
    agt.write_lines(agt.HEADER_PATH, h_lines)
    print(f'\nHeader written.  Backup: {os.path.basename(bak_h)}')

    bak_c = agt.backup(agt.CPP_PATH)
    agt.write_lines(agt.CPP_PATH, c_lines)
    print(f'CPP written.     Backup: {os.path.basename(bak_c)}')

    print('\nDone. Rebuild the project to register the new tags.')
    print('\nNodeTags added:')
    for ts, vn, _ in node_tags:
        print(f'  FFP_GameplayTags::Get().{vn}')
    if passive_tags:
        print('\nSkillPassive tags added:')
        for ts, vn, _ in passive_tags:
            print(f'  FFP_GameplayTags::Get().{vn}')


if __name__ == '__main__':
    main()
