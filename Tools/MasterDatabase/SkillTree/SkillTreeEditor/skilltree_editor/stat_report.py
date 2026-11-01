"""Stat Report: query a stat / passive / flag / skill tag and report every node that grants it,
with per-tag totals across the tree, and highlight those nodes on the canvas."""
from __future__ import annotations

from collections import defaultdict
from dataclasses import dataclass, field

from PySide6.QtCore import Qt, Signal
from PySide6.QtGui import QColor, QFont
from PySide6.QtWidgets import (QCheckBox, QCompleter, QHBoxLayout, QLabel, QLineEdit, QPushButton,
                               QTreeWidget, QTreeWidgetItem, QVBoxLayout, QWidget)

from .model import SkillTree

# Effect type -> (tag field, has a magnitude)
_EFFECT_TAG_FIELD = {
    "Stat Mod": ("EffectStatModAttributeType", True),
    "Skill Passive": ("EffectSkillPassiveTag", True),
    "Effect Flag": ("EffectFlagType", False),
    "Grant Skill": ("GrantSkillTag", False),
}


def _fmt(v: float) -> str:
    return f"{v:.4f}".rstrip("0").rstrip(".") if v != int(v) else str(int(v))


@dataclass
class Hit:
    uid: int
    node_tag: str
    node_name: str
    node_type: str
    effect_type: str
    tag: str
    op: str                  # "Add" / "Multiply" / "" (flags, grants)
    value: float | None


@dataclass
class Group:
    tag: str
    effect_type: str
    op: str
    hits: list[Hit] = field(default_factory=list)

    @property
    def node_uids(self) -> set[int]:
        return {h.uid for h in self.hits}

    def total(self) -> float | None:
        vals = [h.value for h in self.hits if h.value is not None]
        if not vals:
            return None
        if self.op == "Multiply":
            # GAS Multiplicitive mods on one attribute combine as 1 + sum(m - 1).
            return 1.0 + sum(v - 1.0 for v in vals)
        return sum(vals)

    def value_breakdown(self) -> str:
        counts: dict[float, int] = defaultdict(int)
        for h in self.hits:
            if h.value is not None:
                counts[h.value] += 1
        return ", ".join(f"{_fmt(v)} ×{c}" if c > 1 else _fmt(v)
                         for v, c in sorted(counts.items(), key=lambda kv: (-kv[1], kv[0])))


def matches(tag: str, query: str, include_sub: bool) -> bool:
    t, q = tag.lower(), query.lower()
    if t == q:
        return True
    if include_sub and t.startswith(q + "."):
        return True
    return False


def build_report(tree: SkillTree, query: str, include_sub: bool = True) -> list[Group]:
    query = query.strip().rstrip(".")
    if not query:
        return []
    groups: dict[tuple[str, str, str], Group] = {}
    for node in tree.nodes.values():
        for eff in node.effects:
            spec = _EFFECT_TAG_FIELD.get(eff.effect_type)
            if spec is None:
                continue
            key_field, has_value = spec
            tag = str(eff.values.get(key_field) or "").strip()
            if not tag or not matches(tag, query, include_sub):
                continue
            op = str(eff.values.get("EffectStatModifierOp") or "Add") if eff.effect_type == "Stat Mod" else ""
            value = None
            if has_value:
                try:
                    value = float(eff.values.get("EffectStatMagnitude"))
                except (TypeError, ValueError):
                    value = None
            g = groups.setdefault((tag, eff.effect_type, op), Group(tag, eff.effect_type, op))
            g.hits.append(Hit(node.uid, node.tag, node.name, node.node_type, eff.effect_type, tag, op, value))
    out = sorted(groups.values(), key=lambda g: (g.tag.lower(), g.effect_type, g.op))
    for g in out:
        g.hits.sort(key=lambda h: h.node_tag)
    return out


