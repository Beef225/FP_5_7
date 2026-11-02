"""Gameplay tag catalog — feeds autocomplete and validation.

Sources:
  FP_GameplayTags.cpp     — every native tag registered in C++ (+ its description)
  SkillTreeNodeTags.ini   — skill-tree NodeTags (data-only tags, Config/Tags)
  AttributeTagMap.csv     — curated attribute list for Stat Mod effects
"""
from __future__ import annotations

import csv
import re
from dataclasses import dataclass, field
from pathlib import Path

from . import config
from .model import TAG_PATTERN

_INI_TAG = re.compile(r'^GameplayTagList=\(Tag="([^"]+)"(?:,DevComment="((?:[^"\\]|\\.)*)")?\)', re.M)
_NATIVE_TAG = re.compile(
    r'AddNativeGameplayTag\(\s*FName\("([^"]+)"\)\s*,\s*FString\("((?:[^"\\]|\\.)*)"\)',
    re.S,
)

# Namespaces that are node identities / plumbing, never a Stat Mod attribute.
_NOT_ATTRIBUTES = ("PassiveTree.", "SkillTree.", "SkillPassive.", "SetByCaller.", "InputTag.",
                   "Montage.", "Event.", "GameplayEvent.", "Flag.", "Skills.", "Inventory.",
                   "Fragment.", "Location.", "Item.", "Player.", "State.", "SkillTagMod.")


@dataclass
class TagCatalog:
    descriptions: dict[str, str] = field(default_factory=dict)
    registered: set[str] = field(default_factory=set)       # in FP_GameplayTags.cpp
    attribute_tags: list[str] = field(default_factory=list)  # Stat Mod attribute candidates
    sources_ok: dict[str, bool] = field(default_factory=dict)

    def known(self, tag: str) -> bool:
        return tag in self.registered or tag in self.descriptions

    def describe(self, tag: str) -> str:
        return self.descriptions.get(tag, "")

    def with_prefix(self, prefix: str) -> list[str]:
        return sorted(t for t in self.registered | set(self.descriptions) if t.startswith(prefix))

    def resolve_gated(self, tag: str) -> tuple[str | None, list[str]]:
        """Gated-stat standard: <BaseStat>.<Leaf>[.<Leaf>...] where each trailing Leaf names a
        SkillTagMod.<Leaf>. Returns (base_tag, [SkillTagMod tags]) or (None, []).
        Mirrors UFP_AttributeSet::ResolveStatTag in C++."""
        segs, gates = tag.split("."), []
        while len(segs) > 1:
            leaf = "RangedWeapon" if segs[-1] == "Ranged" else segs[-1]
            gate = "SkillTagMod." + leaf
            if gate not in self.registered:
                return None, []
            gates.insert(0, gate)
            segs = segs[:-1]
            base = ".".join(segs)
            # "Damage.Increased" -> "Damage.Increased.Generic", like the C++ resolver.
            for cand in (base, base + ".Generic"):
                if cand in self.registered and not cand.startswith(_NOT_ATTRIBUTES):
                    return cand, gates
        return None, []

    def options_for(self, field_key: str) -> list[str]:
        if field_key == "EffectStatModAttributeType":
            return self.attribute_tags
        if field_key == "EffectSkillPassiveTag":
            return self.with_prefix("SkillPassive.")
        if field_key == "EffectFlagType":
            return self.with_prefix("Flag.")
        if field_key == "GrantSkillTag":
            return self.with_prefix("Skills.")
        return []


def load_catalog(cpp_path: Path = config.GAMEPLAY_TAGS_CPP,
                 csv_path: Path = config.ATTRIBUTE_TAG_MAP_CSV,
                 ini_path: Path = config.NODE_TAGS_INI) -> TagCatalog:
    cat = TagCatalog()

    try:
        text = ini_path.read_text(encoding="utf-8-sig", errors="replace")
        for tag, desc in _INI_TAG.findall(text):
            cat.registered.add(tag)
            if desc:
                cat.descriptions.setdefault(tag, desc)
        cat.sources_ok["SkillTreeNodeTags.ini"] = True
    except OSError:
        cat.sources_ok["SkillTreeNodeTags.ini"] = False

    try:
        text = cpp_path.read_text(encoding="utf-8", errors="replace")
        for tag, desc in _NATIVE_TAG.findall(text):
            cat.registered.add(tag)
            if desc:
                cat.descriptions[tag] = desc.replace('\\"', '"')
        cat.sources_ok["FP_GameplayTags.cpp"] = True
    except OSError:
        cat.sources_ok["FP_GameplayTags.cpp"] = False

    attributes: dict[str, None] = {}
    try:
        with open(csv_path, newline="", encoding="utf-8-sig") as f:
            for row in csv.reader(f):
                if len(row) < 2:
                    continue
                desc = row[0].strip()
                for tag in (c.strip() for c in row[1:]):
                    if not TAG_PATTERN.match(tag):
                        continue
                    if desc and tag not in cat.descriptions:
                        cat.descriptions[tag] = desc
                    if tag == row[1].strip():  # column 2 = attribute tag
                        attributes[tag] = None
        cat.sources_ok["AttributeTagMap.csv"] = True
    except OSError:
        cat.sources_ok["AttributeTagMap.csv"] = False

    # Curated CSV attributes first, then any other registered tag that could be an attribute.
    extra = sorted(t for t in cat.registered
                   if t not in attributes and not t.startswith(_NOT_ATTRIBUTES))
    cat.attribute_tags = list(attributes) + extra
    return cat
