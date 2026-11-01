"""Generate Fortitude passive-tree clusters around the existing base path (base nodes are never moved).

Usage:  python gen_fortitude.py [--write]
Without --write it only lays out, checks overlaps/crossings and prints a report.
"""
import math
import sys
from pathlib import Path

sys.path.insert(0, r"C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\SkillTreeEditor")
from skilltree_editor.model import Effect  # noqa: E402
from skilltree_editor.project import Project  # noqa: E402

TREE = Path(r"C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Trees\Passive\Fortitude\Fortitude.sktree")
ICON_ROOT = Path(r"C:\Unreal\FP_5_7\Content\Assets\UIElements\Icons\game-icons_net_png\icons\ffffff\000000\1x1")
ICON_UE = "/Game/Assets/UIElements/Icons/game-icons_net_png/icons/ffffff/000000/1x1"
RADIUS = {"Minor": 22.0, "Major": 36.0, "Mastery": 62.5}


def icon(name: str) -> str:
    hits = sorted(ICON_ROOT.glob(f"*/{name}.uasset"))
    if not hits:
        raise SystemExit(f"missing icon {name}")
    return f"{ICON_UE}/{hits[0].parent.name}/{name}.{name}"


def pct(v: float) -> str:
    return f"{abs(v) * 100:g}%"


# ---- effect helpers: (attribute, magnitude, description line) -------------------------
def inc(attr, v, what):      return (attr, v, f"{pct(v)} Increased {what}")
def more(attr, v, what):     return (attr, v, f"{pct(v)} More {what}")
def red(attr, v, what):      return (attr, v, f"{pct(v)} Reduced {what}")
def flat(attr, v, what):     return (attr, v, f"+{v:g} {what}")
def plain(attr, v, text):    return (attr, v, text)
def flag(tag, text):         return ("FLAG:" + tag, None, text)

RES = ("Physical", "Explosive", "Radiation", "Chemical", "Energy")


def res(kind, v):  return plain(f"Resistance.Damage.{kind}", v, f"{pct(v)} {kind} Resistance")
def maxres(kind, v): return plain(f"Resistance.Max.{kind}", v, f"+{pct(v)} Maximum {kind} Resistance")


# ---- cluster spec ---------------------------------------------------------------------
# group: tag group under PassiveTree. ; anchors: base tags the first minor links to
# centre: cluster centre ; minors: list of (name, [effects]) ; major: (name, [effects])
# shape: "ring" (minors on an arc round the centre, major in the middle) | "line" (straight out)
C = []


def cluster(group, anchor, centre, minors, major, icon_minor, icon_major, shape="ring", sweep=280, cw=True):
    C.append(dict(group=group, anchor=anchor, centre=centre, minors=minors, major=major,
                  icon_minor=icon_minor, icon_major=icon_major, shape=shape, sweep=sweep, cw=cw))


def rep(n, name, effects):
    return [(name, effects)] * n


def alt(n, a, b):
    return [a if i % 2 == 0 else b for i in range(n)]


# ======================= GAUNTLETS (left, Hit Points ring) ==============================
cluster("Gauntlets.GauntletDamage", "IncreasedHitPoints.7", (540, 760),
        rep(6, "Increased Gauntlets Damage", [inc("Damage.Increased.Gauntlets", 0.08, "Gauntlets Damage")]),
        ("Iron Knuckles", [inc("Damage.Increased.Gauntlets", 0.12, "Gauntlets Damage"),
                           more("Damage.More.Gauntlets", 0.05, "Gauntlets Damage")]),
        "brass-knuckles", "mailed-fist")
cluster("Gauntlets.Haymaker", "IncreasedHitPoints.6", (540, 1060),
        alt(4, ("Gauntlets Critical Strike Chance", [inc("Critical.StrikeChance.Increased.Gauntlets", 0.10, "Critical Strike Chance with Gauntlets")]),
               ("Gauntlets Critical Strike Multiplier", [plain("Critical.StrikeMultiplier.Gauntlets", 0.15, "15% Critical Strike Multiplier with Gauntlets")])),
        ("Haymaker", [plain("Critical.StrikeChance.Base.Gauntlets", 0.03, "+3% Base Critical Strike Chance with Gauntlets"),
                      plain("Critical.StrikeMultiplier.Gauntlets", 0.25, "25% Critical Strike Multiplier with Gauntlets")]),
        "punch", "thor-fist")
