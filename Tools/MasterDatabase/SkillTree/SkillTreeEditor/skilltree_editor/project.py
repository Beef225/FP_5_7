"""Tree projects (.sktree): metadata + nodes, with the UE import JSON exported on every save.

  <Name>.sktree        what you open/save — {"format", "formatVersion", "project": {...}, "nodes": [...]}
  <Name>.json          generated on save — exactly what the UE importers read ({"nodes": [...], "treeInfo"})
  releases/<Name>_v<version>.json   frozen export written by "Mark Live Release"
"""
from __future__ import annotations

import json
import os
import re
from dataclasses import asdict, dataclass, field, fields
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from . import config
from .model import SkillTree

PROJECT_EXT = ".sktree"
FORMAT_ID = "FP.SkillTreeProject"
FORMAT_VERSION = 1
TREE_TYPES = ("Passive", "Skill")
SEMVER = re.compile(r"^(\d+)\.(\d+)\.(\d+)$")


def now_iso() -> str:
    return datetime.now(timezone.utc).astimezone().isoformat(timespec="seconds")


def bump(version: str, part: str) -> str:
    m = SEMVER.match(version or "")
    major, minor, patch = (int(g) for g in m.groups()) if m else (0, 1, 0)
    if part == "major":
        return f"{major + 1}.0.0"
    if part == "minor":
        return f"{major}.{minor + 1}.0"
    return f"{major}.{minor}.{patch + 1}"


@dataclass
class Release:
    version: str
    date: str
    file: str            # relative to the project folder
    notes: str = ""
    node_count: int = 0


@dataclass
class ProjectMeta:
    name: str = "New Tree"
    tree_type: str = "Skill"           # Passive | Skill
    category: str = ""                  # e.g. Physical / Chemical / Might
    skill_tag: str = ""                 # skill this tree modifies, e.g. Skills.Impact
    description: str = ""
    author: str = ""
    version: str = "0.1.0"              # working version
    live_version: str = ""              # last version released to players ("" = never)
    revision: int = 0                   # auto-incremented on every save
    created: str = field(default_factory=now_iso)
    modified: str = ""
    tag_root: str = ""                  # prefix for new nodes; "" = derive from tags
    ue_asset_folder: str = ""           # FP_SkillTreeImporter NodeAssetOutputPath
    export_file: str = ""               # UE JSON, relative to project; "" = <stem>.json
    canvas_width: float = config.CANVAS_SIZE   # document units (NodePosition space)
    canvas_height: float = config.CANVAS_SIZE
    grid_minor: float = config.GRID_MINOR      # thin lines + snap step; 0 = no thin lines
    grid_major: float = config.GRID_MAJOR      # bold lines; 0 = none
    ring_step: float = config.RING_STEP        # guide ovals; 0 = none
    notes: str = ""
    releases: list[Release] = field(default_factory=list)

    @classmethod
    def from_json(cls, data: dict) -> "ProjectMeta":
        known = {f.name for f in fields(cls)}
        meta = cls(**{k: v for k, v in data.items() if k in known and k != "releases"})
        meta.releases = [Release(**r) for r in data.get("releases", [])]
        return meta

    def to_json(self) -> dict:
        return asdict(self)


