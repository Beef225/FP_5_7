#!/usr/bin/env python3
"""
add_gameplay_tag.py
Adds a new native gameplay tag to FP_GameplayTags.h and FP_GameplayTags.cpp.

Usage:
    python add_gameplay_tag.py <TagString> "<Description>" [SectionHint]
    python add_gameplay_tag.py <TagString> "<Description>" --new-section "<Name>"
    python add_gameplay_tag.py <TagString> "<Description>" --dry-run

Examples:
    python add_gameplay_tag.py Damage.Increased.Frost "Increased frost damage"
    python add_gameplay_tag.py Damage.Increased.Frost "Increased frost damage" Damage
    python add_gameplay_tag.py SkillTree.Node.Core "Skill tree core node" --new-section "SkillTree"
    python add_gameplay_tag.py Damage.Increased.Frost "Increased frost damage" --dry-run
"""

import sys, os, re, argparse, shutil
from datetime import datetime

# ---------------------------------------------------------------------------
# Paths — script lives in Tools/, repo root is one level up
# ---------------------------------------------------------------------------
SCRIPT_DIR  = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT   = os.path.abspath(os.path.join(SCRIPT_DIR, '..'))
HEADER_PATH = os.path.join(REPO_ROOT, 'Source', 'FP_5_7', 'Public',   'FP_GameplayTags.h')
CPP_PATH    = os.path.join(REPO_ROOT, 'Source', 'FP_5_7', 'Private',  'FP_GameplayTags.cpp')


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def tag_to_var(tag_string: str) -> str:
    """Damage.Increased.Frost  →  Damage_Increased_Frost"""
    return tag_string.replace('.', '_')


def read_lines(path: str):
    with open(path, 'r', encoding='utf-8') as f:
        return f.readlines()


def write_lines(path: str, lines: list):
    with open(path, 'w', encoding='utf-8', newline='\n') as f:
        f.writelines(lines)


def backup(path: str) -> str:
    stamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    dst = path + f'.bak_{stamp}'
    shutil.copy2(path, dst)
    return dst


# ---------------------------------------------------------------------------
# Section scanning
# ---------------------------------------------------------------------------

def scan_sections(lines: list, start_at: int = 0):
    """
    Return [(line_idx, section_name)] for every // comment line.
    section_name has the leading // and whitespace stripped.
    """
    sections = []
    for i in range(start_at, len(lines)):
        s = lines[i].strip()
        # Single-line comment, not a /// doc comment or a URL
        if re.match(r'^//[^/]', s) or s == '//':
            name = s.lstrip('/').strip()
            sections.append((i, name))
    return sections


def cpp_func_start(lines: list) -> int:
    for i, line in enumerate(lines):
        if 'InitializeGameplayTags()' in line:
            return i
    return 0


def match_section(sections: list, hint: str):
    """
    Find the best section matching hint (case-insensitive).
    Priority: exact → hint inside section name → section name inside hint.
    Returns (line_idx, name) or None.
    """
    h = hint.lower()
    # Bare "//" lines (empty name) and short comment fragments would otherwise match any hint via
    # the "section name inside hint" rule and drop tags into the middle of comment blocks.
    sections = [(i, n) for i, n in sections if len(n.strip()) >= 3]
    exact = [(i, n) for i, n in sections if n.lower() == h]
    if exact:
        return exact[0]
    sub = [(i, n) for i, n in sections if h in n.lower()]
    if sub:
        return sorted(sub, key=lambda x: len(x[1]))[0]
    sup = [(i, n) for i, n in sections if n.lower() in h]
    if sup:
        return sorted(sup, key=lambda x: -len(x[1]))[0]
    return None


def next_section_line(sections: list, current_idx: int, total: int) -> int:
    for (li, _) in sections:
        if li > current_idx:
            return li
    return total


# ---------------------------------------------------------------------------
# Header helpers
# ---------------------------------------------------------------------------

def check_header_duplicate(lines: list, var_name: str) -> int:
    pattern = re.compile(r'\bFGameplayTag\s+' + re.escape(var_name) + r'\s*;')
    for i, line in enumerate(lines):
        if pattern.search(line):
            return i
    return -1


