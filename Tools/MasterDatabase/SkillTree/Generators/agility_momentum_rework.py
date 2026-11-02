"""2026-11-01: Momentum Engine mastery -> 2x(+1% movement) -> Major (25% movement speed to skill speed);
the mastery slot becomes Perpetual Motion. Edits the EXISTING Agility tree in place (keeps manual edits).
Run from SkillTreeEditor with the venv:  .venv\\Scripts\\python ..\\Generators\\agility_momentum_rework.py [--write]
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from treegen import *  # noqa
from skilltree_editor.model import Effect  # noqa

TREE = Path(r"C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Trees\Passive\Agility\Agility.sktree")
BASE_GROUPS = ("Agility", "Evasion", "CriticalChance", "SkillSpeed")

proj = Project.load(TREE)
tree = proj.tree
by_tag = {n.tag.replace("PassiveTree.", ""): n for n in tree.nodes.values()}
if "PerpetualMotion.1" in by_tag:
    raise SystemExit("already applied")

# ---- mastery slot: Momentum Engine -> Perpetual Motion ----
m = by_tag.pop("MomentumEngine.1")
pm_effs = [flag("Flag.SkillTree.PerpetualMotion", "While Moving: 20% More Damage and +10% Chance to Dodge"),
           ("FLAG_TEXT", None, "While Stationary: 30% Less Damage and you cannot Dodge")]
m.tag = "PassiveTree.PerpetualMotion.1"
m.name = "Perpetual Motion"
m.description = desc(pm_effs)
m.effects = make_effects(pm_effs)
m.icon = icon("run", "sprint")
by_tag["PerpetualMotion.1"] = m

# ---- rebuild a Layout of the current tree for collision checks, then place the new cluster ----
L = Layout()
uid_tag = {}
for t, n in by_tag.items():
    is_base = t.rsplit(".", 1)[0] in BASE_GROUPS
    L.add(t, n.node_type, (n.x, n.y), n.name, [], n.icon, start=n.is_start)
    if not is_base:
        L.cluster_nodes.add(t)
    uid_tag[n.uid] = t
for e in tree.edges:
    a, b = (uid_tag[u] for u in e)
    L.link(a, b)
    if a in L.cluster_nodes and b not in L.cluster_nodes:
        L.anchor_use[b] = L.anchor_use.get(b, 0) + 1
    if b in L.cluster_nodes and a not in L.cluster_nodes:
        L.anchor_use[a] = L.anchor_use.get(a, 0) + 1
n_edges_before = len(L.edges)

base_minors = [t for t, n in by_tag.items() if t.rsplit(".", 1)[0] in BASE_GROUPS and not n.is_start]
prefer = [t for t in base_minors if t.startswith(("SkillSpeed.", "Agility."))]
place_cluster(L, dict(group="MomentumEngine", shape="ring",
                      minors=rep(2, "Movement Speed", [inc("Speed.Movement", 0.01, "Movement Speed")]),
                      major=("Momentum Engine", [plain("Conversion.MovementSpeedToSkillSpeed", 0.25,
                                                       "25% of Increases and Reductions to Movement Speed also apply to Skill Speed")]),
                      icon_minor=icon("winged-leg", "boots"), icon_major=icon("speedometer")),
              prefer, base_minors)

new_tags = [t for t in L.nodes if t.startswith("MomentumEngine.")]
for t in new_tags:
    n = L.nodes[t]
    node = tree.add_node(tag="PassiveTree." + t, node_type=n["type"], x=round(n["x"], 1), y=round(n["y"], 1),
                         name=n["name"], description=desc(n["effs"]), icon=n["icon"], effects=make_effects(n["effs"]))
    by_tag[t] = node
for a, b in L.edges[n_edges_before:]:
    tree.connect(by_tag[a].uid, by_tag[b].uid)
print("placed:", [(t, round(L.nodes[t]["x"]), round(L.nodes[t]["y"])) for t in new_tags],
      "anchor:", [a for a, b in L.edges[n_edges_before:] if b == "MomentumEngine.1"])
if "--write" in sys.argv:
    proj.save(TREE)
    print("saved")