cluster("Gauntlets.Flurry", "IncreasedHitPoints.8", (540, 1360),
        rep(4, "Skill Speed with Gauntlets", [inc("Speed.Skill.Gauntlets", 0.04, "Skill Speed with Gauntlets")]),
        ("Flurry of Blows", [inc("Speed.Skill.Gauntlets", 0.08, "Skill Speed with Gauntlets"),
                             inc("Damage.Increased.Gauntlets", 0.10, "Gauntlets Damage")]),
        "fist", "punch-blast")
cluster("Gauntlets.BoneBreaker", "IncreasedHitPoints.9", (540, 1640),
        alt(4, ("Bruised Effectiveness", [inc("Debuff.Effectiveness.Bruised", 0.06, "Bruised Debuff Effectiveness")]),
               ("Gauntlets Damage Against Bruised", [inc("Conditional.Damage.IncreasedAgainstBruised.Gauntlets", 0.15, "Damage with Gauntlets Against Bruised Enemies")])),
        ("Bone Breaker", [inc("Debuff.Effectiveness.Bruised", 0.15, "Bruised Debuff Effectiveness"),
                          inc("Conditional.Damage.IncreasedAgainstBruised.Gauntlets", 0.24, "Damage with Gauntlets Against Bruised Enemies")]),
        "broken-bone", "shatter")
cluster("Gauntlets.ShellCracker", "IncreasedHitPoints.7", (225, 950),
        rep(3, "Gauntlets Armour Penetration", [plain("Penetration.Armour.Gauntlets", 0.02, "2% Armour Penetration with Gauntlets")]),
        ("Shell Cracker", [plain("Penetration.Armour.Gauntlets", 0.05, "5% Armour Penetration with Gauntlets"),
                           plain("Penetration.Physical.Gauntlets", 0.05, "5% Physical Resistance Penetration with Gauntlets")]),
        "armor-downgrade", "armor-downgrade")

# ======================= AXE (left, Fortitude spoke) ====================================
cluster("Axe.Hewing", "Fortitude.1", (1090, 1480),
        rep(6, "Increased Axe Damage", [inc("Damage.Increased.Axe", 0.08, "Axe Damage")]),
        ("Heartwood Cleaver", [inc("Damage.Increased.Axe", 0.12, "Axe Damage"),
                               more("Damage.More.Axe", 0.05, "Axe Damage")]),
        "wood-axe", "axe-in-log")
cluster("Axe.Sunder", "IncreasedHitPoints.3", (950, 1000),
        rep(4, "Axe Armour Penetration", [plain("Penetration.Armour.Axe", 0.02, "2% Armour Penetration with Axes")]),
        ("Sunder", [plain("Penetration.Armour.Axe", 0.05, "5% Armour Penetration with Axes"),
                    plain("Penetration.Physical.Axe", 0.05, "5% Physical Resistance Penetration with Axes")]),
        "battered-axe", "sharp-axe")
cluster("Axe.Bloodletting", "Fortitude.2", (830, 1630),
        alt(4, ("Axe Damage Over Time", [inc("DoT.Increased.Axe", 0.08, "Damage Over Time with Axes")]),
               ("Axe Physical Damage", [inc("Damage.Increased.Physical.Axe", 0.08, "Physical Damage with Axes")])),
        ("Bloodletting", [inc("DoT.Increased.Axe", 0.15, "Damage Over Time with Axes"),
                          inc("Debuff.Effectiveness.Bruised", 0.10, "Bruised Debuff Effectiveness")]),
        "bleeding-heart", "bleeding-wound")
