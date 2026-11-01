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
ATTRIBUTE_TAG_MAP_CSV = REFERENCE_DIR / "AttributeTagMap.csv"
CONTENT_DIR = PROJECT_ROOT / "Content"
ICON_LIBRARY_DIR = CONTENT_DIR / "Assets" / "UIElements" / "Icons"
IMPORT_TREE_TAGS_SCRIPT = PIPELINE_DIR / "ImportTreeTags" / "import_tree_tags.py"

# Document units — same space as NodePosition in the JSON (0..3000, centre 1500,1500).
CANVAS_SIZE = 3000.0
CANVAS_CENTRE = CANVAS_SIZE / 2

# Node radius per NodeType, in document units (Mastery = 125 px diameter).
NODE_RADIUS = {"Minor": 22.0, "Major": 36.0, "Mastery": 62.5}

GRID_MINOR = 50.0
GRID_MAJOR = 250.0
SNAP_SIZE = 10.0

ORG_NAME = "FP_5_7"
APP_NAME = "Skill Tree Editor"
