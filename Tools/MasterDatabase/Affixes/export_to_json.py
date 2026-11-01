"""
Exports AffixDatabase.xlsm -> two JSON files consumed by the Unreal editor utility:

  affixes.json  — keyed by AffixID, each entry contains metadata + all tiers
  pools.json    — keyed by pool sheet name, each entry is an ordered list of pool rows

affixes.json structure:
{
  "AddedPhysicalDamage": {
    "AffixID":        "AddedPhysicalDamage",
    "Type":           "Prefix",
    "GroupTag":       "Group.AddedDamage",
    "DisplayTag":     "Damage, Physical",
    "Stat1_Attr":     "Attributes.Damage.AddedPhysical",
    "Stat2_Attr":     "",
    "DisplayFormat":  "+{1} Physical Damage",
    "Tiers": [
      {
        "Tier":               7,
        "ItemName":           "FORCEFUL",
        "RequiredItemLevel":  1,
        "Weight":             50,
        "Stat1_Min":          1.0,
        "Stat1_Max":          8.0,
        "Stat1_Step":         1.0,
        "Stat1_DecimalCount": 0,
        "Stat1_DisplayAs":    "Flat",
        "Stat2_Min":          null,
        "Stat2_Max":          null,
        "Stat2_Step":         null,
        "Stat2_DecimalCount": null,
        "Stat2_DisplayAs":    null
      }, ...
    ]
  }, ...
}

pools.json structure:
{
  "Weapon_Ranged": [
    { "GroupTag": "Group.AddedDamage", "AffixID": "AddedPhysicalDamage", "PoolWeight": 100, "AffinityTag": "Physical" },
    ...
  ], ...
}
"""
import openpyxl
import json
import os

WORKBOOK_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "AffixDatabase.xlsm")
OUT_DIR       = os.path.dirname(os.path.abspath(__file__))

POOL_SHEETS = [
    "Weapon_Ranged", "Weapon_Melee", "Weapon_Utility",
    "Headwear_Might", "Headwear_Agility",
    "BodyArmour_Might", "BodyArmour_Agility",
    "Gloves_Might", "Gloves_Agility",
    "Boots_Might", "Boots_Agility",
    "Belt", "Ring", "Amulet",
]

# MasterList column indices (0-based)
COL_ROW_NAME        = 0
COL_AFFIX_ID        = 1
COL_ITEM_NAME       = 2
COL_TYPE            = 3
COL_GROUP_TAG       = 4
COL_DISPLAY_TAG     = 5
COL_TIER            = 6
COL_ILVL            = 7
COL_WEIGHT          = 8
COL_STAT1_ATTR      = 9
COL_STAT1_MIN       = 10
COL_STAT1_MAX       = 11
COL_STAT1_STEP      = 12
COL_STAT1_DEC       = 13
COL_STAT1_DISPLAY   = 14
COL_STAT2_ATTR      = 15
COL_STAT2_MIN       = 16
COL_STAT2_MAX       = 17
COL_STAT2_STEP      = 18
COL_STAT2_DEC       = 19
COL_STAT2_DISPLAY   = 20
COL_DISPLAY_FORMAT  = 21


def to_float(val):
    if val is None or val == "":
        return None
    try:
        return float(val)
    except (TypeError, ValueError):
        return None


def to_int(val):
    if val is None or val == "":
        return None
    try:
        return int(val)
    except (TypeError, ValueError):
        return None


def to_str(val):
    if val is None:
        return ""
    return str(val).strip()