cluster("Axe.Headsman", "Fortitude.4", (640, 1890),
        rep(3, "Axe Critical Strike Multiplier", [plain("Critical.StrikeMultiplier.Axe", 0.15, "15% Critical Strike Multiplier with Axes")]),
        ("Headsman", [plain("Execute.Threshold.Axe", 0.03, "Kill Enemies with less than 3% of Life Remaining with Axes"),
                      plain("Critical.StrikeMultiplier.Axe", 0.20, "20% Critical Strike Multiplier with Axes")]),
        "war-axe", "chopped-skull")

# ======================= LIFE (left) ====================================================
cluster("Vitality", "IncreasedHitPoints.4", (850, 760),
        rep(4, "Increased Hit Points", [inc("Health.Increased", 0.04, "Maximum Hit Points")]),
        ("Vital Reserves", [more("Health.More", 0.05, "Maximum Hit Points")]),
        "health-increase", "crowned-heart")
cluster("ThickSkin", "IncreasedHitPoints.2", (1120, 1240),
        rep(2, "Increased Hit Points", [inc("Health.Increased", 0.05, "Maximum Hit Points")]),
        ("Thick Skin", [inc("Health.Increased", 0.10, "Maximum Hit Points"),
                        red("Defense.CriticalHitResistance", 0.10, "Extra Damage Taken from Critical Strikes")]),
        "rhinoceros-horn", "rhinoceros-horn", shape="line")
cluster("MendingTissue", "IncreasedHitPoints.8", (200, 1420),
        rep(3, "Hit Point Regeneration", [plain("Defence.HealthRegeneration", 0.004, "0.4% of Hit Points Regenerated per Second")]),
        ("Mending Tissue", [plain("Defence.HealthRegeneration", 0.01, "1% of Hit Points Regenerated per Second"),
                            inc("Health.Increased", 0.05, "Maximum Hit Points")]),
        "heart-bottle", "regeneration")

# ======================= SHOTGUN (right, Armour ring) ====================================
cluster("Shotgun.ShotgunDamage", "IncreasedArmour.9", (2460, 760),
        rep(6, "Increased Shotgun Damage", [inc("Damage.Increased.Shotgun", 0.08, "Shotgun Damage")]),
        ("Twelve Gauge", [inc("Damage.Increased.Shotgun", 0.12, "Shotgun Damage"),
                          more("Damage.More.Shotgun", 0.05, "Shotgun Damage")]),
        "shotgun", "sawed-off-shotgun", cw=False)
cluster("Shotgun.PointBlank", "IncreasedArmour.6", (2460, 1060),
        alt(4, ("Shotgun Critical Strike Chance", [inc("Critical.StrikeChance.Increased.Shotgun", 0.10, "Critical Strike Chance with Shotguns")]),
               ("Shotgun Critical Strike Multiplier", [plain("Critical.StrikeMultiplier.Shotgun", 0.15, "15% Critical Strike Multiplier with Shotguns")])),
        ("Point Blank", [plain("Critical.StrikeChance.Base.Shotgun", 0.03, "+3% Base Critical Strike Chance with Shotguns"),
                         plain("Critical.StrikeMultiplier.Shotgun", 0.25, "25% Critical Strike Multiplier with Shotguns")]),
        "bullseye", "bullseye", cw=False)
cluster("Shotgun.KnockdownRounds", "IncreasedArmour.7", (2460, 1360),
        alt(4, ("Concussed Effectiveness", [inc("Debuff.Effectiveness.Concussed", 0.06, "Concussed Debuff Effectiveness")]),
               ("Shotgun Damage Against Concussed", [inc("Conditional.Damage.IncreasedAgainstConcussed.Shotgun", 0.15, "Damage with Shotguns Against Concussed Enemies")])),
        ("Knockdown Rounds", [inc("Debuff.Effectiveness.Concussed", 0.15, "Concussed Debuff Effectiveness"),
                              inc("Conditional.Damage.IncreasedAgainstConcussed.Shotgun", 0.24, "Damage with Shotguns Against Concussed Enemies")]),
        "shatter", "earth-crack", cw=False)
