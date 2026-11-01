"""One-off migration from the old NodeData .xlsx workbooks (Nodes / Effects / Connections)."""
from __future__ import annotations

from pathlib import Path

from .model import Effect, SkillTree, normalize_ue_reference


def _text(value) -> str:
    if value is None:
        return ""
    value = str(value)
    return value[1:] if value.startswith("'") else value  # Excel text-escape apostrophe


def import_node_data_xlsx(path: str | Path) -> tuple[SkillTree, list[str]]:
    from openpyxl import load_workbook

    wb = load_workbook(path, data_only=True)
    warnings: list[str] = []
    tree = SkillTree()
    by_tag: dict[str, int] = {}

    ws = wb["Nodes"]
    rows = ws.iter_rows(values_only=True)
    col = {name: i for i, name in enumerate(next(rows))}
    for row in rows:
        if not row or not row[col["NodeTag"]]:
            continue
        tag = str(row[col["NodeTag"]]).strip()
        x = row[col["X"]] if "X" in col else None
        y = row[col["Y"]] if "Y" in col else None
        if not isinstance(x, (int, float)) or not isinstance(y, (int, float)):
            warnings.append(f"{tag}: no X/Y — placed at canvas centre")
            x, y = 1500.0, 1500.0
        start = row[col["IsStartNode"]]
        node = tree.add_node(
            tag=tag,
            node_type=row[col["NodeType"]] or "Minor",
            is_start=start.strip().lower() == "true" if isinstance(start, str) else bool(start),
            name=_text(row[col["NodeName"]]),
            description=_text(row[col["NodeDescription"]]).replace("\\n", "\n"),
            icon=normalize_ue_reference(_text(row[col["NodeIcon"]])),
            x=float(x), y=float(y),
        )
        if tag in by_tag:
            warnings.append(f"Duplicate NodeTag {tag}")
        by_tag.setdefault(tag, node.uid)

    if "Effects" in wb.sheetnames:
        rows = wb["Effects"].iter_rows(values_only=True)
        ecol = {name: i for i, name in enumerate(next(rows))}
        eff_rows = [r for r in rows if r and r[ecol["NodeTag"]]]
        eff_rows.sort(key=lambda r: (str(r[ecol["NodeTag"]]), r[ecol["EffectOrder"]] or 0))
        for row in eff_rows:
            tag = str(row[ecol["NodeTag"]]).strip()
            if tag not in by_tag:
                warnings.append(f"Effect row for unknown node {tag} skipped")
                continue
            data = {"EffectType": row[ecol["EffectType"]] or "Stat Mod"}
            for key in ("EffectStatModAttributeType", "EffectStatModifierOp", "EffectFlagType",
                        "GrantSkillTag", "EffectSkillPassiveTag", "EffectStatMagnitude"):
                if key in ecol and row[ecol[key]] not in (None, ""):
                    v = row[ecol[key]]
                    data[key] = v if isinstance(v, (int, float)) else _text(v)
            tree.nodes[by_tag[tag]].effects.append(Effect.from_json(data))

    if "Connections" in wb.sheetnames:
        rows = wb["Connections"].iter_rows(values_only=True)
        ccol = {name: i for i, name in enumerate(next(rows))}
        for row in rows:
            if not row:
                continue
            a, b = row[ccol["NodeTagA"]], row[ccol["NodeTagB"]]
            if not a or not b:
                continue
            a, b = str(a).strip(), str(b).strip()
            if a in by_tag and b in by_tag:
                tree.connect(by_tag[a], by_tag[b])
            else:
                warnings.append(f"Connection {a} <-> {b} references a missing node")

    return tree, warnings
