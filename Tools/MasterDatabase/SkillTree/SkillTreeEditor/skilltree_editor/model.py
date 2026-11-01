"""Skill tree data model and JSON round-trip.

The JSON written here is the same file both UE importers consume:
  - FP_SkillTreeImporter (C++, CallInEditor)  — reads nodes AND effects
  - import_skill_tree_assets.py (UE Python)   — reads nodes, not effects
so there is no conversion step between this editor and Unreal.
"""
from __future__ import annotations

import copy
import json
import os
import re
from dataclasses import dataclass, field
from itertools import count
from pathlib import Path
from typing import Any

NODE_TYPES = ("Minor", "Major", "Mastery")
EFFECT_TYPES = ("Stat Mod", "Skill Passive", "Effect Flag", "Grant Skill")
MODIFIER_OPS = ("Add", "Multiply")

# Fields each EffectType uses, in display order.
EFFECT_FIELDS: dict[str, tuple[str, ...]] = {
    "Stat Mod": ("EffectStatModAttributeType", "EffectStatModifierOp", "EffectStatMagnitude"),
    "Skill Passive": ("EffectSkillPassiveTag", "EffectStatMagnitude"),
    "Effect Flag": ("EffectFlagType",),
    "Grant Skill": ("GrantSkillTag",),
}

# The old Python pipeline wrote the left-hand keys; FP_SkillTreeImporter.cpp reads
# the right-hand ones. We read either and write both so every importer is happy.
KEY_ALIASES = {"EffectFlagType": "EffectFlagTag", "GrantSkillTag": "EffectSkillTag"}

FIELD_LABELS = {
    "EffectStatModAttributeType": "Attribute",
    "EffectStatModifierOp": "Op",
    "EffectStatMagnitude": "Magnitude",
    "EffectSkillPassiveTag": "Passive Tag",
    "EffectFlagType": "Flag Tag",
    "GrantSkillTag": "Skill Tag",
}

NUMERIC_FIELDS = {"EffectStatMagnitude"}

TAG_PATTERN = re.compile(r"^[A-Za-z0-9_]+(\.[A-Za-z0-9_]+)+$")
_INDEXED_TAG = re.compile(r"^(.*)\.(\d+)$")

_NODE_KEYS = ("NodeTag", "NodeType", "IsStartNode", "NodeName", "NodeDescription",
              "NodeIcon", "NodePosition", "ConnectedNodeTags", "Effects")


def normalize_ue_reference(value: str) -> str:
    """Accept a pasted "/Script/Engine.Texture2D'/Game/Foo.Foo'" and return "/Game/Foo.Foo"."""
    value = (value or "").strip()
    m = re.search(r"'([^']+)'", value)
    return m.group(1) if m else value


def split_tag(tag: str) -> tuple[str, int | None]:
    """'SkillTree.Impact.Foo.3' -> ('SkillTree.Impact.Foo', 3); no index -> (tag, None)."""
    m = _INDEXED_TAG.match(tag or "")
    if m:
        return m.group(1), int(m.group(2))
    return tag or "", None


@dataclass
class Effect:
    effect_type: str = "Stat Mod"
    values: dict[str, Any] = field(default_factory=dict)

    @classmethod
    def from_json(cls, data: dict) -> "Effect":
        data = dict(data)
        effect_type = data.pop("EffectType", "Stat Mod")
        for legacy, cpp in KEY_ALIASES.items():
            if cpp in data:
                value = data.pop(cpp)
                if data.get(legacy) in (None, ""):
                    data[legacy] = value
        mag = data.get("EffectStatMagnitude")
        if isinstance(mag, str):
            try:
                data["EffectStatMagnitude"] = float(mag)
            except ValueError:
                pass
        return cls(effect_type, data)

    def to_json(self) -> dict:
        out: dict[str, Any] = {"EffectType": self.effect_type}
        keys = EFFECT_FIELDS.get(self.effect_type)
        if keys is None:  # unknown type: keep whatever we were given
            out.update(self.values)
            return out
        for key in keys:
            value = self.values.get(key)
            if value in (None, ""):
                continue
            out[key] = value
            if key in KEY_ALIASES:
                out[KEY_ALIASES[key]] = value
        return out

    def summary(self) -> str:
        v = self.values
        if self.effect_type == "Stat Mod":
            return f"{v.get('EffectStatModAttributeType', '?')} {v.get('EffectStatModifierOp', 'Add')} {v.get('EffectStatMagnitude', '?')}"
        if self.effect_type == "Skill Passive":
            return f"{v.get('EffectSkillPassiveTag', '?')} +{v.get('EffectStatMagnitude', '?')}"
        if self.effect_type == "Effect Flag":
            return f"Flag {v.get('EffectFlagType', '?')}"
        if self.effect_type == "Grant Skill":
            return f"Grant {v.get('GrantSkillTag', '?')}"
        return self.effect_type