cluster("Shotgun.Buckshot", "IncreasedArmour.8", (2460, 1640),
        rep(4, "Shotgun Physical Damage", [inc("Damage.Increased.Physical.Shotgun", 0.10, "Physical Damage with Shotguns")]),
        ("Buckshot", [inc("Damage.Increased.Physical.Shotgun", 0.20, "Physical Damage with Shotguns"),
                      plain("Penetration.Physical.Shotgun", 0.05, "5% Physical Resistance Penetration with Shotguns")]),
        "bullets", "heavy-bullets", cw=False)
cluster("Shotgun.PumpAction", "IncreasedArmour.9", (2775, 950),
        rep(3, "Skill Speed with Shotguns", [inc("Speed.Skill.Shotgun", 0.04, "Skill Speed with Shotguns")]),
        ("Pump Action", [inc("Speed.Skill.Shotgun", 0.08, "Skill Speed with Shotguns"),
                         red("Heat.SkillCost.Shotgun", -0.10, "Heat Cost with Shotguns")]),
        "bullets", "shotgun", cw=False)

# ======================= SPEAR (right, Threshold spoke) =================================
cluster("Spear.SpearDamage", "AllThreshold.2", (1910, 1480),
        rep(6, "Increased Spear Damage", [inc("Damage.Increased.Spear", 0.08, "Spear Damage")]),
        ("Impaler", [inc("Damage.Increased.Spear", 0.12, "Spear Damage"),
                     more("Damage.More.Spear", 0.05, "Spear Damage")]),
        "spears", "barbed-spear", cw=False)
cluster("Spear.Skewer", "IncreasedArmour.3", (2050, 1000),
        rep(4, "Spear Physical Penetration", [plain("Penetration.Physical.Spear", 0.02, "2% Physical Resistance Penetration with Spears")]),
        ("Skewer", [plain("Penetration.Armour.Spear", 0.05, "5% Armour Penetration with Spears"),
                    plain("Penetration.Physical.Spear", 0.05, "5% Physical Resistance Penetration with Spears")]),
        "spear-hook", "spear-hook", cw=False)
cluster("Spear.Lunge", "AllThreshold.3", (2170, 1630),
        alt(4, ("Spear Critical Strike Chance", [inc("Critical.StrikeChance.Increased.Spear", 0.10, "Critical Strike Chance with Spears")]),
               ("Skill Speed with Spears", [inc("Speed.Skill.Spear", 0.04, "Skill Speed with Spears")])),
        ("Lunge", [plain("Critical.StrikeChance.Base.Spear", 0.03, "+3% Base Critical Strike Chance with Spears"),
                   inc("Speed.Movement", 0.03, "Movement Speed")]),
        "thrown-spear", "thrown-spear", cw=False)
cluster("Spear.SweepingArc", "AllThreshold.4", (2360, 1890),
        rep(3, "Spear Area of Effect", [inc("AoE.Multiplier.Spear", 0.08, "Area of Effect with Spears")]),
        ("Sweeping Arc", [inc("AoE.Multiplier.Spear", 0.15, "Area of Effect with Spears"),
                          inc("Damage.Increased.AoE.Spear", 0.10, "Area Damage with Spears")]),
        "harpoon-trident", "trident", cw=False)
cluster("Spear.Phalanx", "IncreasedArmour.7", (2800, 1420),
        alt(3, ("Block Chance", [plain("Defence.BlockChance", 0.02, "+2% Chance to Block")]),
               ("Increased Spear Damage", [inc("Damage.Increased.Spear", 0.06, "Spear Damage")])),
        ("Phalanx", [plain("Defence.BlockChance", 0.04, "+4% Chance to Block"),
                     inc("Defense.IncreasedArmour", 0.15, "Armour")]),
        "trident-shield", "trident-shield", cw=False)

# ======================= ARMOUR (right) ==================================================
cluster("ReinforcedPlating", "IncreasedArmour.4", (2150, 760),
        rep(4, "Increased Armour", [inc("Defense.IncreasedArmour", 0.08, "Armour")]),
        ("Reinforced Plating", [inc("Defense.IncreasedArmour", 0.15, "Armour"),
                                red("Defense.CriticalHitResistance", 0.10, "Extra Damage Taken from Critical Strikes")]),
        "armor-upgrade", "layered-armor", cw=False)
