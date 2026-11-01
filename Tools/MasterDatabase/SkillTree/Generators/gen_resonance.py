"""Resonance passive tree: standing-wave base path + clusters.  python gen_resonance.py [--write]"""
import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from treegen import *  # noqa

TREE = Path(r"C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\Trees\Passive\Resonance\Resonance.sktree")
A, HALF = 800.0, 600.0          # amplitude, half-wavelength (wave nodes at x = 1500, 900, 300 / 2100, 2700)

L = Layout()

# ---------------- base arms ----------------
RES = flat("Attributes.Primary.Resonance", 1, "Resonance")
HEAT = plain("Heat.SkillCostEfficiency", 0.03, "3% Heat Cost Efficiency")
ELEM = [inc("Damage.Increased.Energy", 0.03, "Energy Damage"), inc("Damage.Increased.Chemical", 0.03, "Chemical Damage"),
        inc("Damage.Increased.Radiation", 0.03, "Radiation Damage")]
STAT = [inc("Debuff.Effectiveness.Irradiated", 0.04, "Irradiated Debuff Effectiveness"),
        inc("Debuff.Effectiveness.Corroded", 0.04, "Corroded Debuff Effectiveness")]

ARMS = {
    # name: (side, sign, node name, effects, icon)
    "Resonance":           (-1, +1, "Resonance", [RES], icon("sound-waves", "radial-balance")),
    "HeatEfficiency":      (-1, -1, "Heat Cost Efficiency", [HEAT], icon("snowflake-2", "snowflake-1", "thermometer-cold")),
    "ElementalDamage":     (+1, -1, "Elemental Damage", ELEM, icon("lightning-arc", "electric", "atom")),
    "StatusEffectiveness": (+1, +1, "Status Effectiveness", STAT, icon("radioactive", "poison-bottle")),
}
wave_tag = {-1: "WaveNode.1", +1: "WaveNode.2"}
end_tag = {-1: "SteadyState.1", +1: "ResonantCascade.1"}
L.add("WaveNode.1", "Minor", (1500 - HALF, 1500), "Wave Node", [RES, HEAT], icon("sinusoidal-beam"))
L.add("WaveNode.2", "Minor", (1500 + HALF, 1500), "Wave Node", ELEM + STAT, icon("sinusoidal-beam"))
L.add("SteadyState.1", "Major", (1500 - 2 * HALF, 1500), "Steady State",
      [flat("Attributes.Primary.Resonance", 3, "Resonance"), plain("Heat.SkillCostEfficiency", 0.08, "8% Heat Cost Efficiency")],
      icon("yin-yang", "radial-balance"))
L.add("ResonantCascade.1", "Major", (1500 + 2 * HALF, 1500), "Resonant Cascade",
      [inc(f"Damage.Increased.{k}", 0.10, f"{k} Damage") for k in ("Energy", "Chemical", "Radiation")] +
      [inc("Debuff.Effectiveness.Irradiated", 0.10, "Irradiated Debuff Effectiveness"),
       inc("Debuff.Effectiveness.Corroded", 0.10, "Corroded Debuff Effectiveness")],
      icon("shock-wave", "explosion-rays"))

arm_tags = {}
for arm, (side, sign, name, effs, ic) in ARMS.items():
    poly = []
    for i in range(0, 1201, 3):
        x = 1500 + side * i
        poly.append((x, 1500 + sign * A * math.sin(math.pi * (x - 1500) / HALF)))
    _, Lc = resample(poly, [0])  # dummy to get total length
    # arc lengths at the crossing (i = 600) and the end (i = 1200)
    cum = [0.0]
    for k in range(1, len(poly)):
        cum.append(cum[-1] + math.dist(poly[k - 1], poly[k]))
    Lcross, Lend = cum[200], cum[-1]
    # 7 nodes per lobe at equal arc length -> the middle one sits on the crest
    ss = [Lcross * k / 8 for k in range(1, 8)] + [Lcross + (Lend - Lcross) * k / 8 for k in range(1, 8)]
    pts, _ = resample(poly, ss)
    tags = [f"{arm}.{k + 1}" for k in range(14)]
    for k, (t, p) in enumerate(zip(tags, pts)):
        L.add(t, "Minor", p, name, effs, ic, start=(k == 0))
    for k in range(6):
        L.link(tags[k], tags[k + 1])
    L.link(tags[6], wave_tag[side])
    L.link(wave_tag[side], tags[7])
    for k in range(7, 13):
        L.link(tags[k], tags[k + 1])
    L.link(tags[13], end_tag[side])
    arm_tags[arm] = tags

