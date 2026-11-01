================================================================================
  AFFIX SYSTEM — WORKFLOW INSTRUCTIONS
================================================================================

This document covers the full pipeline from editing the affix database in Excel
through to a live item actor in Unreal with affixes rolling on spawn.

--------------------------------------------------------------------------------
  PART 1 — EXPORTING FROM EXCEL TO JSON
--------------------------------------------------------------------------------

Run this step any time you add, edit, or remove affixes or pool entries.

1. Open AffixDatabase.xlsm in Excel if you need to make changes.
   - MasterList sheet  : one row per affix tier (all affix definitions live here)
   - Weapon_Ranged, Weapon_Melee, Weapon_Utility, Headwear_Might, etc.
                       : pool sheets, one per equipment slot

2. Save and close the workbook (openpyxl cannot read it while Excel has it open).

3. Open a terminal in this folder and run:

       python export_to_json.py

   This produces two files in the same folder:
     - affixes.json  — all affix definitions keyed by AffixID (84 affixes, ~570 tiers)
     - pools.json    — all pool sheets keyed by slot name (14 pools, ~520 entries)

   Expected output:
       Exporting MasterList -> affixes.json ...
         84 affixes, 569 total tier rows -> ...\affixes.json
       Exporting pool sheets -> pools.json ...
         14 pools, 523 total pool entries -> ...\pools.json
       Done.

   If you see a WARNING about a missing sheet, add it to POOL_SHEETS in
   export_to_json.py or create the sheet in the workbook.

NOTE: The other update_pools_*.py and fix_pool_*.py scripts in this folder are
one-shot migration scripts that have already been applied. Do not re-run them
unless you are intentionally re-migrating (they are idempotent but unnecessary).

--------------------------------------------------------------------------------
  PART 2 — IMPORTING INTO UNREAL
--------------------------------------------------------------------------------

Run this step after every export to keep the data assets in sync.

1. In the Content Browser, navigate to the DA_AffixImporter data asset.
   (Wherever you created it — suggested path: /Game/Data/Affixes/)

2. Double-click it to open it in the Details panel.

3. Verify OutputContentPath is set to your intended folder, e.g.:
       /Game/Data/Affixes

4. Click the "Import Affixes" button at the top of the Details panel.

   This creates or updates the following assets under OutputContentPath:
     - Definitions/DA_Affix_{AffixID}  (one UFP_AffixDefinition per affix, 84 total)
     - Pools/DA_Pool_{SlotName}         (one UFP_AffixPool per slot, 14 total)
     - DA_AffixRegistry                 (the central AffixID -> Definition lookup map)

   Watch the Output Log for:
       FP_AffixImporter: imported 84 definitions, 14 pools -> /Game/Data/Affixes

   The importer is safe to re-run at any time — it updates existing assets in place
   rather than creating duplicates.

--------------------------------------------------------------------------------
  PART 3 — SETTING UP THE RARITY TABLE
--------------------------------------------------------------------------------

Do this once, or revisit when you want to tune the affix budget per rarity.

1. If you don't have one yet: Content Browser -> right-click ->
   Miscellaneous -> Data Asset -> FP_RarityTable.
   Name it e.g. DA_RarityTable_Default.

2. Open it and fill in the Entries array. Each entry is one rarity tier:

   Rarity      Weight   MinPrefixes  MaxPrefixes  MinSuffixes  MaxSuffixes
   -------     ------   -----------  -----------  -----------  -----------
   Common       500          0            0            0            0
   Uncommon     200          1            2            1            2
   Rare          50          2            3            2            3
   Legendary      5          3            3            3            3

   Weight drives the weighted random roll (higher = more likely).
   Non-Common weights are boosted by the player's Item Rarity attribute
   (capped at +100% regardless of how much IIR is stacked).

   Common items get no affixes. Legendary items always roll the full budget.

--------------------------------------------------------------------------------
  PART 4 — CREATING A FRAGMENT TEMPLATE  (recommended, do once per item type)
--------------------------------------------------------------------------------

A Fragment Template is a reusable preset of fully-configured fragments. Once
created, you stamp it onto any item actor with a single button click instead of
adding fragments manually each time.

1. Content Browser -> right-click -> Miscellaneous -> Data Asset ->
   FP_ItemFragmentTemplate.
   Suggested naming: DA_Template_Weapon_Ranged, DA_Template_Headwear, etc.

