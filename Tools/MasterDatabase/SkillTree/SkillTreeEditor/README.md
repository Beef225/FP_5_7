# Skill Tree Editor

Custom PySide6 (Qt) editor for FP_5_7 passive and skill trees. Replaces the Inkscape SVG +
NodeData.xlsx + sync-script loop with one tool.

**Each tree is a project file, `<Name>.sktree`.** It holds the tree's metadata (name, type,
category, skill tag, working and live versions, revision, UE asset folder, notes, release history)
and all of its nodes. Every save also writes `<Name>.json` next to it. That JSON is the file
`FP_SkillTreeImporter` reads, and it carries a `treeInfo` version stamp. Don't edit the exported
JSON by hand. Trees live in `..\Trees\` (see `..\README.txt`).

**Releases:** *Project ▸ Mark Live Release* records the live version and freezes a copy to
`releases/<Name>_v<version>.json`. After that, validation errors on live nodes that have been
removed or renamed, and warns on live links that have been removed, since both break players'
allocated points.

**Cross-tree check:** validation errors when a NodeTag is also used by another project under
`Trees\`, because UE asset names come from NodeTags and one import would overwrite the other.

## Setup (PyCharm)

1. *File ▸ Open…* → this `SkillTreeEditor` folder.
2. Interpreter: *Settings ▸ Project ▸ Python Interpreter ▸ Add ▸ Existing* →
   `SkillTreeEditor\.venv\Scripts\python.exe` (PyCharm usually detects it on its own).
3. Run the **Skill Tree Editor** run configuration (from `.run/`), or right-click `main.py` ▸ Run.

From a terminal: `.venv\Scripts\python main.py [path\to\Tree.json]`.
To rebuild the venv: `py -m venv .venv` then `.venv\Scripts\pip install -r requirements.txt`.

## Workflow

1. **Open** a `.sktree` (Ctrl+O). Each project opens in its own tab, with its own undo history, and the
   open tabs are restored the next time you start the editor. *Open Recent ▸ Open All Projects* opens every
   tree. Nodes copied with Ctrl+C can be pasted into another tab. Or create one with *File ▸ New Tree Project*. *File ▸ Import…*
   turns a legacy JSON or `*_NodeData.xlsx` into a project.
2. **Edit** on the canvas and in the Inspector (see Controls below).
3. **Validate** (F5, also runs automatically). Fix the red-dot nodes.
4. **Tools ▸ Renumber Tags** gives new or duplicate nodes unique `.N` indices. Indices that
   are already valid are kept, so existing UE assets keep their names.
5. **Save** (Ctrl+S). This writes the `.sktree`, bumps the revision and re-exports the UE JSON.
6. **Tools ▸ Register Tags** runs `ImportTreeTags/import_tree_tags.py` (dry-run preview first): NodeTags go to
   `Config/Tags/SkillTreeNodeTags.ini` (restart the editor, no rebuild); SkillPassive and gated stat tags go to C++
   (rebuild). See `Tools/GameplayTags_README.md`.
7. Rebuild in UE, then run `FP_SkillTreeImporter` and point it at the JSON
   (*Tools ▸ Copy JSON Path* puts the path on your clipboard).

## Controls

| Action | Input |
|---|---|
| Zoom / pan | Wheel / Middle-drag or Space+drag |
| Box select / toggle select | Drag empty space / Ctrl+click |
| Add node | Double-click empty canvas (copies and links from the selected node) |
| Link / unlink | Shift+drag node → node, or `L` with two nodes selected |
| Nudge | Arrow keys (Shift = ×10) |
| Copy / paste / duplicate (with internal links) | Ctrl+C / Ctrl+V / Ctrl+D |
| Copy node data → paste onto every selected node | Ctrl+Shift+C / Ctrl+Shift+V |
| Undo / redo | Ctrl+Z / Ctrl+Y |
| Fit all / frame selection / snap | F / Shift+F / G |
| Next / previous tab | Ctrl+Tab / Ctrl+Shift+Tab |
| Close tab | Ctrl+W or middle-click the tab (drag tabs to reorder) |

## Data sources (read at startup, *Tools ▸ Reload Tag Catalog* to refresh)

- `Source/FP_5_7/Private/FP_GameplayTags.cpp`: registered tags. Used for autocomplete and for
  flagging unregistered tags.
- `AttributeTagMap.csv`: the curated Stat Mod attribute list, shown first in autocomplete.
- `Content/Assets/UIElements/Icons/**.png`: icon previews and the icon browser. UE paths are
  derived the way UE names imported assets (dots in folder names become `_`).

## JSON notes

- Effect Flag and Grant Skill effects are written with **both** key spellings:
  `EffectFlagType`/`EffectFlagTag` and `GrantSkillTag`/`EffectSkillTag`.
  The old scripts wrote only the first spelling, but `FP_SkillTreeImporter.cpp` reads the second.
- Unknown keys on nodes and at the top level are preserved. Editor settings go under `"editor"`,
  which both importers ignore.

## Code map

```
main.py                     entry point + dark theme
skilltree_editor/
  model.py                  Node / Effect / SkillTree, JSON load/save, renumber, undo snapshots
  project.py                .sktree projects: metadata, export, releases, cross-tree tag scan
  project_ui.py             header bar, project settings / release dialogs
  canvas.py                 QGraphicsScene/View, node + link items, mouse interactions
  inspector.py              property panel (single + multi-select), effect editors
  mainwindow.py             menus, docks, undo history, tools
  validation.py             tree checks
  catalog.py                gameplay tag catalog
  icons.py / icon_picker.py icon resolution + browser
  importers.py              legacy .xlsx migration
```