base_minors = [t for tags in arm_tags.values() for t in tags[1:]] + ["WaveNode.1", "WaveNode.2"]


def arm(*names):
    return [t for n in names for t in arm_tags[n][1:]]


# ---------------- masteries (inside the inner lobes) ----------------
place_single(L, "Endothermic.1", "Mastery", base_minors, "Endothermic",
             [flag("Flag.SkillTree.Endothermic",
                   "Your Heat starts at its Maximum Threshold and Skills spend Heat instead of generating it\n"
                   "Heat regenerates back towards the Maximum over time\n"
                   "You cannot use Skills while your Heat is below 0")],
             icon("snowflake-2", "snowflake-1"), pref_pos=(1200, 1500))
place_single(L, "CascadeFailure.1", "Mastery", base_minors, "Cascade Failure",
             [flag("Flag.SkillTree.SpreadStatusOnKill",
                   "Enemies that die while Irradiated or Corroded spread those Debuffs to nearby Enemies")],
             icon("chain-reaction", "radioactive"), pref_pos=(1800, 1500))

# ---------------- clusters ----------------
C = []


def cluster(group, prefer, minors, major, icon_minor, icon_major, shape="ring"):
    C.append(dict(group=group, prefer=prefer, minors=minors, major=major, icon_minor=icon_minor,
                  icon_major=icon_major, shape=shape))


# Conduits: energy manipulation, elemental / status
cluster("Conduit.ConduitDamage", arm("ElementalDamage"),
        rep(6, "Increased Conduit Damage", [inc("Damage.Increased.Conduit", 0.08, "Conduit Damage")]),
        ("Technomancer", [inc("Damage.Increased.Conduit", 0.12, "Conduit Damage"), more("Damage.More.Conduit", 0.05, "Conduit Damage")]),
        icon("crystal-wand", "wizard-staff"), icon("crystal-wand"))
cluster("Conduit.ArcFlash", arm("ElementalDamage"),
        rep(4, "Conduit Energy Damage", [inc("Damage.Increased.Energy.Conduit", 0.08, "Energy Damage with Conduits")]),
        ("Arc Flash", [inc("Damage.Increased.Energy.Conduit", 0.15, "Energy Damage with Conduits"),
                       plain("Penetration.Energy.Conduit", 0.05, "5% Energy Resistance Penetration with Conduits")]),
        icon("lightning-arc"), icon("lightning-storm", "lightning-arc"))
cluster("Conduit.Catalyst", arm("StatusEffectiveness"),
        alt(4, ("Corroded Effectiveness", [inc("Debuff.Effectiveness.Corroded", 0.06, "Corroded Debuff Effectiveness")]),
               ("Conduit Damage Against Corroded", [inc("Conditional.Damage.IncreasedAgainstCorroded.Conduit", 0.15, "Damage with Conduits Against Corroded Enemies")])),
        ("Catalyst", [inc("Debuff.Effectiveness.Corroded", 0.15, "Corroded Debuff Effectiveness"),
                      inc("Conditional.Damage.IncreasedAgainstCorroded.Conduit", 0.24, "Damage with Conduits Against Corroded Enemies")]),
        icon("acid"), icon("acid-blob", "acid"))
cluster("Conduit.HalfLife", arm("StatusEffectiveness"),
        alt(4, ("Irradiated Effectiveness", [inc("Debuff.Effectiveness.Irradiated", 0.06, "Irradiated Debuff Effectiveness")]),
               ("Conduit Damage Against Irradiated", [inc("Conditional.Damage.IncreasedAgainstIrradiated.Conduit", 0.15, "Damage with Conduits Against Irradiated Enemies")])),
        ("Half-Life", [inc("Debuff.Effectiveness.Irradiated", 0.15, "Irradiated Debuff Effectiveness"),
                       inc("Conditional.Damage.IncreasedAgainstIrradiated.Conduit", 0.24, "Damage with Conduits Against Irradiated Enemies")]),
        icon("radioactive"), icon("nuclear", "radioactive"))