cluster("PlatedHide", "IncreasedArmour.2", (1880, 1240),
        rep(2, "Increased Armour", [inc("Defense.IncreasedArmour", 0.10, "Armour")]),
        ("Plated Hide", [inc("Defense.IncreasedArmour", 0.20, "Armour")]),
        "metal-scales", "scale-mail", shape="line")

# ======================= PROJECTOR (bottom) ==============================================
cluster("Projector.ProjectorDamage", "Fortitude.5", (870, 2200),
        rep(6, "Increased Projector Damage", [inc("Damage.Increased.Projector", 0.08, "Projector Damage")]),
        ("Focused Emitter", [inc("Damage.Increased.Projector", 0.12, "Projector Damage"),
                             more("Damage.More.Projector", 0.05, "Projector Damage")]),
        "light-projector", "ringed-beam")
cluster("Projector.Saturation", "Fortitude.11", (1070, 2420),
        rep(4, "Projector Damage Over Time", [inc("DoT.Increased.Projector", 0.08, "Damage Over Time with Projectors")]),
        ("Lingering Field", [inc("DoT.Increased.Projector", 0.15, "Damage Over Time with Projectors"),
                             inc("Duration.Skill.Projector", 0.10, "Skill Duration with Projectors")]),
        "beams-aura", "beams-aura")
cluster("Projector.WideAperture", "Fortitude.12", (1270, 2590),
        rep(4, "Projector Area of Effect", [inc("AoE.Multiplier.Projector", 0.08, "Area of Effect with Projectors")]),
        ("Wide Aperture", [flat("AoE.AdditionalRadius.Projector", 2, "Additional Area Radius with Projectors"),
                           inc("AoE.Multiplier.Projector", 0.10, "Area of Effect with Projectors")]),
        "sinusoidal-beam", "sinusoidal-beam")
cluster("Projector.HeatExchanger", "AllThreshold.5", (2130, 2200),
        alt(4, ("Reduced Projector Heat Cost", [red("Heat.SkillCost.Projector", -0.10, "Heat Cost with Projectors")]),
               ("Overheat Resistance", [plain("Resistance.Damage.Overheat", 0.04, "4% Overheat Resistance")])),
        ("Heat Exchanger", [red("Heat.SkillCost.Projector", -0.15, "Heat Cost with Projectors"),
                            plain("Resistance.Max.Overheat", 0.03, "+3% Maximum Overheat Resistance")]),
        "thermometer-scale", "thermometer-scale", cw=False)
cluster("Projector.CausticLens", "AllThreshold.11", (1930, 2420),
        alt(4, ("Projector Chemical Damage", [inc("Damage.Increased.Chemical.Projector", 0.08, "Chemical Damage with Projectors")]),
               ("Projector Chemical Penetration", [plain("Penetration.Chemical.Projector", 0.02, "2% Chemical Resistance Penetration with Projectors")])),
        ("Caustic Lens", [inc("Debuff.Effectiveness.Corroded", 0.15, "Corroded Debuff Effectiveness"),
                          plain("Penetration.Chemical.Projector", 0.05, "5% Chemical Resistance Penetration with Projectors")]),
        "bubbling-beam", "bubbling-beam", cw=False)
cluster("UnyieldingArsenal", "AllThreshold.12", (1730, 2590),
        rep(5, "Fortitude", [flat("Attributes.Primary.Fortitude", 1, "Fortitude")]),
        ("Unyielding Arsenal", [more("Damage.More.FortitudeWeapon", 0.08, "Damage with Weapons that Require Fortitude")]),
        "muscle-up", "anvil-impact", cw=False)

# ======================= RESISTANCES / BLOCK / MISC =====================================
cluster("HazardSuit", "Fortitude.1", (1230, 2010),
        rep(3, "Chemical and Radiation Resistance", [res("Chemical", 0.04), res("Radiation", 0.04)]),
        ("Hazard Suit", [maxres("Chemical", 0.03), maxres("Radiation", 0.03)]),
        "gas-mask", "radioactive")
