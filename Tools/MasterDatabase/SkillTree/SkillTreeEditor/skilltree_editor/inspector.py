"""Right-hand property panel. Edits one node fully, or common fields across many."""
from __future__ import annotations

from PySide6.QtCore import QSize, Qt, Signal
from PySide6.QtGui import QDoubleValidator
from PySide6.QtWidgets import (QCheckBox, QComboBox, QCompleter, QDoubleSpinBox, QFormLayout,
                               QFrame, QGroupBox, QHBoxLayout, QLabel, QLineEdit,
                               QPlainTextEdit, QPushButton, QScrollArea, QSizePolicy,
                               QToolButton, QVBoxLayout, QWidget)

from .catalog import TagCatalog
from .icon_picker import IconPickerDialog
from .icons import IconLibrary
from .model import (EFFECT_FIELDS, EFFECT_TYPES, FIELD_LABELS, MODIFIER_OPS, NODE_TYPES, Effect,
                    Node, normalize_ue_reference)

MIXED = "— multiple values —"


class _TextEdit(QPlainTextEdit):
    focus_lost = Signal()

    def focusOutEvent(self, event) -> None:
        super().focusOutEvent(event)
        self.focus_lost.emit()


def _tag_completer(options: list[str], parent) -> QCompleter:
    c = QCompleter(options, parent)
    c.setCaseSensitivity(Qt.CaseSensitivity.CaseInsensitive)
    c.setFilterMode(Qt.MatchFlag.MatchContains)
    c.setMaxVisibleItems(20)
    return c


class EffectEditor(QGroupBox):
    changed = Signal(str)
    remove_requested = Signal(object)
    move_requested = Signal(object, int)

    def __init__(self, effect: Effect, index: int, catalog: TagCatalog) -> None:
        super().__init__(f"Effect {index + 1}")
        self.effect = effect
        self.catalog = catalog

        self.type_combo = QComboBox()
        self.type_combo.addItems(EFFECT_TYPES)
        if effect.effect_type not in EFFECT_TYPES:
            self.type_combo.addItem(effect.effect_type)
        self.type_combo.setCurrentText(effect.effect_type)
        self.type_combo.activated.connect(self._on_type)

        header = QHBoxLayout()
        header.addWidget(self.type_combo, 1)
        for text, tip, slot in (("▲", "Move up", lambda: self.move_requested.emit(self, -1)),
                                ("▼", "Move down", lambda: self.move_requested.emit(self, 1)),
                                ("✕", "Remove effect", lambda: self.remove_requested.emit(self))):
            b = QToolButton(text=text, toolTip=tip)
            b.clicked.connect(slot)
            header.addWidget(b)

        self.form = QFormLayout()
        self.form.setFieldGrowthPolicy(QFormLayout.FieldGrowthPolicy.AllNonFixedFieldsGrow)
        layout = QVBoxLayout(self)
        layout.addLayout(header)
        layout.addLayout(self.form)
        self._build_fields()

    def _build_fields(self) -> None:
        while self.form.rowCount():
            self.form.removeRow(0)
        for key in EFFECT_FIELDS.get(self.effect.effect_type, ()):
            value = self.effect.values.get(key, "")
            if key == "EffectStatModifierOp":
                w = QComboBox()
                w.addItems(MODIFIER_OPS)
                w.setCurrentText(value or "Add")
                if not value:
                    self.effect.values[key] = "Add"
                w.activated.connect(lambda _i, k=key, w=w: self._set(k, w.currentText()))
                w.setToolTip("Add = flat bonus.  Multiply = raw multiplier: 1.10 = 10% increased, 0.90 = 10% reduced (several on one stat add up as 1 + sum(m - 1)).")
                self.form.addRow(FIELD_LABELS[key], w)
            elif key == "EffectStatMagnitude":
                w = QLineEdit("" if value in (None, "") else f"{value:g}" if isinstance(value, (int, float)) else str(value))
                w.setValidator(QDoubleValidator())
                w.editingFinished.connect(lambda k=key, w=w: self._set_number(k, w))
                self.form.addRow(FIELD_LABELS[key], w)
            else:
                w = QLineEdit(str(value))
                w.setCompleter(_tag_completer(self.catalog.options_for(key), w))
                hint = QLabel()
                hint.setWordWrap(True)
                hint.setStyleSheet("color:#9ca3af; font-size:11px;")
                w.textChanged.connect(lambda text, h=hint, w=w: self._update_hint(text, h, w))
                self._update_hint(str(value), hint, w)
                w.editingFinished.connect(lambda k=key, w=w: self._set(k, w.text().strip()))
                self.form.addRow(FIELD_LABELS[key], w)
                self.form.addRow("", hint)

    def _update_hint(self, text: str, hint: QLabel, edit: QLineEdit) -> None:
        text = text.strip()
        if not text:
            hint.setText("")
            edit.setStyleSheet("")
        elif self.catalog.known(text):
            hint.setText(self.catalog.describe(text))
            edit.setStyleSheet("")
        else:
            hint.setText("Not registered in FP_GameplayTags.cpp")
            edit.setStyleSheet("color:#fbbf24;")

    def _set(self, key: str, value) -> None:
        if self.effect.values.get(key) != value:
            self.effect.values[key] = value
            self.changed.emit("Edit effect")

    def _set_number(self, key: str, edit: QLineEdit) -> None:
        text = edit.text().strip()
        try:
            value = float(text) if text else ""
        except ValueError:
            return
        self._set(key, value)

    def _on_type(self, _index: int) -> None:
        new_type = self.type_combo.currentText()
        if new_type != self.effect.effect_type:
            self.effect.effect_type = new_type
            self._build_fields()
            self.changed.emit("Change effect type")