cluster("Conduit.Channeler", arm("HeatEfficiency"),
        rep(3, "Skill Speed with Conduits", [inc("Speed.Skill.Conduit", 0.04, "Skill Speed with Conduits")]),
        ("Channeler", [inc("Speed.Skill.Conduit", 0.08, "Skill Speed with Conduits"),
                       plain("Heat.SkillCostEfficiency.Conduit", 0.05, "5% Heat Cost Efficiency with Conduits")]),
        icon("magic-swirl", "vortex"), icon("magic-swirl", "vortex"))

# Drone Controllers: summons, turrets, autonomous damage
cluster("DroneController.DroneDamage", arm("Resonance"),
        rep(6, "Increased Drone Controller Damage", [inc("Damage.Increased.DroneController", 0.08, "Drone Controller Damage")]),
        ("Overseer", [inc("Damage.Increased.DroneController", 0.12, "Drone Controller Damage"),
                      more("Damage.More.DroneController", 0.05, "Drone Controller Damage")]),
        icon("delivery-drone", "drone"), icon("spy-drone", "delivery-drone", "drone"))
cluster("DroneController.OverclockedServos", arm("Resonance"),
        rep(4, "Skill Speed with Drone Controllers", [inc("Speed.Skill.DroneController", 0.04, "Skill Speed with Drone Controllers")]),
        ("Overclocked Servos", [inc("Speed.Skill.DroneController", 0.08, "Skill Speed with Drone Controllers"),
                                inc("Duration.Skill.DroneController", 0.10, "Skill Duration with Drone Controllers")]),
        icon("gears", "cog"), icon("processor", "gears"))
cluster("DroneController.TargetingMatrix", arm("Resonance", "HeatEfficiency"),
        alt(4, ("Drone Controller Critical Strike Chance", [inc("Critical.StrikeChance.Increased.DroneController", 0.10, "Critical Strike Chance with Drone Controllers")]),
               ("Drone Controller Critical Strike Multiplier", [plain("Critical.StrikeMultiplier.DroneController", 0.15, "15% Critical Strike Multiplier with Drone Controllers")])),
        ("Targeting Matrix", [plain("Critical.StrikeChance.Base.DroneController", 0.03, "+3% Base Critical Strike Chance with Drone Controllers"),
                              plain("Critical.StrikeMultiplier.DroneController", 0.25, "25% Critical Strike Multiplier with Drone Controllers")]),
        icon("radar-sweep", "crosshair"), icon("radar-sweep", "crosshair"))
cluster("DroneController.SwarmProtocol", arm("Resonance"),
        rep(3, "Drone Controller Area of Effect", [inc("AoE.Multiplier.DroneController", 0.08, "Area of Effect with Drone Controllers")]),
        ("Swarm Protocol", [inc("AoE.Multiplier.DroneController", 0.15, "Area of Effect with Drone Controllers"),
                            inc("Damage.Increased.AoE.DroneController", 0.10, "Area Damage with Drone Controllers")]),
        icon("swarm", "bee"), icon("swarm", "bee"))

# Accelerator weapons (Might / Resonance): railguns, coilguns, particle cannons
cluster("Accelerator.CoilCharge", arm("ElementalDamage"),
        rep(6, "Increased Accelerator Damage", [inc("Damage.Increased.Accelerator", 0.08, "Accelerator Damage")]),
        ("Gravitist", [inc("Damage.Increased.Accelerator", 0.12, "Accelerator Damage"), more("Damage.More.Accelerator", 0.05, "Accelerator Damage")]),
        icon("laser-blast", "supersonic-bullet"), icon("black-hole-bolas", "vortex", "laser-blast"))
cluster("Accelerator.ParticleCannon", arm("ElementalDamage"),
        rep(4, "Accelerator Energy Damage", [inc("Damage.Increased.Energy.Accelerator", 0.08, "Energy Damage with Accelerator Weapons")]),
        ("Particle Cannon", [inc("Damage.Increased.Energy.Accelerator", 0.15, "Energy Damage with Accelerator Weapons"),
                             plain("Penetration.Energy.Accelerator", 0.05, "5% Energy Resistance Penetration with Accelerator Weapons")]),
        icon("laser-sparks", "laser-blast"), icon("laser-sparks", "laser-blast"))
cluster("Accelerator.RailKnight", arm("Resonance", "ElementalDamage"),
        rep(3, "Accelerator Projectile Speed", [inc("Speed.Projectile.Accelerator", 0.08, "Projectile Speed with Accelerator Weapons")]),
        ("Rail Knight", [inc("Speed.Projectile.Accelerator", 0.10, "Projectile Speed with Accelerator Weapons"),
                         plain("Critical.StrikeMultiplier.Accelerator", 0.25, "25% Critical Strike Multiplier with Accelerator Weapons")]),
        icon("supersonic-bullet"), icon("supersonic-bullet"))

