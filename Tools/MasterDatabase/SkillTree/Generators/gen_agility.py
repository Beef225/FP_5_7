"""Agility passive tree: twin-arrowhead base path + clusters.  python gen_agility.py [--write]"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from treegen import *  # noqa

TREE = Path(r"C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Trees\Passive\Agility\Agility.sktree")
L = Layout()

AGI = flat("Attributes.Primary.Agility", 1, "Agility")
DODGE = plain("Defence.DodgeChance", 0.01, "+1% Chance to Dodge")
CRIT = inc("Critical.StrikeChance.Increased", 0.05, "Critical Strike Chance")
SPEED = inc("Speed.Skill", 0.02, "Skill Speed")

# name: (start, corner, tip-side, node name, effects, icon)
ARMS = {
    "Agility":        ((1410, 1410), (620, 620),   -1, "Agility", [AGI], icon("sprint", "run")),
    "Evasion":        ((1410, 1590), (620, 2380),  -1, "Evasion", [DODGE], icon("dodging", "dodge", "body-swapping")),
    "CriticalChance": ((1590, 1410), (2380, 620),  +1, "Critical Strike Chance", [CRIT], icon("on-target", "bullseye")),
    "SkillSpeed":     ((1590, 1590), (2380, 2380), +1, "Skill Speed", [SPEED], icon("lightning-trio", "fast-forward-button")),
}
TIPS = {-1: ("WindRunner.1", (230, 1500)), +1: ("KillingEdge.1", (2770, 1500))}
L.add("WindRunner.1", "Major", TIPS[-1][1], "Wind Runner",
      [inc("Speed.Movement", 0.06, "Movement Speed"), plain("Defence.DodgeChance", 0.03, "+3% Chance to Dodge"),
       flat("Attributes.Primary.Agility", 2, "Agility")], icon("wind-slap", "whirlwind", "sprint"))
L.add("KillingEdge.1", "Major", TIPS[+1][1], "Killing Edge",
      [inc("Critical.StrikeChance.Increased", 0.15, "Critical Strike Chance"),
       plain("Critical.StrikeMultiplier", 0.20, "20% Critical Strike Multiplier")], icon("crossed-swords", "sword-clash"))

arm_tags = {}
for arm, (start, corner, side, name, effs, ic) in ARMS.items():
    tip_tag, tip = TIPS[side]
    spoke = [start[0] + (corner[0] - start[0]) * k / 6 for k in range(6)], [start[1] + (corner[1] - start[1]) * k / 6 for k in range(6)]
    pts = list(zip(*spoke)) + [corner] + [(corner[0] + (tip[0] - corner[0]) * k / 7, corner[1] + (tip[1] - corner[1]) * k / 7)
                                          for k in range(1, 7)]
    tags = [f"{arm}.{k + 1}" for k in range(13)]
    for k, (t, p) in enumerate(zip(tags, pts)):
        L.add(t, "Minor", p, name, effs, ic, start=(k == 0))
    for k in range(12):
        L.link(tags[k], tags[k + 1])
    L.link(tags[12], tip_tag)
    arm_tags[arm] = tags

base_minors = [t for tags in arm_tags.values() for t in tags[1:]]


def arm(*names):
    return [t for n in names for t in arm_tags[n][1:]]


# ---------------- masteries ----------------
place_single(L, "Blur.1", "Mastery", base_minors, "Blur",
             [flag("Flag.SkillTree.Blur", "Your Chance to Dodge is Doubled"),
              plain("Defence.Max.DodgeChance", 0.10, "+10% to Maximum Chance to Dodge"),
              ("FLAG_TEXT", None, "You cannot Block\nYou have no Armour"),
              red("Health.Increased", -0.15, "Maximum Hit Points")],
             icon("body-swapping", "dodging"), pref_pos=(1500, 820))
place_single(L, "MomentumEngine.1", "Mastery", base_minors, "Momentum Engine",
             [flag("Flag.SkillTree.MovementSpeedToSkillSpeed",
                   "Increases and Reductions to Movement Speed also apply to Skill Speed")],
             icon("speedometer", "fast-forward-button"), pref_pos=(1500, 2180))

C = []


def cluster(group, prefer, minors, major, icon_minor, icon_major, shape="ring"):
    C.append(dict(group=group, prefer=prefer, minors=minors, major=major, icon_minor=icon_minor,
                  icon_major=icon_major, shape=shape))


def crit_wheel(group, weapon, label, prefer, name, ic_minor, ic_major):
    cluster(group, prefer,
            alt(4, (f"{label} Critical Strike Chance", [inc(f"Critical.StrikeChance.Increased.{weapon}", 0.10, f"Critical Strike Chance with {label}s")]),
                   (f"{label} Critical Strike Multiplier", [plain(f"Critical.StrikeMultiplier.{weapon}", 0.15, f"15% Critical Strike Multiplier with {label}s")])),
            (name, [plain(f"Critical.StrikeChance.Base.{weapon}", 0.03, f"+3% Base Critical Strike Chance with {label}s"),
                    plain(f"Critical.StrikeMultiplier.{weapon}", 0.25, f"25% Critical Strike Multiplier with {label}s")]),
            ic_minor, ic_major)


# Swords: fast combos, crits, mobility
cluster("Sword.SwordDamage", arm("Agility"),
        rep(6, "Increased Sword Damage", [inc("Damage.Increased.Sword", 0.08, "Sword Damage")]),
        ("Blade Dancer", [inc("Damage.Increased.Sword", 0.12, "Sword Damage"), more("Damage.More.Sword", 0.05, "Sword Damage")]),
        icon("broadsword", "sword-wound"), icon("sword-spin", "broadsword"))
crit_wheel("Sword.Riposte", "Sword", "Sword", arm("CriticalChance", "Agility"), "Riposte",
           icon("sword-clash", "crossed-swords"), icon("sword-clash", "crossed-swords"))
cluster("Sword.FlowingStrikes", arm("Agility", "SkillSpeed"),
        rep(4, "Skill Speed with Swords", [inc("Speed.Skill.Sword", 0.04, "Skill Speed with Swords")]),
        ("Flowing Strikes", [inc("Speed.Skill.Sword", 0.08, "Skill Speed with Swords"), inc("Speed.Movement", 0.03, "Movement Speed")]),
        icon("sword-spin", "broadsword"), icon("sword-spin", "broadsword"))
cluster("Sword.Duelist", arm("Agility"),
        rep(3, "Sword Physical Penetration", [plain("Penetration.Physical.Sword", 0.02, "2% Physical Resistance Penetration with Swords")]),
        ("Duelist", [plain("Penetration.Armour.Sword", 0.05, "5% Armour Penetration with Swords"),
                     plain("Penetration.Physical.Sword", 0.05, "5% Physical Resistance Penetration with Swords")]),
        icon("sword-hilt", "broadsword"), icon("sword-hilt", "broadsword"))
cluster("Sword.Ronin", arm("Evasion", "Agility"),
        rep(3, "Sword Physical Damage", [inc("Damage.Increased.Physical.Sword", 0.08, "Physical Damage with Swords")]),
        ("Ronin", [more("Damage.More.Sword", 0.05, "Sword Damage"), plain("Defence.DodgeChance", 0.02, "+2% Chance to Dodge")]),
        icon("sword-brandish", "broadsword"), icon("sword-brandish", "broadsword"))

# Pistols: high attack speed, precision, dual wielding
cluster("Pistol.PistolDamage", arm("SkillSpeed"),
        rep(6, "Increased Pistol Damage", [inc("Damage.Increased.Pistol", 0.08, "Pistol Damage")]),
        ("Gunslinger", [inc("Damage.Increased.Pistol", 0.12, "Pistol Damage"), more("Damage.More.Pistol", 0.05, "Pistol Damage")]),
        icon("pistol-gun", "revolver"), icon("revolver", "pistol-gun"))
cluster("Pistol.FanTheHammer", arm("SkillSpeed"),
        rep(4, "Skill Speed with Pistols", [inc("Speed.Skill.Pistol", 0.04, "Skill Speed with Pistols")]),
        ("Fan the Hammer", [inc("Speed.Skill.Pistol", 0.08, "Skill Speed with Pistols"),
                            red("Heat.SkillCost.Pistol", -0.10, "Heat Cost with Pistols")]),
        icon("bullets"), icon("revolver", "pistol-gun"))
crit_wheel("Pistol.Deadshot", "Pistol", "Pistol", arm("SkillSpeed", "CriticalChance"), "Deadshot",
           icon("bullseye"), icon("headshot", "bullseye"))
cluster("Pistol.Outrider", arm("SkillSpeed", "Evasion"),
        rep(3, "Pistol Projectile Speed", [inc("Speed.Projectile.Pistol", 0.08, "Projectile Speed with Pistols")]),
        ("Outrider", [inc("Speed.Projectile.Pistol", 0.15, "Projectile Speed with Pistols"), inc("Speed.Movement", 0.03, "Movement Speed")]),
        icon("supersonic-bullet"), icon("cowboy-holster", "supersonic-bullet"))
cluster("Pistol.HollowPoints", arm("SkillSpeed"),
        rep(3, "Pistol Armour Penetration", [plain("Penetration.Armour.Pistol", 0.02, "2% Armour Penetration with Pistols")]),
        ("Hollow Points", [plain("Penetration.Armour.Pistol", 0.05, "5% Armour Penetration with Pistols"),
                           inc("Damage.Increased.Physical.Pistol", 0.15, "Physical Damage with Pistols")]),
        icon("heavy-bullets"), icon("heavy-bullets"))

# Rifles (Might / Agility): versatile soldier
cluster("Rifle.RifleDamage", arm("CriticalChance"),
        rep(6, "Increased Rifle Damage", [inc("Damage.Increased.Rifle", 0.07, "Rifle Damage")]),
        ("Commando", [inc("Damage.Increased.Rifle", 0.10, "Rifle Damage"), more("Damage.More.Rifle", 0.05, "Rifle Damage")]),
        icon("ak47", "m3-grease-gun", "rifle"), icon("ak47", "rifle"))
cluster("Rifle.Ranger", arm("CriticalChance"),
        alt(4, ("Rifle Projectile Speed", [inc("Speed.Projectile.Rifle", 0.08, "Projectile Speed with Rifles")]),
               ("Rifle Critical Strike Chance", [inc("Critical.StrikeChance.Increased.Rifle", 0.10, "Critical Strike Chance with Rifles")])),
        ("Ranger", [inc("Speed.Projectile.Rifle", 0.15, "Projectile Speed with Rifles"),
                    plain("Critical.StrikeMultiplier.Rifle", 0.25, "25% Critical Strike Multiplier with Rifles")]),
        icon("supersonic-bullet"), icon("crosshair-arrow", "bullseye"))
cluster("Rifle.BurstFire", arm("SkillSpeed", "CriticalChance"),
        rep(3, "Skill Speed with Rifles", [inc("Speed.Skill.Rifle", 0.04, "Skill Speed with Rifles")]),
        ("Burst Fire", [inc("Speed.Skill.Rifle", 0.08, "Skill Speed with Rifles"), inc("Damage.Increased.Rifle", 0.08, "Rifle Damage")]),
        icon("bullets"), icon("bullets"))

# Spears (Agility / Fortitude): reach, positioning, hit-and-run
cluster("Spear.HitAndRun", arm("Evasion"),
        alt(4, ("Movement Speed", [inc("Speed.Movement", 0.02, "Movement Speed")]),
               ("Increased Spear Damage", [inc("Damage.Increased.Spear", 0.06, "Spear Damage")])),
        ("Hit and Run", [inc("Speed.Movement", 0.04, "Movement Speed"), plain("Defence.DodgeChance", 0.03, "+3% Chance to Dodge")]),
        icon("thrown-spear"), icon("thrown-spear"))
cluster("Spear.Lancer", arm("Evasion"),
        rep(4, "Increased Spear Damage", [inc("Damage.Increased.Spear", 0.08, "Spear Damage")]),
        ("Lancer", [more("Damage.More.Spear", 0.04, "Spear Damage"),
                    plain("Critical.StrikeMultiplier.Spear", 0.25, "25% Critical Strike Multiplier with Spears")]),
        icon("spears"), icon("barbed-spear"))
cluster("Spear.Dragoon", arm("Evasion", "Agility"),
        rep(3, "Spear Critical Strike Chance", [inc("Critical.StrikeChance.Increased.Spear", 0.10, "Critical Strike Chance with Spears")]),
        ("Dragoon", [plain("Critical.StrikeChance.Base.Spear", 0.02, "+2% Base Critical Strike Chance with Spears"),
                     inc("AoE.Multiplier.Spear", 0.15, "Area of Effect with Spears")]),
        icon("spear-feather", "spears"), icon("spear-feather", "spears"))

# Precision weapons (Agility / Resonance): snipers, exploit weakness
cluster("Precision.PrecisionDamage", arm("CriticalChance"),
        rep(6, "Increased Precision Weapon Damage", [inc("Damage.Increased.Precision", 0.08, "Precision Weapon Damage")]),
        ("Marksman", [inc("Damage.Increased.Precision", 0.12, "Precision Weapon Damage"), more("Damage.More.Precision", 0.05, "Precision Weapon Damage")]),
        icon("crosshair-arrow", "bullseye"), icon("crosshair-arrow", "bullseye"))
cluster("Precision.ExploitWeakness", arm("CriticalChance", "SkillSpeed"),
        alt(4, ("Precision Physical Penetration", [plain("Penetration.Physical.Precision", 0.02, "2% Physical Resistance Penetration with Precision Weapons")]),
               ("Precision Energy Penetration", [plain("Penetration.Energy.Precision", 0.02, "2% Energy Resistance Penetration with Precision Weapons")])),
        ("Exploit Weakness", [plain("Penetration.Armour.Precision", 0.05, "5% Armour Penetration with Precision Weapons"),
                              plain("Penetration.Physical.Precision", 0.05, "5% Physical Resistance Penetration with Precision Weapons"),
                              plain("Penetration.Energy.Precision", 0.05, "5% Energy Resistance Penetration with Precision Weapons")]),
        icon("cracked-shield", "broken-shield"), icon("cracked-shield", "broken-shield"))
cluster("Precision.Ghost", arm("CriticalChance", "Evasion"),
        rep(3, "Precision Critical Strike Multiplier", [plain("Critical.StrikeMultiplier.Precision", 0.15, "15% Critical Strike Multiplier with Precision Weapons")]),
        ("Ghost", [plain("Critical.StrikeChance.Base.Precision", 0.03, "+3% Base Critical Strike Chance with Precision Weapons"),
                   plain("Defence.DodgeChance", 0.02, "+2% Chance to Dodge")]),
        icon("hood", "cloak-dagger"), icon("hood", "cloak-dagger"))
cluster("Precision.KineticRounds", arm("SkillSpeed", "CriticalChance"),
        rep(3, "Precision Projectile Speed", [inc("Speed.Projectile.Precision", 0.10, "Projectile Speed with Precision Weapons")]),
        ("Kinetic Rounds", [inc("Speed.Projectile.Precision", 0.15, "Projectile Speed with Precision Weapons"),
                            plain("Conversion.ProjectileSpeedToDamage.Precision", 0.5,
                                  "50% of Increases and Reductions to Projectile Speed also apply to Damage with Precision Weapons")]),
        icon("supersonic-bullet"), icon("laser-sparks", "supersonic-bullet"))

cluster("FleetArsenal", arm("Agility"),
        rep(5, "Agility", [AGI]),
        ("Fleet Arsenal", [more("Damage.More.AgilityWeapon", 0.08, "Damage with Weapons that Require Agility")]),
        icon("sprint", "run"), icon("crossed-swords", "sword-clash"))

# Defence / mobility / crit
cluster("EvasiveManeuvers", arm("Evasion"),
        rep(4, "Chance to Dodge", [plain("Defence.DodgeChance", 0.015, "+1.5% Chance to Dodge")]),
        ("Evasive Maneuvers", [plain("Defence.DodgeChance", 0.04, "+4% Chance to Dodge")]),
        icon("dodging", "body-swapping"), icon("dodging", "body-swapping"))
cluster("FleetFooted", arm("Agility", "Evasion"),
        rep(3, "Movement Speed", [inc("Speed.Movement", 0.03, "Movement Speed")]),
        ("Fleet Footed", [inc("Speed.Movement", 0.06, "Movement Speed")]),
        icon("running-shoe", "boots"), icon("winged-leg", "running-shoe"))
cluster("Acrobatics", arm("Evasion"),
        rep(3, "Dodge and Hit Points", [plain("Defence.DodgeChance", 0.01, "+1% Chance to Dodge"), inc("Health.Increased", 0.03, "Maximum Hit Points")]),
        ("Acrobatics", [plain("Defence.DodgeChance", 0.03, "+3% Chance to Dodge"), inc("Health.Increased", 0.06, "Maximum Hit Points")]),
        icon("body-swapping", "dodging"), icon("body-swapping", "dodging"))
cluster("KeenEye", arm("CriticalChance"),
        rep(4, "Critical Strike Chance", [inc("Critical.StrikeChance.Increased", 0.06, "Critical Strike Chance")]),
        ("Keen Eye", [plain("Critical.StrikeChance.Base", 0.01, "+1% Base Critical Strike Chance")]),
        icon("eyeball"), icon("eyeball"))
cluster("Lethality", arm("CriticalChance", "SkillSpeed"),
        rep(3, "Critical Strike Multiplier", [plain("Critical.StrikeMultiplier", 0.10, "10% Critical Strike Multiplier")]),
        ("Lethality", [plain("Critical.StrikeMultiplier", 0.25, "25% Critical Strike Multiplier")]),
        icon("pointy-sword", "broadsword"), icon("death-zone", "skull-crossed-bones"))
cluster("Composure", arm("Evasion", "Agility"),
        rep(2, "Critical Strike Resistance", [red("Defense.CriticalHitResistance", 0.08, "Extra Damage Taken from Critical Strikes")]),
        ("Composure", [red("Defense.CriticalHitResistance", 0.20, "Extra Damage Taken from Critical Strikes"),
                       plain("Defence.DodgeChance", 0.02, "+2% Chance to Dodge")]),
        icon("meditation"), icon("meditation"), shape="line")
cluster("QuickenedReflexes", arm("SkillSpeed"),
        rep(3, "Skill Speed", [inc("Speed.Skill", 0.03, "Skill Speed")]),
        ("Quickened Reflexes", [inc("Speed.Skill", 0.06, "Skill Speed")]),
        icon("lightning-trio", "fast-forward-button"), icon("reflex", "lightning-trio"))
cluster("Adrenaline", arm("SkillSpeed", "Evasion"),
        rep(3, "Hit Point Regeneration", [plain("Defence.HealthRegeneration", 0.003, "0.3% of Hit Points Regenerated per Second")]),
        ("Adrenaline", [plain("Defence.HealthRegeneration", 0.01, "1% of Hit Points Regenerated per Second"), inc("Speed.Skill", 0.04, "Skill Speed")]),
        icon("heart-beats", "heart-bottle"), icon("syringe", "heart-beats"))
cluster("SteadyAim", arm("CriticalChance", "SkillSpeed"),
        rep(4, "Increased Projectile Damage", [inc("Damage.Increased.Projectile", 0.06, "Projectile Damage")]),
        ("Steady Aim", [more("Damage.More.Projectile", 0.05, "Projectile Damage")]),
        icon("arrow-flights", "bullets"), icon("crosshair-arrow", "bullseye"))
cluster("Momentum", arm("Agility", "SkillSpeed"),
        rep(3, "Projectile Speed", [inc("Speed.Projectile", 0.08, "Projectile Speed")]),
        ("Momentum", [inc("Speed.Projectile", 0.15, "Projectile Speed"), inc("Speed.Movement", 0.03, "Movement Speed")]),
        icon("supersonic-bullet"), icon("comet-spark", "supersonic-bullet"))
cluster("LightweightWeave", arm("Agility", "Evasion"),
        rep(3, "Energy and Chemical Resistance", [res("Energy", 0.04), res("Chemical", 0.04)]),
        ("Lightweight Weave", [maxres("Energy", 0.02), maxres("Chemical", 0.02), plain("Defence.DodgeChance", 0.02, "+2% Chance to Dodge")]),
        icon("layered-armor"), icon("layered-armor"))

for c in C:
    place_cluster(L, c, c["prefer"], base_minors)

print(f"{len(L.nodes)} nodes, {len(L.edges)} links")
if "--write" in sys.argv:
    print("written", write(L, TREE))