class Inspector(QScrollArea):
    edited = Signal(str, list)   # undo label, uids changed

    def __init__(self, catalog: TagCatalog, icons: IconLibrary) -> None:
        super().__init__()
        self.catalog = catalog
        self.icons = icons
        self.nodes: list[Node] = []
        self.setWidgetResizable(True)
        self.setMinimumWidth(340)

        body = QWidget()
        self.setWidget(body)
        root = QVBoxLayout(body)

        self.header = QLabel("Nothing selected")
        self.header.setWordWrap(True)
        self.header.setStyleSheet("font-weight:600; font-size:14px;")
        root.addWidget(self.header)

        self.form_box = QWidget()
        form = QFormLayout(self.form_box)
        form.setFieldGrowthPolicy(QFormLayout.FieldGrowthPolicy.AllNonFixedFieldsGrow)
        form.setContentsMargins(0, 0, 0, 0)

        self.tag_edit = QLineEdit()
        self.tag_edit.editingFinished.connect(self._commit_tag)
        form.addRow("Node Tag", self.tag_edit)

        self.type_combo = QComboBox()
        self.type_combo.addItems(NODE_TYPES)
        self.type_combo.activated.connect(self._commit_type)
        form.addRow("Type", self.type_combo)

        self.start_check = QCheckBox("Start node (allocatable with no neighbour)")
        self.start_check.clicked.connect(self._commit_start)
        form.addRow("", self.start_check)

        self.name_edit = QLineEdit()
        self.name_edit.editingFinished.connect(self._commit_name)
        form.addRow("Name", self.name_edit)

        self.desc_edit = _TextEdit()
        self.desc_edit.setFixedHeight(80)
        self.desc_edit.focus_lost.connect(self._commit_desc)
        form.addRow("Description", self.desc_edit)

        icon_row = QHBoxLayout()
        self.icon_preview = QLabel()
        self.icon_preview.setFixedSize(QSize(56, 56))
        self.icon_preview.setFrameShape(QFrame.Shape.StyledPanel)
        self.icon_preview.setAlignment(Qt.AlignmentFlag.AlignCenter)
        icon_col = QVBoxLayout()
        self.icon_edit = QLineEdit()
        self.icon_edit.editingFinished.connect(self._commit_icon_text)
        browse = QPushButton("Browse…")
        browse.clicked.connect(self._browse_icon)
        icon_col.addWidget(self.icon_edit)
        icon_col.addWidget(browse, 0, Qt.AlignmentFlag.AlignLeft)
        icon_row.addWidget(self.icon_preview)
        icon_row.addLayout(icon_col, 1)
        form.addRow("Icon", icon_row)

        pos_row = QHBoxLayout()
        self.x_spin, self.y_spin = QDoubleSpinBox(), QDoubleSpinBox()
        for spin in (self.x_spin, self.y_spin):
            spin.setRange(-10000, 10000)
            spin.setDecimals(2)
            spin.setKeyboardTracking(False)
            spin.editingFinished.connect(self._commit_pos)
            pos_row.addWidget(spin)
        form.addRow("Position X / Y", pos_row)
        root.addWidget(self.form_box)

        self.effects_box = QGroupBox("Effects")
        eff_layout = QVBoxLayout(self.effects_box)
        self.effects_layout = QVBoxLayout()
        eff_layout.addLayout(self.effects_layout)
        add_btn = QPushButton("+ Add Effect")
        add_btn.clicked.connect(self._add_effect)
        eff_layout.addWidget(add_btn)
        root.addWidget(self.effects_box)

        self.multi_note = QLabel("Effects are edited per node. To copy effects across the selection, "
                                 "select a source node, Edit ▸ Copy Node Data, then select targets and "
                                 "Edit ▸ Paste Node Data.")
        self.multi_note.setWordWrap(True)
        self.multi_note.setStyleSheet("color:#9ca3af;")
        root.addWidget(self.multi_note)
        root.addStretch(1)

        self.set_nodes([])

    # ---- population ----------------------------------------------------------

    def flush(self) -> None:
        """Commit any field that is mid-edit (called before the selection changes)."""
        if self.tag_edit.isModified():
            self._commit_tag()
        if self.name_edit.isModified():
            self._commit_name()
        if self.icon_edit.isModified():
            self._commit_icon_text()
        self._commit_desc()

    def set_nodes(self, nodes: list[Node]) -> None:
        self.nodes = nodes
        single = len(nodes) == 1
        any_sel = bool(nodes)
        self.form_box.setVisible(any_sel)
        self.effects_box.setVisible(single)
        self.multi_note.setVisible(len(nodes) > 1)
        if not any_sel:
            self.header.setText("Nothing selected\n\nDouble-click the canvas to add a node.")
            return
        self.header.setText(nodes[0].name or nodes[0].tag if single else f"{len(nodes)} nodes selected")
        self.refresh_fields()

    def _common(self, attr: str):
        values = {getattr(n, attr) for n in self.nodes}
        return values.pop() if len(values) == 1 else None

    def refresh_fields(self) -> None:
        if not self.nodes:
            return
        single = len(self.nodes) == 1
        n0 = self.nodes[0]

        self.tag_edit.setEnabled(single)
        self.tag_edit.setText(n0.tag if single else "")
        self.tag_edit.setPlaceholderText("" if single else "(unique per node)")
        self.tag_edit.setModified(False)

        t = self._common("node_type")
        self.type_combo.setCurrentIndex(NODE_TYPES.index(t) if t in NODE_TYPES else -1)

        s = self._common("is_start")
        self.start_check.setTristate(s is None)
        self.start_check.setCheckState(Qt.CheckState.PartiallyChecked if s is None
                                       else Qt.CheckState.Checked if s else Qt.CheckState.Unchecked)

        for edit, attr in ((self.name_edit, "name"), (self.icon_edit, "icon")):
            v = self._common(attr)
            edit.setText(v or "")
            edit.setPlaceholderText(MIXED if v is None else "")
            edit.setModified(False)

        d = self._common("description")
        self.desc_edit.setPlainText(d or "")
        self.desc_edit.setPlaceholderText(MIXED if d is None else "")
        self.desc_edit.document().setModified(False)

        self._refresh_icon_preview()

        for spin in (self.x_spin, self.y_spin):
            spin.setEnabled(single)
            spin.blockSignals(True)
        self.x_spin.setValue(n0.x)
        self.y_spin.setValue(n0.y)
        for spin in (self.x_spin, self.y_spin):
            spin.blockSignals(False)

        if single:
            self._rebuild_effects()

    def _refresh_icon_preview(self) -> None:
        icon = self._common("icon")
        pm = self.icons.pixmap(icon, 56) if icon else None
        if pm:
            self.icon_preview.setPixmap(pm)
        else:
            self.icon_preview.clear()
            self.icon_preview.setText("?" if icon else "")

    def _rebuild_effects(self) -> None:
        while self.effects_layout.count():
            w = self.effects_layout.takeAt(0).widget()
            if w:
                w.deleteLater()
        node = self.nodes[0]
        for i, eff in enumerate(node.effects):
            ed = EffectEditor(eff, i, self.catalog)
            ed.changed.connect(lambda label: self._emit(label))
            ed.remove_requested.connect(self._remove_effect)
            ed.move_requested.connect(self._move_effect)
            self.effects_layout.addWidget(ed)

    # ---- commits -------------------------------------------------------------

    def _emit(self, label: str) -> None:
        self.edited.emit(label, [n.uid for n in self.nodes])

    def _apply(self, attr: str, value, label: str) -> None:
        changed = False
        for n in self.nodes:
            if getattr(n, attr) != value:
                setattr(n, attr, value)
                changed = True
        if changed:
            self._emit(label)

    def _commit_tag(self) -> None:
        if len(self.nodes) == 1 and self.tag_edit.isModified():
            self.tag_edit.setModified(False)
            self._apply("tag", self.tag_edit.text().strip(), "Rename tag")
            self.header.setText(self.nodes[0].name or self.nodes[0].tag)

    def _commit_type(self, _i: int) -> None:
        self._apply("node_type", self.type_combo.currentText(), "Change type")

    def _commit_start(self, _checked: bool) -> None:
        self.start_check.setTristate(False)
        self._apply("is_start", self.start_check.isChecked(), "Toggle start node")

    def _commit_name(self) -> None:
        if self.nodes and self.name_edit.isModified():
            self.name_edit.setModified(False)
            self._apply("name", self.name_edit.text(), "Rename node")

    def _commit_desc(self) -> None:
        if self.nodes and self.desc_edit.document().isModified():
            self.desc_edit.document().setModified(False)
            self._apply("description", self.desc_edit.toPlainText(), "Edit description")

    def _commit_icon_text(self) -> None:
        if self.nodes and self.icon_edit.isModified():
            self.icon_edit.setModified(False)
            value = normalize_ue_reference(self.icon_edit.text())
            self.icon_edit.setText(value)
            self._apply("icon", value, "Change icon")
            self._refresh_icon_preview()

    def _browse_icon(self) -> None:
        if not self.nodes:
            return
        dlg = IconPickerDialog(self.icons, self._common("icon") or "", self)
        if dlg.exec() and dlg.selected_path:
            self.icon_edit.setText(dlg.selected_path)
            self._apply("icon", dlg.selected_path, "Change icon")
            self._refresh_icon_preview()

    def _commit_pos(self) -> None:
        if len(self.nodes) == 1:
            n = self.nodes[0]
            x, y = self.x_spin.value(), self.y_spin.value()
            if (round(n.x, 2), round(n.y, 2)) != (x, y):
                n.x, n.y = x, y
                self._emit("Move")

    def _add_effect(self) -> None:
        node = self.nodes[0]
        template = node.effects[-1].effect_type if node.effects else "Stat Mod"
        node.effects.append(Effect(template, {}))
        self._rebuild_effects()
        self._emit("Add effect")

    def _remove_effect(self, editor: EffectEditor) -> None:
        self.nodes[0].effects.remove(editor.effect)
        self._rebuild_effects()
        self._emit("Remove effect")

    def _move_effect(self, editor: EffectEditor, delta: int) -> None:
        effects = self.nodes[0].effects
        i = effects.index(editor.effect)
        j = i + delta
        if 0 <= j < len(effects):
            effects[i], effects[j] = effects[j], effects[i]
            self._rebuild_effects()
            self._emit("Reorder effects")