# Precision weapons (Agility / Resonance): snipers, beam rifles, exploit weakness
cluster("Precision.BeamRifle", arm("StatusEffectiveness", "ElementalDamage"),
        rep(4, "Precision Energy Damage", [inc("Damage.Increased.Energy.Precision", 0.08, "Energy Damage with Precision Weapons")]),
        ("Beam Rifle", [inc("Damage.Increased.Energy.Precision", 0.15, "Energy Damage with Precision Weapons"),
                        plain("Penetration.Energy.Precision", 0.05, "5% Energy Resistance Penetration with Precision Weapons")]),
        icon("ringed-beam", "laser-sparks"), icon("ringed-beam", "laser-sparks"))
cluster("Precision.Spotter", arm("StatusEffectiveness"),
        rep(3, "Precision Damage Against Afflicted", [inc("Conditional.Damage.IncreasedAgainstIrradiated.Precision", 0.12, "Damage with Precision Weapons Against Irradiated Enemies")]),
        ("Spotter", [inc("Conditional.Damage.IncreasedAgainstIrradiated.Precision", 0.20, "Damage with Precision Weapons Against Irradiated Enemies"),
                     inc("Conditional.Damage.IncreasedAgainstCorroded.Precision", 0.20, "Damage with Precision Weapons Against Corroded Enemies")]),
        icon("bullseye", "crosshair"), icon("sniper-scope", "bullseye", "crosshair"))

# Projectors (Fortitude / Resonance): shields, healing, support, damage-over-time fields
cluster("Projector.Lifeline", arm("HeatEfficiency"),
        rep(4, "Hit Point Regeneration", [plain("Defence.HealthRegeneration", 0.004, "0.4% of Hit Points Regenerated per Second")]),
        ("Lifeline", [plain("Defence.HealthRegeneration", 0.01, "1% of Hit Points Regenerated per Second"),
                      inc("Health.Increased", 0.06, "Maximum Hit Points")]),
        icon("heart-bottle", "regeneration"), icon("healing-shield", "heart-shield"))
cluster("Projector.FieldEmitter", arm("HeatEfficiency"),
        rep(4, "Projector Damage Over Time", [inc("DoT.Increased.Projector", 0.08, "Damage Over Time with Projectors")]),
        ("Field Emitter", [inc("DoT.Increased.Projector", 0.15, "Damage Over Time with Projectors"),
                           inc("Duration.Skill.Projector", 0.15, "Skill Duration with Projectors")]),
        icon("beams-aura"), icon("beams-aura"))
cluster("Projector.HardlightBarrier", arm("HeatEfficiency", "Resonance"),
        rep(3, "Energy and Chemical Resistance", [res("Energy", 0.04), res("Chemical", 0.04)]),
        ("Hardlight Barrier", [maxres("Energy", 0.03), inc("Health.Increased", 0.08, "Maximum Hit Points")]),
        icon("energy-shield", "cross-shield"), icon("bubble-field", "energy-shield"))

cluster("HarmonicArsenal", arm("Resonance"),
        rep(5, "Resonance", [RES]),
        ("Harmonic Arsenal", [more("Damage.More.ResonanceWeapon", 0.08, "Damage with Weapons that Require Resonance")]),
        icon("sound-waves", "radial-balance"), icon("tuning-fork", "sound-waves"))

# Defence / heat / status
cluster("HeatSink", arm("HeatEfficiency"),
        rep(4, "Thermal Mass", [flat("Heat.ThermalMass", 5, "Thermal Mass")]),
        ("Heat Sink", [flat("Heat.ThermalMass", 15, "Thermal Mass"), plain("Resistance.Damage.Overheat", 0.05, "5% Overheat Resistance")]),
        icon("stone-block"), icon("cooler", "anvil"))
cluster("CryogenicLoop", arm("HeatEfficiency"),
        rep(3, "Heat Cost Efficiency", [plain("Heat.SkillCostEfficiency", 0.04, "4% Heat Cost Efficiency")]),
        ("Cryogenic Loop", [plain("Heat.SkillCostEfficiency", 0.10, "10% Heat Cost Efficiency")]),
        icon("snowflake-2", "snowflake-1"), icon("ice-cube", "snowflake-2"))
