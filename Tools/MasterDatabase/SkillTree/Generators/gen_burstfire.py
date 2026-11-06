"""Burst Fire skill tree (Skills.Physical.BurstFire), first pass 2026-11-04.

Seven lanes, each with a start node on the left, running right into majors; the Penetrator and
Ricochet lanes meet at the Overpenetration mastery on the right.

    python ..\\Generators\\gen_burstfire.py            # dry run: print the nodes
    python ..\\Generators\\gen_burstfire.py --write    # write Trees/Skills/Physical/BurstFire/BurstFire.sktree + .json

Every effect is a Skill Passive under SkillPassive.BurstFire.*. The C++ reads the mechanic leaves
(UFP_GA_BurstFire); the damage leaves (IncreasedDamage, IncreasedPhysicalDamage, MoreDamage,
IncreasedCritChance, CritMultiplier) go through UFP_DamageGameplayAbility::AssignSkillPassiveBonuses.
--write replaces the whole tree, so don't re-run it after hand edits in the editor.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from treegen import icon  # noqa: E402
from skilltree_editor.model import Effect  # noqa: E402
from skilltree_editor.project import Project, ProjectMeta  # noqa: E402

ROOT = "SkillTree.BurstFire"
SP = "SkillPassive.BurstFire."
OUT = Path(__file__).parents[1] / "Trees" / "Skills" / "Physical" / "BurstFire" / "BurstFire.sktree"

X0, DX = 200.0, 130.0


def sp(leaf, mag, text):
    return (leaf, mag, text)


def pct(v):
    return f"{abs(v) * 100:g}%"


# ---- node content ----------------------------------------------------------------------------
I_PHYS = icon("heavy-bullets")
I_DMG = icon("bullets")
I_HOLLOW = icon("silver-bullet")
I_PIERCE = icon("pierced-body")
I_AP = icon("supersonic-bullet")
I_CHAIN_R = icon("return-arrow")
I_RICO = icon("zig-arrow")
I_INTERVAL = icon("speedometer")
I_ROUND = icon("ammo-box")
I_SPREAD = icon("on-target")
I_FOCUS = icon("targeted")
I_COUP = icon("headshot")
I_VEL = icon("hypersonic-bolt")
I_FRAG = icon("fragmented-meteor")
I_CRIT = icon("target-shot")
I_CRITM = icon("crosshair-arrow")
I_VOLLEY = icon("split-arrows")
I_MASTERY = icon("piercing-sword")

PHYS = ("Calibre", [sp("IncreasedPhysicalDamage", 0.08, "8% Increased Physical Damage")])
DMG = ("Calibre", [sp("IncreasedDamage", 0.08, "8% Increased Damage")])
HOLLOW = ("Hollow Points", [sp("BruisedBuildup", 15.0, "Rounds build up Bruised (15 per hit)"),
                            sp("IncreasedPhysicalDamage", 0.10, "10% Increased Physical Damage")])
PIERCE_DMG = ("Penetrator", [sp("PierceDamage", 0.05, "5% Less damage lost per target pierced")])
AP = ("Armour-Piercing Rounds", [sp("PierceCount", 1.0, "Rounds pierce 1 additional target")])
CHAIN_R = ("Deflection", [sp("ChainRadius", 1.0, "+1m Ricochet Range")])
CHAIN_DMG = ("Hard Ricochet", [sp("ChainDamage", 0.05, "5% Less damage lost per ricochet")])
RICO = ("Ricochet", [sp("ChainCount", 1.0, "Rounds ricochet to 1 additional nearby enemy")])
INTERVAL = ("Hair Trigger", [sp("BurstInterval", -0.08, "8% Reduced time between rounds")])
ROUND = ("Extended Burst", [sp("ExtraRounds", 1.0, "+1 Round per burst"),
                            sp("MoreDamage", -0.10, "10% Less Damage")])
SPREAD = ("Steady Aim", [sp("Spread", -0.15, "15% Reduced spread")])
FOCUS = ("Focus Fire", [sp("FocusFire", 0.10,
                           "Each round deals 10% More Damage for every earlier round\nof the same burst that hit that enemy")])
CRIT = ("Marksman", [sp("IncreasedCritChance", 0.10, "10% Increased Critical Strike Chance")])
COUP = ("Coup de Grace", [sp("FinalRoundDamage", 0.40, "The last round of each burst deals 40% More Damage")])
VEL = ("Muzzle Velocity", [sp("ProjectileSpeed", 0.10, "10% Increased Projectile Speed")])
FRAG = ("Fragmenting Rounds", [sp("FragmentCount", 2.0,
                                  "Rounds that kill an enemy split into 2 fragments\nFragments deal 50% of the round's damage")])
FRAG_DMG = ("Jagged Fragments", [sp("FragmentDamage", 0.10, "Fragments deal 10% more of the round's damage")])
SHRAPNEL = ("Shrapnel Storm", [sp("FragmentCount", 1.0, "+1 Fragment"),
                               sp("FragmentDamage", 0.10, "Fragments deal 10% more of the round's damage")])
CRITM = ("Lethal Grouping", [sp("CritMultiplier", 0.10, "+10% Critical Strike Multiplier")])
VOLLEY = ("Scatter Volley", [sp("Volley", 1.0, "All rounds fire at once in a fan"),
                             sp("MoreDamage", 0.15, "15% More Damage")])
MASTERY = ("Overpenetration", [sp("Overpenetration", 0.15,
                                  "Rounds gain 15% More Damage for each target they pierce\ninstead of losing damage"),
                               sp("PierceCount", 1.0, "Rounds pierce 1 additional target")])

# lane: (group, y, [(group, type, content, icon), ...]); first entry is the start node
LANES = [
    (110, [("Physical", "Minor", PHYS, I_PHYS), ("Damage", "Minor", DMG, I_DMG), ("Physical", "Minor", PHYS, I_PHYS),
           ("Damage", "Minor", DMG, I_DMG), ("HollowPoints", "Major", HOLLOW, I_HOLLOW)]),
    (220, [("PierceDamage", "Minor", PIERCE_DMG, I_PIERCE), ("PierceDamage", "Minor", PIERCE_DMG, I_PIERCE),
           ("PierceDamage", "Minor", PIERCE_DMG, I_PIERCE), ("PierceCount", "Major", AP, I_AP),
           ("PierceDamage", "Minor", PIERCE_DMG, I_PIERCE), ("PierceDamage", "Minor", PIERCE_DMG, I_PIERCE),
           ("PierceCount", "Major", AP, I_AP)]),
    (330, [("ChainRadius", "Minor", CHAIN_R, I_CHAIN_R), ("ChainRadius", "Minor", CHAIN_R, I_CHAIN_R),
           ("ChainRadius", "Minor", CHAIN_R, I_CHAIN_R), ("ChainCount", "Major", RICO, I_RICO),
           ("ChainDamage", "Minor", CHAIN_DMG, I_CHAIN_R), ("ChainDamage", "Minor", CHAIN_DMG, I_CHAIN_R),
           ("ChainCount", "Major", RICO, I_RICO)]),
    (440, [("BurstInterval", "Minor", INTERVAL, I_INTERVAL), ("BurstInterval", "Minor", INTERVAL, I_INTERVAL),
           ("ExtraRounds", "Major", ROUND, I_ROUND), ("BurstInterval", "Minor", INTERVAL, I_INTERVAL),
           ("BurstInterval", "Minor", INTERVAL, I_INTERVAL), ("ExtraRounds", "Major", ROUND, I_ROUND)]),
    (550, [("Spread", "Minor", SPREAD, I_SPREAD), ("Spread", "Minor", SPREAD, I_SPREAD),
           ("FocusFire", "Major", FOCUS, I_FOCUS), ("CritChance", "Minor", CRIT, I_CRIT),
           ("CritChance", "Minor", CRIT, I_CRIT), ("FinalRound", "Major", COUP, I_COUP)]),
    (660, [("ProjectileSpeed", "Minor", VEL, I_VEL), ("ProjectileSpeed", "Minor", VEL, I_VEL),
           ("ProjectileSpeed", "Minor", VEL, I_VEL), ("Fragment", "Major", FRAG, I_FRAG),
           ("FragmentDamage", "Minor", FRAG_DMG, I_FRAG), ("FragmentDamage", "Minor", FRAG_DMG, I_FRAG),
           ("Shrapnel", "Major", SHRAPNEL, I_FRAG)]),
    (770, [("CritChance", "Minor", CRIT, I_CRIT), ("CritChance", "Minor", CRIT, I_CRIT),
           ("CritMultiplier", "Minor", CRITM, I_CRITM), ("CritMultiplier", "Minor", CRITM, I_CRITM),
           ("Volley", "Major", VOLLEY, I_VOLLEY)]),
]
MASTERY_POS = (1260.0, 275.0)
MASTERY_LINKS = (1, 2)  # last nodes of the Penetrator and Ricochet lanes


def build():
    nodes, edges, counters = [], [], {}
    lane_last = []
    for y, entries in LANES:
        prev = None
        for i, (group, ntype, (name, effs), ic) in enumerate(entries):
            counters[group] = counters.get(group, 0) + 1
            tag = f"{ROOT}.{group}.{counters[group]}"
            nodes.append(dict(tag=tag, node_type=ntype, is_start=(i == 0), name=name,
                              description="\n".join(t for _, _, t in effs), icon=ic,
                              x=X0 + DX * i + (20.0 if ntype == "Major" else 0.0), y=y, effs=effs))
            if prev:
                edges.append((prev, tag))
            prev = tag
        lane_last.append(prev)
    name, effs = MASTERY
    mtag = f"{ROOT}.Mastery.Overpenetration.1"
    nodes.append(dict(tag=mtag, node_type="Mastery", is_start=False, name=name,
                      description="\n".join(t for _, _, t in effs), icon=I_MASTERY,
                      x=MASTERY_POS[0], y=MASTERY_POS[1], effs=effs))
    for lane in MASTERY_LINKS:
        edges.append((lane_last[lane], mtag))
    return nodes, edges


def main():
    nodes, edges = build()
    for n in nodes:
        print(f"{n['node_type']:7} {n['tag']:45} {n['name']:24} {n['description'].splitlines()[0]}")
    print(f"{len(nodes)} nodes, {len(edges)} edges")
    if "--write" not in sys.argv:
        return

    meta = ProjectMeta(name="BurstFire", tree_type="Skill", category="Physical",
                       skill_tag="Skills.Physical.BurstFire", author="jakeg",
                       canvas_width=1800.0, canvas_height=900.0,
                       notes="Generated by Generators/gen_burstfire.py (2026-11-04).")
    proj = Project(meta=meta)
    by = {}
    for n in nodes:
        effects = [Effect("Skill Passive", {"EffectSkillPassiveTag": SP + leaf, "EffectStatMagnitude": mag})
                   for leaf, mag, _ in n["effs"]]
        node = proj.tree.add_node(tag=n["tag"], node_type=n["node_type"], is_start=n["is_start"],
                                  name=n["name"], description=n["description"], icon=n["icon"],
                                  x=n["x"], y=n["y"], effects=effects)
        by[n["tag"]] = node.uid
    for a, b in edges:
        assert proj.tree.connect(by[a], by[b]), (a, b)
    proj.save(OUT)
    print(f"wrote {OUT}")


if __name__ == "__main__":
    main()