cluster("InsulatedWeave", "AllThreshold.2", (1770, 2010),
        rep(3, "Energy and Overheat Resistance", [res("Energy", 0.04), plain("Resistance.Damage.Overheat", 0.04, "4% Overheat Resistance")]),
        ("Insulated Weave", [maxres("Energy", 0.03), plain("Resistance.Max.Overheat", 0.03, "+3% Maximum Overheat Resistance")]),
        "lightning-shield", "lightning-shield", cw=False)
cluster("BlastPadding", "IncreasedHitPoints.13", (1330, 500),
        rep(3, "Physical and Explosive Resistance", [res("Physical", 0.04), res("Explosive", 0.04)]),
        ("Blast Padding", [maxres("Physical", 0.03), maxres("Explosive", 0.03)]),
        "trench-body-armor", "trench-body-armor")
cluster("AdaptiveShielding", "IncreasedArmour.13", (1670, 500),
        rep(3, "All Resistances", [res(k, 0.02) for k in RES]),
        ("Adaptive Shielding", [res(k, 0.05) for k in RES]),
        "energy-shield", "cross-shield", cw=False)
cluster("ShieldWall", "IncreasedHitPoints.4", (1290, 760),
        rep(3, "Block Chance", [plain("Defence.BlockChance", 0.02, "+2% Chance to Block")]),
        ("Shield Wall", [plain("Defence.BlockChance", 0.04, "+4% Chance to Block"),
                         plain("Defence.BlockedDamage", 0.05, "+5% of Damage Prevented when Blocking")]),
        "roman-shield", "shield-impact")
cluster("Unflinching", "IncreasedArmour.4", (1710, 760),
        rep(2, "Critical Strike Resistance", [red("Defense.CriticalHitResistance", 0.08, "Extra Damage Taken from Critical Strikes")]),
        ("Unflinching", [red("Defense.CriticalHitResistance", 0.20, "Extra Damage Taken from Critical Strikes"),
                         flat("Debuff.Threshold.Bruised", 5, "Bruised Threshold"),
                         flat("Debuff.Threshold.Concussed", 5, "Concussed Threshold")]),
        "iron-mask", "iron-mask", shape="line")
cluster("ThermalBallast", "IncreasedHitPoints.12", (980, 150),
        rep(3, "Thermal Mass", [flat("Heat.ThermalMass", 10, "Thermal Mass")]),
        ("Thermal Ballast", [flat("Heat.ThermalMass", 25, "Thermal Mass"),
                             plain("Resistance.Damage.Overheat", 0.05, "5% Overheat Resistance")]),
        "stone-block", "anvil", shape="hline")
cluster("Stalwart", "IncreasedArmour.12", (2020, 150),
        rep(3, "Fortitude", [flat("Attributes.Primary.Fortitude", 1, "Fortitude")]),
        ("Stalwart", [flat("Attributes.Primary.Fortitude", 3, "Fortitude")]),
        "muscle-up", "stone-bust", shape="hline", cw=False)

# ======================= SINGLE NODES ===================================================
SINGLES = [
    # (group, type, pos, links, name, effects, icon)
    ("IronConstitution", "Major", (1500, 1980), ["Fortitude.3", "AllThreshold.1"], "Iron Constitution",
     [flat("Debuff.Threshold.Bruised", 8, "Bruised Threshold"), flat("Debuff.Threshold.Concussed", 8, "Concussed Threshold"),
      flat("Debuff.Threshold.Irradiated", 8, "Irradiated Threshold"), flat("Debuff.Threshold.Corroded", 8, "Corroded Threshold")],
     "shield-echoes"),
    ("LastBastion", "Major", (230, 1725), ["IncreasedHitPoints.9", "Fortitude.8"], "Last Bastion",
     [inc("Health.Increased", 0.08, "Maximum Hit Points"), flat("Attributes.Primary.Fortitude", 2, "Fortitude")],
     "locked-fortress"),
    ("Ironclad", "Major", (2770, 1725), ["IncreasedArmour.8", "AllThreshold.8"], "Ironclad",
     [inc("Defense.IncreasedArmour", 0.15, "Armour"), flat("Debuff.Threshold.Bruised", 5, "Bruised Threshold"),
      flat("Debuff.Threshold.Concussed", 5, "Concussed Threshold")],
     "armoured-shell"),
    ("IronFlesh", "Mastery", (1500, 1050), ["IncreasedHitPoints.2", "IncreasedArmour.2"], "Iron Flesh",
     [flag("Flag.SkillTree.ArmourFromMaxHitPoints", "Gain Armour equal to 20% of your Maximum Hit Points"),
      red("Health.Increased", -0.10, "Maximum Hit Points")],
     "metal-scales"),
    ("Unstoppable", "Mastery", (1500, 2610), ["Fortitude.12", "AllThreshold.12"], "Unstoppable",
     [flag("Flag.SkillTree.ImmuneToBruisedAndConcussed", "You cannot be Bruised or Concussed"),
      red("Speed.Movement", -0.10, "Movement Speed")],
     "charging-bull"),
]

