"""Skill-tree NodeTags live in Config/Tags/SkillTreeNodeTags.ini, not in C++.

See Tools/GameplayTags_README.md for which tags go where. Node tags (PassiveTree.<Group>.<N>,
SkillTree.<Skill>.<Group>.<N>) are only referenced by data (node assets, the tree widget), never
by C++ code, so registering them in FP_GameplayTags.h/.cpp only cost recompiles.

UE loads every .ini in <Project>/Config/Tags/ as a tag source (ImportTagsFromConfig=True in
Config/DefaultGameplayTags.ini). Restart the editor to pick up changes — no rebuild needed.
"""
from __future__ import annotations

import re
import unicodedata
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[5]          # .../FP_5_7
INI_PATH = PROJECT_ROOT / "Config" / "Tags" / "SkillTreeNodeTags.ini"
SECTION = "[/Script/GameplayTags.GameplayTagsList]"
NODE_TAG = re.compile(r"^(PassiveTree|SkillTree)\.[A-Za-z0-9_.]+\.\d+$")
_LINE = re.compile(r'^GameplayTagList=\(Tag="([^"]+)"(?:,DevComment="((?:[^"\\]|\\.)*)")?\)\s*$')

HEADER = """; =============================================================================
; Skill-tree NodeTags (PassiveTree.<Group>.<N>, SkillTree.<Skill>.<Group>.<N>)
; Written by Tools/MasterDatabase/SkillTree/Pipeline/ImportTreeTags (editor: Tools > Register Tags).
; Data-only tags: never reference these from C++. See Tools/GameplayTags_README.md.
; Entries are never removed automatically (imported node assets may still use them).
; =============================================================================
"""


def is_node_tag(tag: str) -> bool:
    return bool(NODE_TAG.match(tag))


def _ascii(text: str) -> str:
    text = text.replace("—", "-").replace("–", "-").replace("\n", ", ").replace('"', "'")
    return unicodedata.normalize("NFKD", text).encode("ascii", "ignore").decode("ascii").strip()


def read() -> dict[str, str]:
    """tag -> dev comment, for every tag in the ini (empty if the file doesn't exist)."""
    tags: dict[str, str] = {}
    if INI_PATH.is_file():
        for line in INI_PATH.read_text(encoding="utf-8-sig").splitlines():
            m = _LINE.match(line.strip())
            if m:
                tags[m.group(1)] = (m.group(2) or "").replace('\\"', '"')
    return tags


def write(tags: dict[str, str]) -> None:
    INI_PATH.parent.mkdir(parents=True, exist_ok=True)
    lines = [HEADER, SECTION]
    for tag in sorted(tags, key=lambda t: [int(p) if p.isdigit() else p.lower() for p in t.split(".")]):
        comment = _ascii(tags[tag])
        lines.append(f'GameplayTagList=(Tag="{tag}",DevComment="{comment}")' if comment
                     else f'GameplayTagList=(Tag="{tag}")')
    INI_PATH.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\r\n")


def add(new: dict[str, str]) -> list[str]:
    """Merge tags into the ini; returns the ones that were actually new. Existing comments are kept."""
    tags = read()
    added = [t for t in new if t not in tags]
    if added:
        for t in added:
            tags[t] = new[t]
        write(tags)
    return added
