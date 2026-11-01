==============================================================================
 SKILL TREES — folder layout
==============================================================================

SkillTreeEditor\     The editor app (PySide6). Open this folder in PyCharm and run
                     "Skill Tree Editor". See SkillTreeEditor\README.md.

Trees\               One folder per tree. Each tree has:
  Passive\<Name>\       <Name>.sktree   project: metadata + nodes. OPEN AND EDIT THIS.
  Skills\<Element>\<Name>\
                        <Name>.json     UE import file. Regenerated on every save.
                                        Don't edit by hand.
                        releases\       frozen copies written by Project > Mark Live Release
                        *TagReference.txt   design notes for the tree, where one exists

Reference\           AttributeTagMap.csv/.txt (the editor reads the .csv for autocomplete)
                     and NodeTemplate.txt (field reference)

Pipeline\            Scripts still in use:
  ImportTreeTags\       import_tree_tags.py. Adds NodeTags to FP_GameplayTags.h/.cpp.
                        Run it from the editor: Tools > Register NodeTags in C++.
  ImportSkillTreeAssets\  UE Python console importer (nodes only, no effects). The C++
                        FP_SkillTreeImporter (CallInEditor) also imports effects and is
                        preferred.

Archive\             Old versions of trees saved as projects, for reference only (open them in
                     the editor to compare). They are outside Trees\, so they're excluded from
                     the cross-tree tag check. NEVER import their JSON into UE: they reuse the
                     live trees' NodeTags and would overwrite those trees' assets.
                       Might_OldCopy       older 210-node Might (from "NodeData - Copy.xlsx")
                       Might_PreNewNodes   original 54-node Might (from svg.bak_pre_newnodes)

Legacy\              The retired Inkscape/xlsx pipeline, kept for reference: original SVGs,
                     NodeData workbooks, .bak files and old scripts. Nothing reads from here.
                     Delete it once you trust the projects.

------------------------------------------------------------------------------
 WORKFLOW
------------------------------------------------------------------------------
 1. Open Trees\...\<Name>.sktree in the editor.
 2. Edit, validate (F5), and use Tools > Renumber Tags when adding nodes.
 3. Ctrl+S. This saves the project and re-exports <Name>.json.
 4. Tools > Register NodeTags in C++ for new nodes, then rebuild.
 5. In UE, run FP_SkillTreeImporter on <Name>.json (Tools > Copy Export JSON Path).
 6. When a version ships to players: Project > Mark Live Release. From then on,
    validation warns about edits that would break players' allocated points.

Migrated 2026-10-31. A full backup of the pre-migration folder is at
  Tools\MasterDatabase\SkillTree_backup_20261031.zip