def find_header_insert(lines: list, section_line: int, next_section: int) -> int:
    """Return the index of the last FGameplayTag VarName; line in the section."""
    last = section_line
    for i in range(section_line + 1, next_section):
        if re.search(r'\bFGameplayTag\s+\w+\s*;', lines[i]):
            last = i
    return last


def header_private_line(lines: list) -> int:
    """Return the line index of 'private:' inside the struct."""
    for i, line in enumerate(lines):
        if line.strip() == 'private:':
            return i
    return len(lines) - 2  # fallback: just before closing brace


# ---------------------------------------------------------------------------
# CPP helpers
# ---------------------------------------------------------------------------

def check_cpp_duplicate(lines: list, var_name: str, tag_string: str):
    for i, line in enumerate(lines):
        if f'GameplayTags.{var_name}' in line:
            return i, 'variable name'
        if f'FName("{tag_string}")' in line:
            return i, 'tag string'
    return -1, None


def find_cpp_insert(lines: list, section_line: int, next_section: int) -> int:
    """Return index of the last ); that closes an AddNativeGameplayTag block."""
    last = section_line
    for i in range(section_line + 1, next_section):
        if lines[i].strip() == ');':
            last = i
    return last


def cpp_func_end(lines: list, func_start: int) -> int:
    """Return the line index of the closing } of InitializeGameplayTags."""
    depth = 0
    for i in range(func_start, len(lines)):
        for ch in lines[i]:
            if ch == '{':
                depth += 1
            elif ch == '}':
                depth -= 1
                if depth == 0:
                    return i
    return len(lines) - 1