@dataclass
class Node:
    uid: int
    tag: str = ""
    node_type: str = "Minor"
    is_start: bool = False
    name: str = ""
    description: str = ""
    icon: str = ""
    x: float = 0.0
    y: float = 0.0
    effects: list[Effect] = field(default_factory=list)
    extra: dict[str, Any] = field(default_factory=dict)  # unknown keys, preserved

    @property
    def group(self) -> str:
        """Colour/grouping key — the tag's effect-name segment (second-to-last)."""
        parts = self.tag.split(".")
        return parts[-2] if len(parts) >= 3 else self.tag

    def content_json(self) -> dict:
        """Everything except identity, position and connections (for 'paste node data')."""
        return {
            "NodeType": self.node_type,
            "NodeName": self.name,
            "NodeDescription": self.description,
            "NodeIcon": self.icon,
            "Effects": [e.to_json() for e in self.effects],
        }

    def apply_content_json(self, data: dict) -> None:
        self.node_type = data.get("NodeType", self.node_type)
        self.name = data.get("NodeName", self.name)
        self.description = data.get("NodeDescription", self.description)
        self.icon = data.get("NodeIcon", self.icon)
        self.effects = [Effect.from_json(e) for e in data.get("Effects", [])]


class SkillTree:
    def __init__(self) -> None:
        self.nodes: dict[int, Node] = {}
        self.edges: set[frozenset[int]] = set()
        self.meta: dict[str, Any] = {}  # unknown top-level keys, preserved
        self.explicit_tag_root = ""     # set from project metadata; "" = derive from tags
        self._uids = count(1)

    # ---- construction ------------------------------------------------------

    def new_uid(self) -> int:
        return next(self._uids)

    def add_node(self, **fields: Any) -> Node:
        node = Node(uid=self.new_uid(), **fields)
        self.nodes[node.uid] = node
        return node

    def remove_node(self, uid: int) -> None:
        self.nodes.pop(uid, None)
        self.edges = {e for e in self.edges if uid not in e}

    def connect(self, a: int, b: int) -> bool:
        if a == b or a not in self.nodes or b not in self.nodes:
            return False
        key = frozenset((a, b))
        if key in self.edges:
            return False
        self.edges.add(key)
        return True

    def disconnect(self, a: int, b: int) -> bool:
        key = frozenset((a, b))
        if key in self.edges:
            self.edges.remove(key)
            return True
        return False

    def is_connected(self, a: int, b: int) -> bool:
        return frozenset((a, b)) in self.edges

    def neighbors(self, uid: int) -> list[int]:
        return [next(iter(e - {uid})) for e in self.edges if uid in e]

    # ---- tag root ----------------------------------------------------------

    @property
    def tag_root(self) -> str:
        """e.g. 'SkillTree.Impact' — used to name new nodes."""
        if self.explicit_tag_root:
            return self.explicit_tag_root
        # Common prefix of every tag, minus the trailing <Group>.<Index> segments:
        #   SkillTree.Impact.InitialAreaOfEffect.1      -> SkillTree.Impact
        #   PassiveTree.Accelerator.AcceleratorDamage.1 \
        #   PassiveTree.DeferredDamage.12               / -> PassiveTree
        common: list[str] | None = None
        for node in self.nodes.values():
            parts = node.tag.split(".")[:-2]
            if not parts:
                continue
            if common is None:
                common = parts
            else:
                i = 0
                while i < min(len(common), len(parts)) and common[i] == parts[i]:
                    i += 1
                common = common[:i]
        return ".".join(common) if common else "SkillTree.NewTree"

    def next_free_tag(self, prefix: str) -> str:
        used = {split_tag(n.tag)[1] for n in self.nodes.values() if split_tag(n.tag)[0] == prefix}
        i = 1
        while i in used:
            i += 1
        return f"{prefix}.{i}"

    def renumber(self) -> list[tuple[str, str]]:
        """Give every node a unique index within its prefix group.

        Valid, unique indices are kept (so already-imported UE assets keep their
        names); only placeholders (0 / missing) and duplicates get the next free one.
        """
        groups: dict[str, list[tuple[Node, int | None]]] = {}
        for node in self.nodes.values():
            prefix, idx = split_tag(node.tag)
            groups.setdefault(prefix, []).append((node, idx))
        renames: list[tuple[str, str]] = []
        for prefix, items in groups.items():
            if not prefix:
                continue
            used: set[int] = set()
            needs: list[Node] = []
            for node, idx in items:
                if idx and idx not in used:
                    used.add(idx)
                else:
                    needs.append(node)
            nxt = 1
            for node in needs:
                while nxt in used:
                    nxt += 1
                used.add(nxt)
                new_tag = f"{prefix}.{nxt}"
                renames.append((node.tag, new_tag))
                node.tag = new_tag
        return renames

    # ---- undo snapshots ----------------------------------------------------

    def snapshot(self) -> tuple:
        return copy.deepcopy((self.nodes, self.edges, self.meta))

    def restore(self, snap: tuple) -> None:
        self.nodes, self.edges, self.meta = copy.deepcopy(snap)
        top = max(self.nodes, default=0)
        self._uids = count(top + 1)

    # ---- clipboard fragments ----------------------------------------------

    def fragment(self, uids: list[int]) -> dict:
        index = {uid: i for i, uid in enumerate(uids)}
        nodes = []
        for uid in uids:
            n = self.nodes[uid]
            nodes.append({"NodeTag": n.tag, "IsStartNode": n.is_start,
                          "X": n.x, "Y": n.y, **n.content_json()})
        edges = [sorted((index[a], index[b])) for a, b in (tuple(e) for e in self.edges)
                 if a in index and b in index]
        return {"nodes": nodes, "edges": edges}

    def paste_fragment(self, frag: dict, dx: float, dy: float) -> list[int]:
        new_uids: list[int] = []
        for data in frag["nodes"]:
            prefix, _ = split_tag(data["NodeTag"])
            node = self.add_node(tag=self.next_free_tag(prefix), is_start=False,
                                 x=data["X"] + dx, y=data["Y"] + dy)
            node.apply_content_json(data)
            new_uids.append(node.uid)
        for i, j in frag["edges"]:
            self.connect(new_uids[i], new_uids[j])
        return new_uids

    # ---- JSON ---------------------------------------------------------------

    @classmethod
    def from_json(cls, data: dict) -> "SkillTree":
        tree = cls()
        tree.meta = {k: v for k, v in data.items() if k != "nodes"}
        by_tag: dict[str, int] = {}
        pending: list[tuple[int, list[str]]] = []
        for raw in data.get("nodes", []):
            pos = raw.get("NodePosition") or {}
            node = tree.add_node(
                tag=raw.get("NodeTag", ""),
                node_type=raw.get("NodeType", "Minor"),
                is_start=bool(raw.get("IsStartNode", False)),
                name=raw.get("NodeName", ""),
                description=raw.get("NodeDescription", ""),
                icon=normalize_ue_reference(raw.get("NodeIcon", "")),
                x=float(pos.get("X", 0.0)),
                y=float(pos.get("Y", 0.0)),
                effects=[Effect.from_json(e) for e in raw.get("Effects", [])],
                extra={k: v for k, v in raw.items() if k not in _NODE_KEYS},
            )
            by_tag.setdefault(node.tag, node.uid)
            pending.append((node.uid, raw.get("ConnectedNodeTags", [])))
        for uid, targets in pending:
            for tag in targets:
                other = by_tag.get(tag)
                if other is not None:
                    tree.connect(uid, other)
        return tree

    def to_json(self) -> dict:
        out_nodes = []
        for node in self.nodes.values():
            connected = sorted(self.nodes[n].tag for n in self.neighbors(node.uid))
            entry = {
                "NodeTag": node.tag,
                "NodeType": node.node_type,
                "IsStartNode": node.is_start,
                "NodeName": node.name,
                "NodeDescription": node.description,
                "NodeIcon": node.icon,
                "NodePosition": {"X": round(node.x, 2), "Y": round(node.y, 2)},
                "ConnectedNodeTags": connected,
                "Effects": [e.to_json() for e in node.effects],
            }
            entry.update(node.extra)
            out_nodes.append(entry)
        return {"nodes": out_nodes, **self.meta}

    @classmethod
    def load(cls, path: str | Path) -> "SkillTree":
        with open(path, "r", encoding="utf-8") as f:
            return cls.from_json(json.load(f))

    def save(self, path: str | Path) -> None:
        path = Path(path)
        tmp = path.with_suffix(path.suffix + ".tmp")
        with open(tmp, "w", encoding="utf-8", newline="\n") as f:
            json.dump(self.to_json(), f, indent=2, ensure_ascii=False)
            f.write("\n")
        os.replace(tmp, path)
