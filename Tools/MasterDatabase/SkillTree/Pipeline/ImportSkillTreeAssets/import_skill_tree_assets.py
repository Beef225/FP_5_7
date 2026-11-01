"""
import_skill_tree_assets.py
Run inside Unreal Engine's Python console to create/update UFP_SkillTreeNodeData
assets from a skill tree JSON file.

# =============================================================================
# UE OUTPUT LOG PYTHON CONSOLE — copy and paste one block at a time
# =============================================================================
# import sys, importlib
# sys.path.append(r'C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Pipeline\ImportSkillTreeAssets')
# import import_skill_tree_assets as importer; importlib.reload(importer)
# importer.import_tree(json_path=r'C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Trees\Passive\Might\Might.json', output_folder='/Game/Blueprints/UI/SkillTrees/CharacterSkillTrees/Data/Nodes/Might')

# import sys, importlib
# sys.path.append(r'C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Pipeline\ImportSkillTreeAssets')
# import import_skill_tree_assets as importer; importlib.reload(importer)
# importer.import_tree(json_path=r'C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Trees\Passive\Resonance\Resonance.json', output_folder='/Game/Blueprints/UI/SkillTrees/CharacterSkillTrees/Data/Nodes/Resonance')

# import sys, importlib
# sys.path.append(r'C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Pipeline\ImportSkillTreeAssets')
# import import_skill_tree_assets as importer; importlib.reload(importer)
# importer.import_tree(json_path=r'C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Trees\Passive\Agility\Agility.json', output_folder='/Game/Blueprints/UI/SkillTrees/CharacterSkillTrees/Data/Nodes/Agility')

# import sys, importlib
# sys.path.append(r'C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Pipeline\ImportSkillTreeAssets')
# import import_skill_tree_assets as importer; importlib.reload(importer)
# importer.import_tree(json_path=r'C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Trees\Passive\Fortitude\Fortitude.json', output_folder='/Game/Blueprints/UI/SkillTrees/CharacterSkillTrees/Data/Nodes/Fortitude')
# =============================================================================
"""

import unreal
import json
import os


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _tag_to_asset_name(tag_string: str) -> str:
    """PassiveTree.IncreasedArmour.1  ->  PassiveTree_IncreasedArmour_1"""
    return tag_string.replace('.', '_')


def _node_type_enum(type_string: str):
    """Map JSON NodeType string to ESkillTreeNodeType enum value."""
    mapping = {
        'Minor':   unreal.ESkillTreeNodeType.MINOR,
        'Major':   unreal.ESkillTreeNodeType.MAJOR,
        'Mastery': unreal.ESkillTreeNodeType.MASTERY,
    }
    return mapping.get(type_string, unreal.ESkillTreeNodeType.MINOR)


def _make_gameplay_tag(tag_string: str) -> unreal.GameplayTag:
    return unreal.GameplayTagsManager.get().request_gameplay_tag(
        unreal.Name(tag_string), error_if_not_found=False
    )


def _load_or_create_asset(asset_name: str, package_path: str):
    """
    Load an existing UFP_SkillTreeNodeData asset or create a new one.
    Returns the asset object, or None on failure.
    """
    full_path = f'{package_path}/{asset_name}'

    existing = unreal.EditorAssetLibrary.load_asset(full_path)
    if existing and existing.get_class().get_name() == 'FP_SkillTreeNodeData':
        return existing

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    new_asset = asset_tools.create_asset(
        asset_name=asset_name,
        package_path=package_path,
        asset_class=unreal.FP_SkillTreeNodeData,
        factory=None
    )

    if new_asset is None:
        unreal.log_error(f'[SkillTree] Failed to create asset: {full_path}')
    return new_asset