def export_affixes(ws):
    """Returns dict keyed by AffixID with metadata + tiers list."""
    affixes = {}

    for row in ws.iter_rows(min_row=2, values_only=True):
        if not row[COL_AFFIX_ID]:
            continue

        affix_id = to_str(row[COL_AFFIX_ID])

        # First time seeing this AffixID — record metadata from this row
        if affix_id not in affixes:
            affixes[affix_id] = {
                "AffixID":       affix_id,
                "Type":          to_str(row[COL_TYPE]),
                "GroupTag":      to_str(row[COL_GROUP_TAG]),
                "DisplayTag":    to_str(row[COL_DISPLAY_TAG]),
                "Stat1_Attr":    to_str(row[COL_STAT1_ATTR]),
                "Stat2_Attr":    to_str(row[COL_STAT2_ATTR]),
                "DisplayFormat": to_str(row[COL_DISPLAY_FORMAT]),
                "Tiers":         [],
            }

        has_stat2 = bool(row[COL_STAT2_ATTR] and str(row[COL_STAT2_ATTR]).strip())

        stat1_display = to_str(row[COL_STAT1_DISPLAY])
        stat2_display = to_str(row[COL_STAT2_DISPLAY]) if has_stat2 else None
        s1_scale = 0.01 if stat1_display == "Percent" else 1.0
        s2_scale = 0.01 if stat2_display == "Percent" else 1.0

        def scaled(val, scale):
            v = to_float(val)
            return round(v * scale, 6) if v is not None else None

        tier_data = {
            "Tier":               to_int(row[COL_TIER]),
            "ItemName":           to_str(row[COL_ITEM_NAME]),
            "RequiredItemLevel":  to_int(row[COL_ILVL]),
            "Weight":             to_int(row[COL_WEIGHT]),
            "Stat1_Min":          scaled(row[COL_STAT1_MIN],  s1_scale),
            "Stat1_Max":          scaled(row[COL_STAT1_MAX],  s1_scale),
            "Stat1_Step":         scaled(row[COL_STAT1_STEP], s1_scale),
            "Stat1_DecimalCount": to_int(row[COL_STAT1_DEC]),
            "Stat1_DisplayAs":    stat1_display,
            "Stat2_Min":          scaled(row[COL_STAT2_MIN],  s2_scale) if has_stat2 else None,
            "Stat2_Max":          scaled(row[COL_STAT2_MAX],  s2_scale) if has_stat2 else None,
            "Stat2_Step":         scaled(row[COL_STAT2_STEP], s2_scale) if has_stat2 else None,
            "Stat2_DecimalCount": to_int(row[COL_STAT2_DEC])            if has_stat2 else None,
            "Stat2_DisplayAs":    stat2_display                          if has_stat2 else None,
        }

        affixes[affix_id]["Tiers"].append(tier_data)

    # Sort each affix's tiers descending by Tier number (T8/weakest first -> T1/best last)
    for entry in affixes.values():
        entry["Tiers"].sort(key=lambda t: t["Tier"], reverse=True)

    return affixes


def export_pools(wb):
    """Returns dict keyed by pool sheet name."""
    pools = {}
    for sheet_name in POOL_SHEETS:
        if sheet_name not in wb.sheetnames:
            print(f"  WARNING: pool sheet '{sheet_name}' not found — skipped")
            continue

        ws = wb[sheet_name]
        rows = []
        for row in ws.iter_rows(min_row=2, values_only=True):
            if not row[0] and not row[1]:
                continue
            rows.append({
                "GroupTag":    to_str(row[0]),
                "AffixID":     to_str(row[1]),
                "PoolWeight":  to_int(row[2]) or 100,
                "AffinityTag": to_str(row[3]),
            })
        pools[sheet_name] = rows

    return pools


def main():
    if not os.path.exists(WORKBOOK_PATH):
        print(f"ERROR: Could not find workbook at {WORKBOOK_PATH}")
        return

    wb = openpyxl.load_workbook(WORKBOOK_PATH, keep_vba=True, read_only=True)

    # ── Export affixes ─────────────────────────────────────────────────────
    print("Exporting MasterList -> affixes.json ...")
    ws_master = wb["MasterList"]
    affixes = export_affixes(ws_master)

    affix_path = os.path.join(OUT_DIR, "affixes.json")
    with open(affix_path, "w", encoding="utf-8") as f:
        json.dump(affixes, f, indent=2, ensure_ascii=False)

    tier_count = sum(len(a["Tiers"]) for a in affixes.values())
    print(f"  {len(affixes)} affixes, {tier_count} total tier rows -> {affix_path}")

    # ── Export pools ───────────────────────────────────────────────────────
    print("Exporting pool sheets -> pools.json ...")
    pools = export_pools(wb)

    pool_path = os.path.join(OUT_DIR, "pools.json")
    with open(pool_path, "w", encoding="utf-8") as f:
        json.dump(pools, f, indent=2, ensure_ascii=False)

    pool_row_count = sum(len(v) for v in pools.values())
    print(f"  {len(pools)} pools, {pool_row_count} total pool entries -> {pool_path}")

    wb.close()
    print("\nDone.")


if __name__ == "__main__":
    main()