cluster("FaradayMesh", arm("Resonance"),
        rep(3, "Energy and Radiation Resistance", [res("Energy", 0.04), res("Radiation", 0.04)]),
        ("Faraday Mesh", [maxres("Energy", 0.03), maxres("Radiation", 0.03)]),
        icon("electrical-resistance", "lightning-shield"), icon("lightning-shield"))
cluster("Neutralizer", arm("StatusEffectiveness"),
        rep(3, "Chemical Resistance", [res("Chemical", 0.05)]),
        ("Neutralizer", [maxres("Chemical", 0.03), flat("Debuff.Threshold.Corroded", 10, "Corroded Threshold")]),
        icon("gas-mask"), icon("gas-mask"))
cluster("LingeringToxins", arm("StatusEffectiveness"),
        rep(4, "Status Duration", [flat("Debuff.Duration.Irradiated", 0.25, "Seconds to Irradiated Duration"),
                                   flat("Debuff.Duration.Corroded", 0.25, "Seconds to Corroded Duration")]),
        ("Lingering Toxins", [flat("Debuff.Duration.Irradiated", 1, "Second to Irradiated Duration"),
                              flat("Debuff.Duration.Corroded", 1, "Second to Corroded Duration")]),
        icon("hourglass"), icon("poison-cloud", "poison-gas"))
cluster("StableIsotopes", arm("StatusEffectiveness", "Resonance"),
        rep(3, "Irradiated and Corroded Threshold", [flat("Debuff.Threshold.Irradiated", 5, "Irradiated Threshold"),
                                                     flat("Debuff.Threshold.Corroded", 5, "Corroded Threshold")]),
        ("Stable Isotopes", [flat("Debuff.Threshold.Irradiated", 10, "Irradiated Threshold"),
                             flat("Debuff.Threshold.Corroded", 10, "Corroded Threshold")]),
        icon("shield-echoes"), icon("atom"))
cluster("BreachProtocol", arm("ElementalDamage"),
        rep(3, "Elemental Penetration", [plain(f"Penetration.{k}", 0.015, f"1.5% {k} Resistance Penetration") for k in ("Energy", "Chemical", "Radiation")]),
        ("Breach Protocol", [plain(f"Penetration.{k}", 0.04, f"4% {k} Resistance Penetration") for k in ("Energy", "Chemical", "Radiation")]),
        icon("drill", "hole"), icon("breaking-chain", "drill"))
cluster("Ionize", arm("ElementalDamage"),
        rep(2, "Physical to Energy", [plain("Conversion.Damage.PhysicalToEnergy", 0.10, "10% of Physical Damage Converted to Energy Damage")]),
        ("Ionize", [plain("Conversion.Damage.PhysicalToEnergy", 0.25, "25% of Physical Damage Converted to Energy Damage")]),
        icon("electric", "lightning-arc"), icon("electric", "lightning-arc"), shape="line")
cluster("Overcharge", arm("ElementalDamage"),
        rep(4, "Increased Energy Damage", [inc("Damage.Increased.Energy", 0.06, "Energy Damage")]),
        ("Overcharge", [more("Damage.More.Energy", 0.05, "Energy Damage")]),
        icon("battery-plus", "battery-pack"), icon("battery-pack", "battery-plus"))
cluster("ResonantField", arm("Resonance", "StatusEffectiveness"),
        rep(3, "Area of Effect", [inc("AoE.Multiplier", 0.06, "Area of Effect")]),
        ("Resonant Field", [inc("AoE.Multiplier", 0.12, "Area of Effect"), inc("Damage.Increased.AoE", 0.10, "Area Damage")]),
        icon("concentric-crescents", "ripple"), icon("concentric-crescents", "ripple"))
cluster("Persistence", arm("HeatEfficiency", "Resonance"),
        rep(3, "Skill Duration", [inc("Duration.Skill", 0.05, "Skill Duration")]),
        ("Persistence", [inc("Duration.Skill", 0.12, "Skill Duration")]),
        icon("sands-of-time", "hourglass"), icon("sands-of-time", "hourglass"))

for c in C:
    place_cluster(L, c, c["prefer"], base_minors)

print(f"{len(L.nodes)} nodes, {len(L.edges)} links")
if "--write" in sys.argv:
    print("written", write(L, TREE))
