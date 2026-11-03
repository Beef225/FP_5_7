r"""2026-11-02: one-off. Cooling nodes from add_heat_resist_clusters.py used Multiply on Heat.CoolingCoefficient;
cooling is now tuned by Heat.Retention (cooling rate / (1 + value), see Source/.../FP_HeatModel.h).
  Multiply m on CoolingCoefficient  ->  Add (1 - m) on Heat.Retention
  "X% Increased Cooling Coefficient" -> "X% Reduced Heat Retention";  "X% Reduced Cooling Coefficient" -> "X% Heat Retention"
Run from SkillTreeEditor:  .venv\Scripts\python ..\Generators\migrate_cooling_to_retention.py [--write]
"""
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from treegen import Project  # noqa

TREES = Path(r"C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Trees\Passive")
for name in ("Resonance", "Fortitude", "Might", "Agility"):
    path = TREES / name / f"{name}.sktree"
    proj = Project.load(path)
    changed = 0
    for n in proj.tree.nodes.values():
        hit = False
        for e in n.effects:
            v = e.values
            if v.get("EffectStatModAttributeType") == "Heat.CoolingCoefficient" and v.get("EffectStatModifierOp") == "Multiply":
                v["EffectStatModAttributeType"] = "Heat.Retention"
                v["EffectStatModifierOp"] = "Add"
                v["EffectStatMagnitude"] = round(1.0 - float(v["EffectStatMagnitude"]), 4)
                hit = True
        if hit:
            d = re.sub(r"(\d+(?:\.\d+)?%) Increased Cooling Coefficient", r"\1 Reduced Heat Retention", n.description)
            n.description = re.sub(r"(\d+(?:\.\d+)?%) Reduced Cooling Coefficient", r"\1 Heat Retention", d)
            if n.name == "Cooling Coefficient":
                n.name = "Cooling Rate"
            changed += 1
    print(f"{name}: {changed} nodes migrated")
    if "--write" in sys.argv and changed:
        proj.save(path)