2. Open it and expand the Fragments array. Click + to add fragments.
   For a standard equippable, a typical template includes:

   a) FFP_RarityFragment
      - RarityTable : set to your DA_RarityTable_Default asset
      - FragmentTag : Fragment.Rarity

   b) FFP_AffixFragment
      - AffixPool   : set to the appropriate DA_Pool_{SlotName} asset
                      (e.g. DA_Pool_Weapon_Ranged for a ranged weapon)
      - Registry    : set to DA_AffixRegistry
      - ItemLevel   : leave at 1 as a default — the loot spawner overrides this
      - FragmentTag : Fragment.Affixes

   c) FFP_ItemLevelFragment
      - FragmentTag : Fragment.ItemLevel

   d) FFP_GridFragment
      - GridSize    : e.g. (2, 4) for a rifle
      - FragmentTag : Fragment.Grid

   e) FFP_ImageFragment
      - Icon        : assign your item icon texture
      - FragmentTag : Fragment.Image

   f) FFP_TextFragment  (item name)
      - FragmentText: e.g. "Combat Rifle"
      - FragmentTag : Fragment.Name

   Add any other fragments your item type needs (FFP_AttributeRequirementFragment,
   FFP_MeshFragment, etc.).

3. Save the template asset.

--------------------------------------------------------------------------------
  PART 5 — SETTING UP AN ITEM ACTOR
--------------------------------------------------------------------------------

1. Create (or open) a Blueprint that inherits from AFP_ItemActor.

2. In the Class Defaults Details panel, under "Item | Setup":
   - Set FragmentTemplate to your template asset from Part 4.
   - Click "Apply Fragment Template".
     This stamps all fragments from the template onto the item's manifest,
     skipping any fragment whose FragmentTag is already present (so hand-edits
     are never overwritten by a re-apply).

3. Under "Item", verify the manifest now shows the expected fragments on
   the ItemComponent -> Item Manifest -> Fragments array.

4. If you need to override a specific value (e.g. a unique item name or a
   different pool for this specific actor), expand the relevant fragment in
   the Fragments array and change the field directly. Re-applying the template
   will not overwrite it because the tag is already present.

5. Set the item's Mesh and any VFX as usual.

6. Place the actor in the level (or let the loot spawner spawn it).
   On BeginPlay:
     - FFP_RarityFragment rolls the item rarity and determines the affix budget
       (e.g. Rare = 2-3 prefixes, 2-3 suffixes).
     - FFP_AffixFragment reads the rolled budget, picks affixes from the pool
       (respecting GroupTag exclusivity and AffinityTag locking), picks a tier
       per affix weighted by item level, then rolls a final stat value within
       the tier's [Min, Max] range at the defined step.

--------------------------------------------------------------------------------
  PART 6 — DISPLAYING AFFIXES (UI)
--------------------------------------------------------------------------------

The affix leaf widget (UFP_Leaf_Affixes) displays all rolled affixes as
formatted text lines. Prefixes appear above suffixes; within each group the
order matches the pool sheet ordering for cross-item consistency.

1. Create a Widget Blueprint inheriting from FP_Leaf_Affixes.

2. Add a Rich Text Block to the widget canvas and name it exactly:
       RichText_Affixes

3. Assign a Rich Text Style Datatable to the Rich Text Block for styling.
   (Font size and colour are controlled here, not in C++.)

4. Set the widget's FragmentTag to match the FFP_AffixFragment tag on your items,
   e.g. Fragment.Affixes.

5. Place this leaf widget inside your item description composite widget at the
   position where affix lines should appear.

When the player hovers an item, AssimilateInventoryFragments walks the composite
tree, matches the Fragment.Affixes tag, and calls SetAffixes with the sorted
rolled list. The format string (e.g. "+{1} Physical Damage") has {1} and {2}
replaced with the correctly formatted stat values before display.

--------------------------------------------------------------------------------
  QUICK REFERENCE — FULL PIPELINE SUMMARY
--------------------------------------------------------------------------------

  [Excel] Edit AffixDatabase.xlsm
      |
      v
  [Terminal] python export_to_json.py
      |
      v
  [Unreal] Open DA_AffixImporter -> click "Import Affixes"
      |
      v
  [Unreal] Item Actor Blueprint -> set FragmentTemplate -> "Apply Fragment Template"
      |
      v
  [Runtime] BeginPlay -> rarity rolls -> affix budget -> affixes roll -> UI displays

================================================================================