def _set_node_properties(asset, node: dict):
    """Write all non-Effects properties from the JSON node dict to the data asset."""

    tag = _make_gameplay_tag(node.get('NodeTag', ''))
    asset.set_editor_property('node_tag', tag)

    asset.set_editor_property('node_type', _node_type_enum(node.get('NodeType', 'Minor')))

    asset.set_editor_property('b_is_starter_node', bool(node.get('IsStartNode', False)))

    asset.set_editor_property('node_name',        unreal.Text(node.get('NodeName', '')))
    asset.set_editor_property('node_description', unreal.Text(node.get('NodeDescription', '')))

    pos = node.get('NodePosition', {})
    asset.set_editor_property('node_position', unreal.Vector2D(
        float(pos.get('X', 0.0)),
        float(pos.get('Y', 0.0))
    ))

    connected = []
    for tag_str in node.get('ConnectedNodeTags', []):
        connected.append(_make_gameplay_tag(tag_str))
    asset.set_editor_property('connected_node_tags', connected)

    icon_path = node.get('NodeIcon', '').strip()
    if icon_path:
        icon = unreal.EditorAssetLibrary.load_asset(icon_path)
        if icon:
            asset.set_editor_property('node_icon', icon)
        else:
            unreal.log_warning(
                f'[SkillTree] Icon not found for {node.get("NodeTag")}: {icon_path}'
            )


# ---------------------------------------------------------------------------
# Main entry point
# ---------------------------------------------------------------------------

def import_tree(json_path: str, output_folder: str):
    """
    Import all nodes from json_path and create/update UFP_SkillTreeNodeData
    assets under output_folder.

    Args:
        json_path:     Absolute OS path to the .json file.
        output_folder: UE content path, e.g. '/Game/.../Nodes/Might'
    """

    if not os.path.isfile(json_path):
        unreal.log_error(f'[SkillTree] JSON not found: {json_path}')
        return

    with open(json_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    nodes = data.get('nodes', [])
    if not nodes:
        unreal.log_warning('[SkillTree] No nodes found in JSON.')
        return

    unreal.log(f'[SkillTree] Importing {len(nodes)} node(s) -> {output_folder}')

    unreal.EditorAssetLibrary.make_directory(output_folder)

    # Build set of asset names that should exist after this import
    expected_asset_names = set()
    for node in nodes:
        tag_string = node.get('NodeTag', '').strip()
        if tag_string:
            expected_asset_names.add(_tag_to_asset_name(tag_string))

    # Delete assets in the folder that are no longer in the JSON
    deleted = []
    existing_assets = unreal.EditorAssetLibrary.list_assets(output_folder, recursive=False, include_folder=False)
    for asset_path in existing_assets:
        asset_name = asset_path.split('/')[-1].split('.')[0]
        if asset_name not in expected_asset_names:
            asset_obj = unreal.EditorAssetLibrary.load_asset(asset_path)
            if asset_obj and asset_obj.get_class().get_name() == 'FP_SkillTreeNodeData':
                if unreal.EditorAssetLibrary.delete_asset(asset_path):
                    deleted.append(asset_name)
                    unreal.log(f'[SkillTree] Deleted stale asset: {asset_name}')
                else:
                    unreal.log_warning(f'[SkillTree] Could not delete: {asset_path}')

    created  = []
    updated  = []
    failed   = []

    with unreal.ScopedEditorTransaction('SkillTree Import') as trans:
        for node in nodes:
            tag_string = node.get('NodeTag', '').strip()
            if not tag_string:
                unreal.log_warning('[SkillTree] Node with empty NodeTag skipped.')
                continue

            asset_name = _tag_to_asset_name(tag_string)
            full_path  = f'{output_folder}/{asset_name}'
            is_new     = not unreal.EditorAssetLibrary.does_asset_exist(full_path)

            asset = _load_or_create_asset(asset_name, output_folder)
            if asset is None:
                failed.append(tag_string)
                continue

            _set_node_properties(asset, node)
            unreal.EditorAssetLibrary.save_asset(full_path)

            if is_new:
                created.append(tag_string)
            else:
                updated.append(tag_string)

    unreal.log(f'[SkillTree] Import complete.')
    unreal.log(f'  Created : {len(created)}')
    unreal.log(f'  Updated : {len(updated)}')
    unreal.log(f'  Deleted : {len(deleted)}')
    unreal.log(f'  Failed  : {len(failed)}')

    if failed:
        for t in failed:
            unreal.log_error(f'  FAILED: {t}')

    unreal.log('')
    unreal.log('[SkillTree] NEXT STEPS:')
    unreal.log('  1. Open each new data asset and add Effects manually')
    unreal.log('     (Effects use TInstancedStruct -- not settable via Python).')
    unreal.log('  2. Assign all generated assets to NodeDataAssets on your')
    unreal.log(f'     tree widget BP subclass. Assets are in: {output_folder}')

    return created + updated