class StatReportPanel(QWidget):
    """Dock panel. Emits highlight_changed(set[uid]) and select_requested(list[uid], frame)."""
    highlight_changed = Signal(object)
    select_requested = Signal(object, bool)

    def __init__(self, parent=None) -> None:
        super().__init__(parent)
        self._tree: SkillTree | None = None
        self._groups: list[Group] = []
        self._resolve = None   # catalog.resolve_gated: tag -> (base attribute tag | None, gates)

        self.query = QLineEdit(placeholderText="Stat / passive / flag tag, e.g. Damage.Increased or Defence.Armour")
        self.query.setClearButtonEnabled(True)
        self.sub_tags = QCheckBox("Include sub-tags (gated variants)")
        self.sub_tags.setChecked(True)
        self.sub_tags.setToolTip("Damage.Increased also matches Damage.Increased.Rifle, .Generic, …")
        self.highlight = QCheckBox("Highlight on canvas")
        self.highlight.setChecked(True)
        self.select_btn = QPushButton("Select All")
        self.select_btn.setToolTip("Select every matching node on the canvas")

        top = QHBoxLayout()
        top.addWidget(self.query, 1)
        top.addWidget(self.sub_tags)
        top.addWidget(self.highlight)
        top.addWidget(self.select_btn)

        self.summary = QLabel()
        self.summary.setTextInteractionFlags(Qt.TextInteractionFlag.TextSelectableByMouse)
        self.summary.setWordWrap(True)

        self.table = QTreeWidget()
        self.table.setHeaderLabels(["Tag / Node", "Kind", "Op", "Nodes", "Total", "Values"])
        self.table.setColumnWidth(0, 340)
        self.table.setColumnWidth(1, 90)
        self.table.setColumnWidth(2, 70)
        self.table.setColumnWidth(3, 55)
        self.table.setColumnWidth(4, 80)
        self.table.setAlternatingRowColors(True)

        lay = QVBoxLayout(self)
        lay.setContentsMargins(4, 4, 4, 4)
        lay.addLayout(top)
        lay.addWidget(self.summary)
        lay.addWidget(self.table, 1)

        self.query.textChanged.connect(lambda _t: self.refresh())
        self.sub_tags.toggled.connect(lambda _c: self.refresh())
        self.highlight.toggled.connect(lambda _c: self._emit_highlight())
        self.select_btn.clicked.connect(lambda: self.select_requested.emit(sorted(self.matched_uids()), True))
        self.table.itemClicked.connect(self._on_item_clicked)
        self.table.itemActivated.connect(self._on_item_clicked)

    # ---- public ----------------------------------------------------------------

    def set_completions(self, tags: list[str]) -> None:
        c = QCompleter(sorted(set(tags)), self.query)
        c.setCaseSensitivity(Qt.CaseSensitivity.CaseInsensitive)
        c.setFilterMode(Qt.MatchFlag.MatchContains)
        c.setMaxVisibleItems(20)
        self.query.setCompleter(c)

    def set_resolver(self, resolve_gated) -> None:
        self._resolve = resolve_gated

    def base_of(self, tag: str) -> str:
        """Underlying stat a (possibly gated) tag adds to; '.Generic' folded into its parent."""
        base = tag
        if self._resolve is not None:
            resolved, _gates = self._resolve(tag)
            base = resolved or tag
        return base[: -len(".Generic")] if base.endswith(".Generic") else base

    def set_tree(self, tree: SkillTree) -> None:
        self._tree = tree
        self.refresh()

    def matched_uids(self) -> set[int]:
        return set().union(*(g.node_uids for g in self._groups)) if self._groups else set()

    def refresh(self) -> None:
        q = self.query.text()
        self._groups = build_report(self._tree, q, self.sub_tags.isChecked()) if self._tree else []
        self._fill()
        self._emit_highlight()

    # ---- internals ---------------------------------------------------------------

    def _emit_highlight(self) -> None:
        self.highlight_changed.emit(self.matched_uids() if self.highlight.isChecked() else set())

    def _fill(self) -> None:
        self.table.clear()
        q = self.query.text().strip()
        if not q:
            self.summary.setText("<span style='color:#9ca3af'>Type a tag to report on it.</span>")
            return
        if not self._groups:
            self.summary.setText(f"No nodes grant <b>{q}</b>.")
            return

        n_effects = sum(len(g.hits) for g in self._groups)
        n_nodes = len(self.matched_uids())
        by_type: dict[str, int] = defaultdict(int)
        if self._tree:
            for uid in self.matched_uids():
                node = self._tree.nodes.get(uid)
                if node:
                    by_type[node.node_type] += 1
        types = ", ".join(f"{c} {t}" for t, c in sorted(by_type.items()))
        lines = [f"<b>{n_effects}</b> effect(s) on <b>{n_nodes}</b> node(s) ({types})."]
        # Totals per underlying stat: a base tag and its gated variants (Damage.Increased + .Rifle + .AoE.Mace)
        # add to the same attribute, so they sum; different stats (DeferredDamageAmount vs Time) never do.
        per_base: dict[str, list[Group]] = defaultdict(list)
        for g in self._groups:
            if g.effect_type == "Stat Mod" and g.op == "Add":
                per_base[self.base_of(g.tag)].append(g)
        combined = [(base, gs) for base, gs in sorted(per_base.items()) if len(gs) > 1]
        if combined:
            lines.append("Totals per stat, incl. gated variants "
                         "<span style='color:#9ca3af'>(a gated variant only applies to matching skills)</span>:")
            for base, gs in combined:
                total = sum(g.total() or 0.0 for g in gs)
                lines.append(f"&nbsp;&nbsp;{base}: <b style='color:#fbbf24'>{_fmt(total)}</b> "
                             f"from {sum(len(g.hits) for g in gs)} effects over {len(gs)} tags")
        self.summary.setText("<br>".join(lines))

        bold = QFont()
        bold.setBold(True)
        for g in self._groups:
            total = g.total()
            top = QTreeWidgetItem([g.tag, g.effect_type, g.op, str(len(g.node_uids)),
                                   ("×" + _fmt(total) if g.op == "Multiply" else _fmt(total)) if total is not None else "—",
                                   g.value_breakdown()])
            top.setData(0, Qt.ItemDataRole.UserRole, sorted(g.node_uids))
            for col in (0, 4):
                top.setFont(col, bold)
            top.setForeground(4, QColor("#fbbf24"))
            for h in g.hits:
                child = QTreeWidgetItem([f"{h.node_tag}   —   {h.node_name}", h.node_type, "", "",
                                         _fmt(h.value) if h.value is not None else "", ""])
                child.setData(0, Qt.ItemDataRole.UserRole, [h.uid])
                child.setForeground(0, QColor("#d1d5db"))
                top.addChild(child)
            self.table.addTopLevelItem(top)
            top.setExpanded(len(self._groups) <= 3)

    def _on_item_clicked(self, item: QTreeWidgetItem, _col: int = 0) -> None:
        uids = item.data(0, Qt.ItemDataRole.UserRole) or []
        if uids:
            self.select_requested.emit(list(uids), len(uids) > 1)