class Project:
    def __init__(self, tree: SkillTree | None = None, meta: ProjectMeta | None = None,
                 path: Path | None = None) -> None:
        self.tree = tree or SkillTree()
        self.meta = meta or ProjectMeta()
        self.path = path
        self._live_snapshot: SkillTree | None = None
        self._live_loaded_for: str | None = None
        self.apply_tag_root()

    # ---- paths ---------------------------------------------------------------

    @property
    def folder(self) -> Path | None:
        return self.path.parent if self.path else None

    @property
    def export_path(self) -> Path | None:
        if not self.path:
            return None
        return self.folder / (self.meta.export_file or self.path.stem + ".json")

    def apply_tag_root(self) -> None:
        self.tree.explicit_tag_root = self.meta.tag_root

    # ---- load / save -----------------------------------------------------------

    @classmethod
    def load(cls, path: str | Path) -> "Project":
        path = Path(path)
        with open(path, "r", encoding="utf-8") as f:
            data = json.load(f)
        if data.get("format") != FORMAT_ID:
            raise ValueError(f"{path.name} is not a skill tree project ({FORMAT_ID}).")
        meta = ProjectMeta.from_json(data.get("project", {}))
        body = {k: v for k, v in data.items() if k not in ("format", "formatVersion", "project")}
        return cls(SkillTree.from_json(body), meta, path)

    @classmethod
    def from_tree_json(cls, json_path: str | Path, meta: ProjectMeta) -> "Project":
        """Wrap a plain UE-format tree JSON in a new (unsaved) project."""
        tree = SkillTree.load(json_path)
        tree.meta.pop("treeInfo", None)
        legacy_root = tree.meta.pop("editor", {}).get("tagRoot")
        if legacy_root and not meta.tag_root:
            meta.tag_root = legacy_root
        return cls(tree, meta)

    def export_json(self) -> dict:
        data = self.tree.to_json()
        data["treeInfo"] = {
            "name": self.meta.name,
            "treeType": self.meta.tree_type,
            "version": self.meta.version,
            "liveVersion": self.meta.live_version,
            "revision": self.meta.revision,
            "canvasSize": {"X": self.meta.canvas_width, "Y": self.meta.canvas_height},
            "exported": self.meta.modified,
        }
        return data

    def save(self, path: Path | None = None) -> None:
        if path is not None:
            self.path = Path(path)
        if self.path is None:
            raise ValueError("Project has no path")
        self.meta.revision += 1
        self.meta.modified = now_iso()
        self.folder.mkdir(parents=True, exist_ok=True)
        body = self.tree.to_json()
        doc = {"format": FORMAT_ID, "formatVersion": FORMAT_VERSION,
               "project": self.meta.to_json(), **body}
        _write_json(self.path, doc)
        _write_json(self.export_path, self.export_json())

    # ---- releases ----------------------------------------------------------------

    def mark_live(self, notes: str, bump_after: bool) -> Release:
        """Freeze the current tree as the live release. Caller must save afterwards."""
        version = self.meta.version
        rel_file = f"releases/{self.path.stem}_v{version}.json"
        self.meta.live_version = version
        self.meta.modified = now_iso()
        snapshot = self.export_json()
        snapshot["treeInfo"]["releaseNotes"] = notes
        _write_json(self.folder / rel_file, snapshot)
        release = Release(version, now_iso(), rel_file, notes, len(self.tree.nodes))
        self.meta.releases.append(release)
        if bump_after:
            self.meta.version = bump(version, "patch")
        self._live_loaded_for = None
        return release

    def live_snapshot(self) -> SkillTree | None:
        """The tree as last released to players (cached)."""
        if not self.meta.live_version or not self.folder:
            return None
        if self._live_loaded_for != self.meta.live_version:
            self._live_loaded_for = self.meta.live_version
            self._live_snapshot = None
            rel = next((r for r in reversed(self.meta.releases) if r.version == self.meta.live_version), None)
            if rel and (self.folder / rel.file).is_file():
                self._live_snapshot = SkillTree.load(self.folder / rel.file)
        return self._live_snapshot


def other_project_tags(trees_dir: Path, exclude: Path | None) -> dict[str, str]:
    """NodeTag -> project name for every other .sktree under trees_dir.

    UE asset names are derived from NodeTag, so a tag used by two trees means one tree's
    import overwrites the other's node assets.
    """
    owners: dict[str, str] = {}
    exclude = exclude.resolve() if exclude else None
    for path in trees_dir.rglob("*" + PROJECT_EXT):
        if exclude and path.resolve() == exclude:
            continue
        try:
            with open(path, encoding="utf-8") as f:
                data = json.load(f)
        except (OSError, ValueError):
            continue
        name = data.get("project", {}).get("name", path.stem)
        for node in data.get("nodes", []):
            owners.setdefault(node.get("NodeTag", ""), name)
    return owners


def _write_json(path: Path, data: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = path.with_suffix(path.suffix + ".tmp")
    with open(tmp, "w", encoding="utf-8", newline="\n") as f:
        json.dump(data, f, indent=2, ensure_ascii=False)
        f.write("\n")
    os.replace(tmp, path)