# ---------------------------------------------------------------------------------------


def make_effects(effs):
    out = []
    for attr, mag, _ in effs:
        if attr.startswith("FLAG:"):
            tag = attr[5:]
            out.append(Effect("Effect Flag", {"EffectFlagType": tag}))
        else:
            out.append(Effect("Stat Mod", {"EffectStatModAttributeType": attr, "EffectStatModifierOp": "Add",
                                           "EffectStatMagnitude": mag}))
    return out


def desc(effs):
    return "\n".join(t for _, _, t in effs)


def layout(c, base_pos):
    """Return list of (x, y) for minors then major."""
    cx, cy = c["centre"]
    ax, ay = base_pos[c["anchor"]]
    n = len(c["minors"])
    if c["shape"] in ("line", "hline"):
        if c["shape"] == "hline":   # horizontal run beside the anchor, away from the canvas centre side
            dx = 1 if c["cw"] else -1   # toward the canvas centre line
            pts = [(ax + dx * (90 + 80 * i), cy) for i in range(n)]
            pts.append((ax + dx * (90 + 80 * n + 10), cy))
            return pts
        vx, vy = cx - ax, cy - ay
        d = math.hypot(vx, vy)
        ux, uy = vx / d, vy / d
        steps = [d - 80 * (n - 1) / 2 - 40 + 80 * i for i in range(n)] + [d - 80 * (n - 1) / 2 - 40 + 80 * n + 10]
        return [(ax + ux * s, ay + uy * s) for s in steps]
    if n <= 3:   # small cluster: minors + major all on the ring (no hub crossing the chords)
        r = 62
        a0 = math.atan2(ay - cy, ax - cx)
        sign = 1 if c["cw"] else -1
        step = math.radians(270) / n
        return [(cx + r * math.cos(a0 + sign * step * i), cy + r * math.sin(a0 + sign * step * i)) for i in range(n + 1)]
    r = 82 if n >= 6 else 74
    a0 = math.atan2(ay - cy, ax - cx)
    sweep = math.radians(c["sweep"])
    sign = 1 if c["cw"] else -1
    pts = [(cx + r * math.cos(a0 + sign * sweep * i / max(n - 1, 1)),
            cy + r * math.sin(a0 + sign * sweep * i / max(n - 1, 1))) for i in range(n)]
    pts.append((cx, cy))
    return pts


def seg_dist(p, a, b):
    (px, py), (ax, ay), (bx, by) = p, a, b
    vx, vy = bx - ax, by - ay
    L = vx * vx + vy * vy
    t = 0 if L == 0 else max(0, min(1, ((px - ax) * vx + (py - ay) * vy) / L))
    return math.hypot(px - (ax + t * vx), py - (ay + t * vy))


def cross(a, b, c, d):
    def o(p, q, r):
        return (q[0] - p[0]) * (r[1] - p[1]) - (q[1] - p[1]) * (r[0] - p[0])
    return (o(a, b, c) * o(a, b, d) < 0) and (o(c, d, a) * o(c, d, b) < 0)


