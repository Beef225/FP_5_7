# Passive tree generators

First-pass generators for the passive trees (2026-11-01). Run with the editor's venv:

```
cd Tools\MasterDatabase\SkillTree\SkillTreeEditor
.venv\Scripts\python ..\Generators\gen_agility.py            # dry run: lay out + check, print counts
.venv\Scripts\python ..\Generators\gen_agility.py --write    # write .sktree + export .json
.venv\Scripts\python ..\Generators\render_tree.py <tree.sktree> out.png [labels]
```

| File | What it does |
|---|---|
| `treegen.py` | Shared library: effect/description helpers, icon lookup, `Layout` with overlap / edge-crossing / canvas checks, auto-placer for clusters (`place_cluster`) and free nodes (`place_single`), `write()` through the editor's `Project` API. |
| `gen_agility.py` | Agility: twin-arrowhead (◁▷) base path + clusters. |
| `gen_resonance.py` | Resonance: standing-wave base path + clusters. |
| `gen_fortitude.py` | Fortitude: clusters only, hand-placed around the user-built shield base path (adds to the existing tree; refuses to run twice because the groups already exist). |
| `agility_momentum_rework.py` | One-off in-place edit (2026-11-01): Momentum Engine mastery -> small cluster + major, slot becomes Perpetual Motion. Pattern for adding clusters to an existing tree without regenerating it. |
| `add_heat_resist_clusters.py` | In-place edit (2026-11-02): 30 heat-band / cooling / thermal-mass / reduced-efficiency / resistance clusters across all four trees. Adds `incm`/`redm` (Multiply op: 1.10 = 10% increased) usage. Refuses to run twice. |
| `migrate_cooling_to_retention.py` | One-off (2026-11-02): cooling nodes moved from Multiply on `Heat.CoolingCoefficient` to Add on `Heat.Retention`. |
| `render_tree.py` | Headless render of a tree to PNG for checking layouts. |

**Warning:** `gen_agility.py --write` and `gen_resonance.py --write` *replace the whole tree* (base path included). Any edits made in the
editor since are lost — use them for reference / new trees, or copy cluster definitions into a new script that only adds nodes.

Conventions (from the Might tree): 4 base arms of ~13 stat nodes with start nodes near the centre and 2 majors at the far ends;
7-node weapon damage chains (6 × 8% + major 12% inc / 5% more), 5-node wheels, 3–4 node small clusters; weapon-gated stats only on
offence (gates read the attacker's skill tags). Weapon → stat map: `Tools/pics4Claude/weapon types.png`.
