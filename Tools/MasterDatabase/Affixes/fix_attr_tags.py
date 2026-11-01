"""
Fixes the Stat1_Attr / Stat2_Attr columns in AffixDatabase.xlsm so they match
the native gameplay tag names registered in TagsToAttributes.

Run once, then re-run export_to_json.py and re-import in Unreal.
"""

import openpyxl
import pathlib

XLSX = pathlib.Path(__file__).parent / "AffixDatabase.xlsm"
SHEET = "MasterList"
STAT1_COL = 10   # 1-based column index (J)
STAT2_COL = 16   # (P)

# Maps the Excel tag value -> correct native tag that matches TagsToAttributes
TAG_MAP = {
    # Damage - Added (flat)
    "Attributes.Damage.AddedPhysical":          "Damage.Added.Physical",
    "Attributes.Damage.AddedExplosive":         "Damage.Added.Explosive",
    "Attributes.Damage.AddedChemical":          "Damage.Added.Chemical",
    "Attributes.Damage.AddedEnergy":            "Damage.Added.Energy",
    "Attributes.Damage.AddedRadiation":         "Damage.Added.Radiation",
    # Damage - Increased (weapon type)
    "Attributes.Damage.IncreasedPhysical":      "Damage.Increased.Physical",
    "Attributes.Damage.IncreasedExplosive":     "Damage.Increased.Explosive",
    "Attributes.Damage.IncreasedChemical":      "Damage.Increased.Chemical",
    "Attributes.Damage.IncreasedEnergy":        "Damage.Increased.Energy",
    "Attributes.Damage.IncreasedRadiation":     "Damage.Increased.Radiation",
    "Attributes.Damage.IncreasedAoE":           "Damage.Increased.AoE",
    "Attributes.Damage.IncreasedAxe":           "Damage.Increased.Axe",
    "Attributes.Damage.IncreasedClub":          "Damage.Increased.Club",
    "Attributes.Damage.IncreasedDrone":         "Damage.Increased.Drone",
    "Attributes.Damage.IncreasedGrenade":       "Damage.Increased.Grenade",
    "Attributes.Damage.IncreasedLauncher":      "Damage.Increased.Launcher",
    "Attributes.Damage.IncreasedMelee":         "Damage.Increased.Melee",
    "Attributes.Damage.IncreasedPistol":        "Damage.Increased.Pistol",
    "Attributes.Damage.IncreasedProjectile":    "Damage.Increased.Projectile",
    "Attributes.Damage.IncreasedRifle":         "Damage.Increased.Rifle",
    "Attributes.Damage.IncreasedShotgun":       "Damage.Increased.Shotgun",
    "Attributes.Damage.IncreasedSword":         "Damage.Increased.Sword",
    "Attributes.Damage.IncreasedUnarmed":       "Damage.Increased.Unarmed",
    # Defence
    "Attributes.Defence.Armour":                "Defence.Armour",
    "Attributes.Defence.BlockChance":           "Defence.BlockChance",
    "Attributes.Defence.ChanceToDodge":         "Defence.DodgeChance",
    "Attributes.Defence.CriticalHitResistance": "Defense.CriticalHitResistance",
    "Attributes.Defence.DeferredDamageAmount":  "Defence.DeferredDamageAmount",
    "Attributes.Defence.DeferredDamageTime":    "Defence.DeferredDamageTime",
    "Attributes.Defence.HealthRegeneration":    "Defence.HealthRegeneration",
    "Attributes.Defence.IncreasedArmour":       "Defense.IncreasedArmour",
    "Attributes.Defence.ReducedDamageFromAoE":  "Defense.ReducedDamage.AoE",
    "Attributes.Defence.ReducedDamageFromMelee":"Defense.ReduceeDamage.Melee",
    "Attributes.Defence.ReducedDamageFromProjectiles": "Defense.ReducedDamage.Projectiles",
    # Critical
    "Attributes.Critical.Chance":               "Critical.StrikeChance.Base",
    "Attributes.Critical.IncreasedChance":      "Critical.StrikeChance.Increased",
    "Attributes.Critical.Multiplier":           "Critical.StrikeMultiplier",
    # Resistance
    "Attributes.Resistance.Physical":           "Resistance.Damage.Physical",
    "Attributes.Resistance.Explosive":          "Resistance.Damage.Explosive",
    "Attributes.Resistance.Radiation":          "Resistance.Damage.Radiation",
    "Attributes.Resistance.Chemical":           "Resistance.Damage.Chemical",
    "Attributes.Resistance.Energy":             "Resistance.Damage.Energy",
    "Attributes.Resistance.Overheat":           "Resistance.Damage.Overheat",
    "Attributes.Resistance.MaxPhysical":        "Resistance.Max.Physical",
    "Attributes.Resistance.MaxExplosive":       "Resistance.Max.Explosive",
    "Attributes.Resistance.MaxRadiation":       "Resistance.Max.Radiation",
    "Attributes.Resistance.MaxChemical":        "Resistance.Max.Chemical",
    "Attributes.Resistance.MaxEnergy":          "Resistance.Max.Energy",
    "Attributes.Resistance.MaxOverheat":        "Resistance.Max.Overheat",
    # Heat
    "Attributes.Heat.AmbientTemperature":       "Heat.AmbientTemperature",
    "Attributes.Heat.CoolingCoefficient":       "Heat.CoolingCoefficient",
    "Attributes.Heat.ThermalMass":              "Heat.ThermalMass",
    # Debuff
    "Attributes.Debuff.Bruised.Duration":               "Debuff.Duration.Bruised",
    "Attributes.Debuff.Bruised.Effectiveness":          "Debuff.Effectiveness.Bruised",
    "Attributes.Debuff.Bruised.Threshold":              "Debuff.Threshold.Bruised",
    "Attributes.Debuff.Concussed.Duration":             "Debuff.Duration.Concussed",
    "Attributes.Debuff.Concussed.Effectiveness":        "Debuff.Effectiveness.Concussed",
    "Attributes.Debuff.Concussed.Threshold":            "Debuff.Threshold.Concussed",
    "Attributes.Debuff.Corroded.Duration":              "Debuff.Duration.Corroded",
    "Attributes.Debuff.Corroded.Effectiveness":         "Debuff.Effectiveness.Corroded",
    "Attributes.Debuff.Corroded.Threshold":             "Debuff.Threshold.Corroded",
    "Attributes.Debuff.Irradiated.Duration":            "Debuff.Duration.Irradiated",
    "Attributes.Debuff.Irradiated.Effectiveness":       "Debuff.Effectiveness.Irradiated",
    "Attributes.Debuff.Irradiated.Threshold":           "Debuff.Threshold.Irradiated",
    "Attributes.Debuff.ThermalDestabilization.Effectiveness": "Debuff.Effectiveness.ThermalDestabalization",
    "Attributes.Debuff.ThermalInsulation":              "Debuff.ThermalInsulation",
    # Health (secondary)
    "Attributes.Health.Increased":             "Health.Increased",
    "Attributes.Health.More":                  "Health.More",
}

def fix_sheet(ws):
    changed = 0
    unknown = set()
    for row in ws.iter_rows(min_row=2):
        for col_idx in (STAT1_COL, STAT2_COL):
            cell = row[col_idx - 1]
            val = cell.value
            if not val or not isinstance(val, str):
                continue
            if val in TAG_MAP:
                new_val = TAG_MAP[val]
                if new_val != val:
                    cell.value = new_val
                    changed += 1
            elif val and not any(val.startswith(p) for p in ("Attributes.Vital.", "Attributes.Primary.", "Attributees.")):
                unknown.add(val)
    return changed, unknown

wb = openpyxl.load_workbook(XLSX, keep_vba=True)
ws = wb[SHEET]
changed, unknown = fix_sheet(ws)
wb.save(XLSX)

print(f"Fixed {changed} cells in '{SHEET}'")
if unknown:
    print(f"\nWARNING — {len(unknown)} tags not in TAG_MAP (left unchanged):")
    for t in sorted(unknown):
        print(f"  {t}")
else:
    print("All tags resolved — no unknowns.")