def main(write):
    proj = Project.load(TREE)
    tree = proj.tree
    by_tag = {n.tag: n for n in tree.nodes.values()}
    base_pos = {t.replace("PassiveTree.", ""): (n.x, n.y) for t, n in by_tag.items()}
    existing_groups = {t.replace("PassiveTree.", "").rsplit(".", 1)[0] for t in by_tag}

    new_nodes = []   # (tag, type, x, y, name, effs, icon)
    new_edges = []   # (tagA, tagB) short tags
    for c in C:
        assert c["group"] not in existing_groups, c["group"]
        pts = layout(c, base_pos)
        tags = [f"{c['group']}.{i + 1}" for i in range(len(pts))]
        for i, (name, effs) in enumerate(c["minors"]):
            new_nodes.append((tags[i], "Minor", *pts[i], name, effs, c["icon_minor"]))
        mname, meffs = c["major"]
        new_nodes.append((tags[-1], "Major", *pts[-1], mname, meffs, c["icon_major"]))
        new_edges.append((c["anchor"], tags[0]))
        for i in range(len(tags) - 1):
            new_edges.append((tags[i], tags[i + 1]))
    for group, ntype, (x, y), links, name, effs, ic in SINGLES:
        tag = f"{group}.1"
        new_nodes.append((tag, ntype, x, y, name, effs, ic))
        for l in links:
            new_edges.append((l, tag))

    # ---- checks ----
    pos = dict(base_pos)
    rad = {t.replace("PassiveTree.", ""): RADIUS[n.node_type] for t, n in by_tag.items()}
    for tag, ntype, x, y, *_ in new_nodes:
        pos[tag] = (x, y)
        rad[tag] = RADIUS[ntype]
    all_edges = [(a.tag.replace("PassiveTree.", ""), b.tag.replace("PassiveTree.", ""))
                 for e in tree.edges for a, b in [tuple(tree.nodes[u] for u in e)]] + new_edges
    problems = []
    new_tags = {t[0] for t in new_nodes}
    tags_all = list(pos)
    for i, a in enumerate(tags_all):
        for b in tags_all[i + 1:]:
            if a not in new_tags and b not in new_tags:
                continue
            d = math.dist(pos[a], pos[b])
            if d < rad[a] + rad[b] + 14:
                problems.append(f"overlap {a} / {b}: {d:.0f}")
    for (ea, eb) in all_edges:
        for t in tags_all:
            if t in (ea, eb):
                continue
            if ea not in new_tags and eb not in new_tags and t not in new_tags:
                continue
            if seg_dist(pos[t], pos[ea], pos[eb]) < rad[t] + 6:
                problems.append(f"node {t} on edge {ea}-{eb}")
    for i, (a, b) in enumerate(all_edges):
        for c2, d2 in all_edges[i + 1:]:
            if {a, b} & {c2, d2}:
                continue
            if not ({a, b, c2, d2} & new_tags):
                continue
            if cross(pos[a], pos[b], pos[c2], pos[d2]):
                problems.append(f"edges cross {a}-{b} x {c2}-{d2}")
    cx = cy = 1500
    for tag, ntype, x, y, *_ in new_nodes:
        if math.hypot(x - cx, y - cy) + RADIUS[ntype] > 1500:
            problems.append(f"outside canvas circle {tag} ({x:.0f},{y:.0f})")
    print(f"{len(new_nodes)} new nodes, {len(new_edges)} new links, {len(problems)} problems")
    for p in problems:
        print("  ", p)
    if problems and write:
        raise SystemExit("not writing: fix problems first")

    if write:
        for tag, ntype, x, y, name, effs, ic in new_nodes:
            node = tree.add_node(tag="PassiveTree." + tag, node_type=ntype, x=round(x, 1), y=round(y, 1),
                                 name=name, description=desc(effs), icon=icon(ic), effects=make_effects(effs))
            by_tag[node.tag] = node
        for a, b in new_edges:
            tree.connect(by_tag["PassiveTree." + a].uid, by_tag["PassiveTree." + b].uid)
        proj.save(TREE)
        print("saved", TREE)
    else:
        for _, _, _, _, _, _, ic in new_nodes:
            icon(ic)


if __name__ == "__main__":
    main("--write" in sys.argv)
