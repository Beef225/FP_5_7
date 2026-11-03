r"""2026-11-02: heat-band / cooling / thermal-mass / cost-efficiency / resistance clusters on all four passive trees.
Edits the EXISTING trees in place (keeps manual edits), same pattern as agility_momentum_rework.py.
Run from SkillTreeEditor with the venv:
    .venv\\Scripts\\python ..\\Generators\\add_heat_resist_clusters.py            # dry run: place + report
    .venv\\Scripts\\python ..\\Generators\\add_heat_resist_clusters.py --write    # save .sktree + .json

Heat stats (bases: MaxHeat ~100, MinHeat ~-100, ThermalMass 40, CoolingCoefficient 0.02):
  Thermal model (Source/FP_5_7/Public/AbilitySystem/FP_HeatModel.h): Thermal Mass is a buffer, skill heat and
  cooling are both x 40 / ThermalMass; cooling rate / (1 + Heat.Retention), retention floored at -0.9.
  (Cooling nodes were first written as Multiply on CoolingCoefficient; migrate_cooling_to_retention.py moved them.)
  Percent heat mods use the Multiply op (incm/redm: 1.10 = 10% increased, 0.90 = 10% reduced; several add up).
  Heat cost efficiency below 0 raises costs: Paid = cost / (1 + efficiency), efficiency floored at -0.9.
Representation (heat): Resonance > Fortitude > Might = Agility. Resistances follow the damage-type alignment.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from treegen import *  # noqa

TREES = Path(r"C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Trees\Passive")
BASE_GROUPS = {
    "Agility":   ("Agility", "Evasion", "CriticalChance", "SkillSpeed"),
    "Fortitude": ("IncreasedHitPoints", "IncreasedArmour", "Fortitude", "AllThreshold"),
    "Might":     ("Might", "IncreasedMelee", "BruisedConcussedThreshold", "DeferredDamage"),
    "Resonance": ("Resonance", "HeatEfficiency", "ElementalDamage", "StatusEffectiveness"),
}

MAXH, MINH = "Attributes.Vital.MaxHeatThreshold", "Attributees.VitalMinHeatThreshold"
MASS, RET, EFF = "Heat.ThermalMass", "Heat.Retention", "Heat.SkillCostEfficiency"


def maxh(v):   return flat(MAXH, v, "to Maximum Heat Threshold")
def minh(v):   return plain(MINH, v, f"{v:+g} to Minimum Heat Threshold")          # negative = colder floor
def maxh_red(v): return redm(MAXH, v, "Maximum Heat Threshold")
def cool_inc(v): return plain(RET, -v, f"{pct(v)} Reduced Heat Retention")   # faster cooling
def cool_red(v): return plain(RET, v, f"{pct(v)} Heat Retention")             # slower cooling
def mass_inc(v): return incm(MASS, v, "Thermal Mass")
def mass_red(v): return redm(MASS, v, "Thermal Mass")
def mass(v):   return flat(MASS, v, "Thermal Mass")
def eff_red(v): return plain(EFF, -v, f"{pct(v)} Reduced Heat Cost Efficiency")
def eff(v):    return plain(EFF, v, f"{pct(v)} Heat Cost Efficiency")
def dmg(kind, v): return inc(f"Damage.Increased.{kind}", v, f"{kind} Damage")


def C(group, minors, major, icons, near=None):
    return dict(group=group, shape="ring", minors=minors, major=major,
                icon_minor=icon(*icons[0]), icon_major=icon(*icons[1]), near=near)


CLUSTERS = {
    # ------------------------------------------------------------------ RESONANCE (most heat)
    "Resonance": [
        C("ThermalHeadroom", rep(3, "Maximum Heat", [maxh(5)]),
          ("Thermal Headroom", [maxh(15), res("Overheat", 0.05)]),
          (("thermometer-scale",), ("thermometer-hot",)), "HeatEfficiency"),
        C("DeepFreeze", rep(3, "Minimum Heat", [minh(-5)]),
          ("Deep Freeze", [minh(-15), cool_red(0.05)]),
          (("snowflake-1",), ("frozen-orb",)), "HeatEfficiency"),
        C("Volatility", rep(3, "Volatile Core", [maxh_red(0.05), dmg("Energy", 0.03)]),
          ("Volatility", [maxh_red(0.12), minh(5), dmg("Energy", 0.08), dmg("Chemical", 0.08), dmg("Radiation", 0.08)]),
          (("lightning-flame",), ("heat-haze",)), "ElementalDamage"),
        C("ThermalRunaway", rep(3, "Runaway Reaction", [eff_red(0.05), dmg("Radiation", 0.03)]),
          ("Thermal Runaway", [eff_red(0.15), dmg("Energy", 0.10), dmg("Chemical", 0.10), dmg("Radiation", 0.10)]),
          (("fire-ring",), ("fireball",)), "ElementalDamage"),
        C("Radiator", rep(3, "Cooling Coefficient", [cool_inc(0.08)]),
          ("Radiator", [cool_inc(0.20), eff(0.05)]),
          (("steam",), ("steam-blast",)), "HeatEfficiency"),
        C("HeatRetention", rep(3, "Heat Retention", [cool_red(0.08)]),
          ("Heat Retention", [cool_red(0.20), maxh(10)]),
          (("fireplace",), ("furnace",)), "Resonance"),
        C("LeadLining", rep(3, "Radiation Resistance", [res("Radiation", 0.04)]),
          ("Lead Lining", [maxres("Radiation", 0.02), res("Radiation", 0.08)]),
          (("sun-radiations",), ("lead-pipe",)), "StatusEffectiveness"),
        C("Chelation", rep(3, "Chemical Resistance", [res("Chemical", 0.04)]),
          ("Chelation", [maxres("Chemical", 0.02), res("Chemical", 0.08)]),
          (("chemical-drop",), ("chemical-tank",)), "StatusEffectiveness"),
        C("Capacitance", rep(3, "Energy Resistance", [res("Energy", 0.04)]),
          ("Capacitance", [maxres("Energy", 0.02), res("Energy", 0.06), res("Radiation", 0.04)]),
          (("lightning-shield",), ("lightning-dissipation",)), "Resonance"),
    ],
    # ------------------------------------------------------------------ FORTITUDE (next most heat, increased mass)
    "Fortitude": [
        C("ThermalPlating", rep(3, "Maximum Heat", [maxh(5)]),
          ("Thermal Plating", [maxh(15), inc("Defense.IncreasedArmour", 0.10, "Armour")]),
          (("thermometer-scale",), ("armor-upgrade",)), "IncreasedArmour"),
        C("ColdIron", rep(3, "Minimum Heat", [minh(-5)]),
          ("Cold Iron", [minh(-15), mass(10)]),
          (("ice-cube",), ("anvil",)), "Fortitude"),
        C("DenseCore", rep(3, "Thermal Mass", [mass(5)]),
          ("Dense Core", [mass_inc(0.15), inc("Health.Increased", 0.05, "Maximum Hit Points")]),
          (("stone-block",), ("stone-sphere",)), "IncreasedHitPoints"),
        C("HeavyFrame", rep(3, "Increased Thermal Mass", [mass_inc(0.06)]),
          ("Heavy Frame", [mass(20), inc("Defense.IncreasedArmour", 0.10, "Armour")]),
          (("metal-bar",), ("weight-crush",)), "IncreasedArmour"),
        C("Coolant", rep(3, "Cooling Coefficient", [cool_inc(0.06)]),
          ("Coolant", [cool_inc(0.15), res("Overheat", 0.05)]),
          (("fire-extinguisher",), ("ice-shield",)), "Fortitude"),
        C("Insulation", rep(3, "Heat Retention", [cool_red(0.06)]),
          ("Insulation", [cool_red(0.15), maxh(10)]),
          (("fire-shield",), ("heavy-helm",)), "AllThreshold"),
        C("Heatproof", rep(3, "Overheat Resistance", [res("Overheat", 0.04)]),
          ("Heatproof", [maxres("Overheat", 0.02), res("Overheat", 0.08)]),
          (("fire-shield",), ("firewall",)), "AllThreshold"),
        C("Fortified", rep(3, "All Resistances", [res(k, 0.02) for k in ("Physical", "Explosive", "Radiation", "Chemical", "Energy")]),
          ("Fortified", [maxres(k, 0.01) for k in ("Physical", "Explosive", "Radiation", "Chemical", "Energy")]),
          (("shieldcomb",), ("shield-reflect",)), "IncreasedHitPoints"),
    ],
    # ------------------------------------------------------------------ MIGHT (least heat; physical / explosive / some energy)
    "Might": [
        C("Furnace", rep(3, "Stoked Furnace", [eff_red(0.05), dmg("Physical", 0.03)]),
          ("Furnace", [eff_red(0.15), dmg("Physical", 0.10), dmg("Explosive", 0.10)]),
          (("fire-bowl",), ("furnace",)), "Might"),
        C("HeatShielding", rep(2, "Maximum Heat", [maxh(5)]),
          ("Heat Shielding", [maxh(10), res("Overheat", 0.05)]),
          (("thermometer-scale",), ("fire-shield",)), "DeferredDamage"),
        C("Ventilation", rep(2, "Cooling Coefficient", [cool_inc(0.06)]),
          ("Ventilation", [cool_inc(0.15)]),
          (("steam",), ("steam-locomotive",)), "IncreasedMelee"),
        C("Smoulder", rep(2, "Heat Retention", [cool_red(0.06)]),
          ("Smoulder", [cool_red(0.15), dmg("Explosive", 0.08)]),
          (("fire-zone",), ("lava",)), "Might"),
        C("Hardened", rep(3, "Physical Resistance", [res("Physical", 0.04)]),
          ("Hardened", [maxres("Physical", 0.02), res("Physical", 0.08)]),
          (("metal-scales",), ("armor-vest",)), "BruisedConcussedThreshold"),
        C("BlastShielding", rep(3, "Explosive Resistance", [res("Explosive", 0.04)]),
          ("Blast Shielding", [maxres("Explosive", 0.02), res("Explosive", 0.08)]),
          (("blast",), ("explosion-rays",)), "DeferredDamage"),
        C("Grounding", rep(2, "Energy Resistance", [res("Energy", 0.04)]),
          ("Grounding", [maxres("Energy", 0.01), res("Energy", 0.06)]),
          (("lightning-arc",), ("lightning-shield",)), "IncreasedMelee"),
    ],
    # ------------------------------------------------------------------ AGILITY (least heat, reduced mass; mixed resistances)
    "Agility": [
        C("Featherweight", rep(3, "Reduced Thermal Mass", [mass_red(0.08), inc("Speed.Movement", 0.02, "Movement Speed")]),
          ("Featherweight", [mass_red(0.20), inc("Speed.Movement", 0.05, "Movement Speed")]),
          (("feather",), ("feathered-wing",)), "Agility"),
        C("AirCooled", rep(2, "Cooling Coefficient", [cool_inc(0.06)]),
          ("Air Cooled", [cool_inc(0.15), inc("Speed.Movement", 0.03, "Movement Speed")]),
          (("sun-cloud",), ("heavy-rain",)), "SkillSpeed"),
        C("Afterburn", rep(2, "Heat Retention", [cool_red(0.06)]),
          ("Afterburn", [cool_red(0.15), inc("Speed.Skill", 0.04, "Skill Speed")]),
          (("fire-dash",), ("fire-tail",)), "SkillSpeed"),
        C("ThermalTolerance", alt(2, ("Maximum Heat", [maxh(5)]), ("Minimum Heat", [minh(-5)])),
          ("Thermal Tolerance", [maxh(10), minh(-10)]),
          (("thermometer-scale",), ("frostfire",)), "Evasion"),
        C("ShrapnelDodger", alt(3, ("Physical Resistance", [res("Physical", 0.04)]), ("Explosive Resistance", [res("Explosive", 0.04)])),
          ("Shrapnel Dodger", [maxres("Physical", 0.01), maxres("Explosive", 0.01), plain("Defence.DodgeChance", 0.02, "+2% Chance to Dodge")]),
          (("heavy-bullets",), ("explosion-rays",)), "Evasion"),
        C("Scrubbers", alt(3, ("Radiation Resistance", [res("Radiation", 0.04)]), ("Chemical Resistance", [res("Chemical", 0.04)])),
          ("Scrubbers", [maxres("Radiation", 0.01), maxres("Chemical", 0.01), inc("Speed.Movement", 0.03, "Movement Speed")]),
          (("biohazard",), ("acid-tube",)), "CriticalChance"),
    ],
}


def add_clusters(tree_name, clusters, write):
    path = TREES / tree_name / f"{tree_name}.sktree"
    proj = Project.load(path)
    tree = proj.tree
    by_tag = {n.tag.replace("PassiveTree.", ""): n for n in tree.nodes.values()}
    base = BASE_GROUPS[tree_name]

    L = Layout()
    uid_tag = {}
    for t, n in by_tag.items():
        L.add(t, n.node_type, (n.x, n.y), n.name, [], n.icon, start=n.is_start)
        if t.rsplit(".", 1)[0] not in base:
            L.cluster_nodes.add(t)
        uid_tag[n.uid] = t
    for e in tree.edges:
        a, b = (uid_tag[u] for u in e)
        L.link(a, b)
        for x, y in ((a, b), (b, a)):
            if x in L.cluster_nodes and y not in L.cluster_nodes:
                L.anchor_use[y] = L.anchor_use.get(y, 0) + 1
    n_edges_before = len(L.edges)

    base_minors = [t for t, n in by_tag.items() if t.rsplit(".", 1)[0] in base and not n.is_start]
    placed, failed = [], []
    for c in clusters:
        if any(t.startswith(c["group"] + ".") for t in L.nodes):
            raise SystemExit(f"{tree_name}: group {c['group']} already exists (already applied?)")
        prefer = [t for t in base_minors if t.startswith(c["near"] + ".")] if c["near"] else base_minors
        try:
            anchor = place_cluster(L, c, prefer, base_minors)
            placed.append((c["group"], anchor))
        except SystemExit as e:
            failed.append(c["group"])
            print("  !!", e)

    new_tags = [t for t in L.nodes if t not in by_tag]
    for t in new_tags:
        n = L.nodes[t]
        node = tree.add_node(tag="PassiveTree." + t, node_type=n["type"], x=round(n["x"], 1), y=round(n["y"], 1),
                             name=n["name"], description=desc(n["effs"]), icon=n["icon"], effects=make_effects(n["effs"]))
        by_tag[t] = node
    for a, b in L.edges[n_edges_before:]:
        assert tree.connect(by_tag[a].uid, by_tag[b].uid), (a, b)
    print(f"{tree_name}: +{len(new_tags)} nodes in {len(placed)} clusters; failed: {failed or 'none'}")
    for g, a in placed:
        print(f"    {g:18s} <- {a}")
    if write:
        proj.save(path)
        print("    saved")
    return failed


if __name__ == "__main__":
    only = [a for a in sys.argv[1:] if not a.startswith("--")]
    any_failed = False
    for name, clusters in CLUSTERS.items():
        if only and name not in only:
            continue
        any_failed |= bool(add_clusters(name, clusters, "--write" in sys.argv))
    if any_failed:
        print("some clusters could not be placed")
