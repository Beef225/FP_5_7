"""Project paths and editor-wide constants."""
from __future__ import annotations

from pathlib import Path


def _find_project_root() -> Path:
    here = Path(__file__).resolve()
    for parent in here.parents:
        if any(parent.glob("*.uproject")):
            return parent
    return Path(r"C:\Unreal\FP_5_7")


PROJECT_ROOT = _find_project_root()
SKILLTREE_DIR = PROJECT_ROOT / "Tools" / "MasterDatabase" / "SkillTree"
TREES_DIR = SKILLTREE_DIR / "Trees"
REFERENCE_DIR = SKILLTREE_DIR / "Reference"
PIPELINE_DIR = SKILLTREE_DIR / "Pipeline"
GAMEPLAY_TAGS_CPP = PROJECT_ROOT / "Source" / "FP_5_7" / "Private" / "FP_GameplayTags.cpp"
NODE_TAGS_INI = PROJECT_ROOT / "Config" / "Tags" / "SkillTreeNodeTags.ini"   # data-only node tags
ATTRIBUTE_TAG_MAP_CSV = REFERENCE_DIR / "AttributeTagMap.csv"
CONTENT_DIR = PROJECT_ROOT / "Content"
ICON_LIBRARY_DIR = CONTENT_DIR / "Assets" / "UIElements" / "Icons"
IMPORT_TREE_TAGS_SCRIPT = PIPELINE_DIR / "ImportTreeTags" / "import_tree_tags.py"

# Document units — same space as NodePosition in the JSON. Each project sets its own canvas
# (ProjectMeta.canvas_width / canvas_height); this is the default for new and older projects.
CANVAS_SIZE = 3000.0
CANVAS_CENTRE = CANVAS_SIZE / 2

# Node radius per NodeType, in document units (Mastery = 125 px diameter).
NODE_RADIUS = {"Minor": 22.0, "Major": 36.0, "Mastery": 62.5}

# Defaults for new / older projects; each project overrides them (ProjectMeta.grid_*).
GRID_MINOR = 10.0     # thin line every 10 units (snap uses this too)
GRID_MAJOR = 50.0     # bold line every 50 units
RING_STEP = 500.0     # guide rings, measured along the canvas's longer half-axis; 0 = none

ORG_NAME = "FP_5_7"
APP_NAME = "Skill Tree Editor"