def make_cpp_block(var_name: str, tag_string: str, description: str) -> str:
    return (
        f'\tGameplayTags.{var_name} = UGameplayTagsManager::Get().AddNativeGameplayTag(\n'
        f'\t\tFName("{tag_string}"),\n'
        f'\t\tFString("{description}")\n'
        f'\t\t);\n'
    )


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description='Add a native gameplay tag to FP_GameplayTags.h/.cpp'
    )
    parser.add_argument('tag',         help='Tag string, e.g. Damage.Increased.Frost')
    parser.add_argument('description', help='Short description')
    parser.add_argument('section',     nargs='?', default=None,
                        help='Section hint (default: first segment of tag)')
    parser.add_argument('--new-section', metavar='NAME',
                        help='Create a new section with this name if no match is found')
    parser.add_argument('--dry-run', action='store_true',
                        help='Print changes without writing files')
    args = parser.parse_args()

    tag_string  = args.tag.strip()
    description = args.description.strip()
    var_name    = tag_to_var(tag_string)
    hint        = args.section if args.section else tag_string.split('.')[0]

    print(f'  Tag      : {tag_string}')
    print(f'  Variable : {var_name}')
    print(f'  Section  : {hint}')
    print()

    # Validate file paths
    for path, label in [(HEADER_PATH, 'Header'), (CPP_PATH, 'CPP')]:
        if not os.path.isfile(path):
            print(f'ERROR: {label} not found:\n  {path}')
            sys.exit(1)

    h_lines = read_lines(HEADER_PATH)
    c_lines = read_lines(CPP_PATH)

    # Duplicate check
    hdup = check_header_duplicate(h_lines, var_name)
    if hdup >= 0:
        print(f'SKIP: {var_name} already declared in header at line {hdup + 1}.')
        sys.exit(0)

    cdup_line, cdup_reason = check_cpp_duplicate(c_lines, var_name, tag_string)
    if cdup_line >= 0:
        print(f'SKIP: {cdup_reason} already exists in cpp at line {cdup_line + 1}.')
        sys.exit(0)

    # Scan sections
    h_sections   = scan_sections(h_lines)
    c_func_idx   = cpp_func_start(c_lines)
    c_sections   = scan_sections(c_lines, start_at=c_func_idx)

    # ---- Header insert plan ------------------------------------------------
    h_match = match_section(h_sections, hint)

    if h_match:
        h_sec_idx, h_sec_name = h_match
        h_next       = next_section_line(h_sections, h_sec_idx, len(h_lines))
        h_insert_at  = find_header_insert(h_lines, h_sec_idx, h_next) + 1
        h_new_lines  = [f'FGameplayTag {var_name};\n']
        print(f'  Header: inserting into section "{h_sec_name}" after line {h_insert_at}')
    elif args.new_section:
        private_idx  = header_private_line(h_lines)
        h_insert_at  = private_idx         # insert block before private:
        h_new_lines  = [f'\n//{args.new_section}\n', f'FGameplayTag {var_name};\n', '\n']
        print(f'  Header: creating new section "{args.new_section}" before private:')
    else:
        print(f'ERROR: No section matching "{hint}" found in header.')
        print('Available sections:')
        for _, name in h_sections:
            print(f'  {name}')
        print('\nRe-run with a section hint or --new-section "Name" to create one.')
        sys.exit(1)

    # ---- CPP insert plan ---------------------------------------------------
    c_match = match_section(c_sections, hint)

    if c_match:
        c_sec_idx, c_sec_name = c_match
        c_next      = next_section_line(c_sections, c_sec_idx, len(c_lines))
        c_insert_at = find_cpp_insert(c_lines, c_sec_idx, c_next) + 1
        c_new_lines = [make_cpp_block(var_name, tag_string, description)]
        print(f'  CPP:    inserting into section "{c_sec_name}" after line {c_insert_at}')
    elif args.new_section:
        func_end    = cpp_func_end(c_lines, c_func_idx)
        c_insert_at = func_end             # insert before closing }
        c_new_lines = [
            f'\n\t//{args.new_section}\n',
            make_cpp_block(var_name, tag_string, description),
        ]
        print(f'  CPP:    creating new section "{args.new_section}" before end of function')
    else:
        print(f'ERROR: No section matching "{hint}" found in cpp.')
        print('Available sections:')
        for _, name in c_sections:
            print(f'  {name}')
        print('\nRe-run with a section hint or --new-section "Name" to create one.')
        sys.exit(1)

    print()

    # ---- Dry run -----------------------------------------------------------
    if args.dry_run:
        print('=== DRY RUN — no files written ===\n')
        print(f'Header would insert at line {h_insert_at}:')
        for line in h_new_lines:
            print('  ' + repr(line))
        print(f'\nCPP would insert at line {c_insert_at}:')
        for line in c_new_lines:
            print('  ' + repr(line))
        return

    # ---- Write files -------------------------------------------------------
    # Apply CPP first (separate file, order doesn't matter, but cpp tends to
    # have higher line numbers so do header last to keep debug output clean)

    bak_h = backup(HEADER_PATH)
    h_lines[h_insert_at:h_insert_at] = h_new_lines
    write_lines(HEADER_PATH, h_lines)
    print(f'Header written.  Backup: {os.path.basename(bak_h)}')

    bak_c = backup(CPP_PATH)
    c_lines[c_insert_at:c_insert_at] = c_new_lines
    write_lines(CPP_PATH, c_lines)
    print(f'CPP written.     Backup: {os.path.basename(bak_c)}')

    print()
    print(f'Done.  Access via:  FFP_GameplayTags::Get().{var_name}')
    print()
    print('Next steps:')
    print('  1. Rebuild the project in Unreal / VS.')
    if tag_string.split('.')[0] in (
        'Attributes', 'Health', 'Damage', 'Resistance', 'Critical',
        'Defence', 'Defense', 'Heat', 'Debuff', 'AoE', 'Speed', 'Item',
        'DoT', 'Penetration', 'Projectile', 'Conversion', 'Conditional', 'Recovery'
    ):
        print('  2. Wire it into UFP_AttributeSet::TagsToAttributes if it maps to an attribute.')
        print('  3. Add it to Tools/MasterDatabase/SkillTree/AttributeTagMap.txt.')


if __name__ == '__main__':
    main()
