"""Project header bar, project settings dialog and release dialogs."""
from __future__ import annotations

from pathlib import Path

from PySide6.QtCore import Qt, Signal
from PySide6.QtGui import QRegularExpressionValidator
from PySide6.QtCore import QRegularExpression
from PySide6.QtWidgets import (QCheckBox, QComboBox, QCompleter, QDialog, QDialogButtonBox,
                               QFileDialog, QFormLayout, QFrame, QHBoxLayout, QLabel, QLineEdit,
                               QPlainTextEdit, QPushButton, QSizePolicy, QTreeWidget,
                               QTreeWidgetItem, QVBoxLayout, QWidget)

from . import config
from .catalog import TagCatalog
from .project import PROJECT_EXT, TREE_TYPES, Project, ProjectMeta, bump


def _badge(text: str, bg: str, fg: str = "#0b0c0f") -> str:
    return (f"<span style='background:{bg}; color:{fg}; font-weight:600;'>"
            f"&nbsp;{text}&nbsp;</span>")


class ProjectHeader(QFrame):
    settings_requested = Signal()
    release_requested = Signal()

    def __init__(self) -> None:
        super().__init__()
        self.setObjectName("ProjectHeader")
        self.setStyleSheet("#ProjectHeader { background:#15171b; border-bottom:1px solid #2c3039; }")
        self.title = QLabel()
        self.title.setStyleSheet("font-size:18px; font-weight:700; color:#f3f4f6;")
        self.badges = QLabel()
        self.badges.setTextFormat(Qt.TextFormat.RichText)
        self.sub = QLabel()
        self.sub.setStyleSheet("color:#9ca3af;")
        self.sub.setTextInteractionFlags(Qt.TextInteractionFlag.TextSelectableByMouse)
        # Long paths must clip, not force the window wider.
        self.sub.setSizePolicy(QSizePolicy.Policy.Ignored, QSizePolicy.Policy.Preferred)
        self.badges.setSizePolicy(QSizePolicy.Policy.Ignored, QSizePolicy.Policy.Preferred)
        self.badges.setMinimumWidth(320)

        left = QVBoxLayout()
        left.setSpacing(2)
        top = QHBoxLayout()
        top.addWidget(self.title)
        top.addSpacing(10)
        top.addWidget(self.badges)
        top.addStretch(1)
        left.addLayout(top)
        left.addWidget(self.sub)

        settings = QPushButton("Project Settings…")
        settings.clicked.connect(self.settings_requested)
        release = QPushButton("Mark Live Release…")
        release.clicked.connect(self.release_requested)

        row = QHBoxLayout(self)
        row.setContentsMargins(12, 8, 12, 8)
        row.addLayout(left, 1)
        row.addWidget(settings)
        row.addWidget(release)

    def update_from(self, project: Project, dirty: bool) -> None:
        m = project.meta
        self.title.setText(m.name + (" •" if dirty else ""))
        badges = [_badge(m.tree_type.upper(), "#60a5fa")]
        if m.category:
            badges.append(_badge(m.category, "#a78bfa"))
        badges.append(_badge(f"v{m.version}", "#e5e7eb"))
        if m.live_version:
            badges.append(_badge(f"LIVE v{m.live_version}", "#4ade80"))
            if m.version != m.live_version:
                badges.append(_badge("UNRELEASED CHANGES", "#fbbf24"))
        else:
            badges.append(_badge("NEVER RELEASED", "#4b5563", "#e5e7eb"))
        self.badges.setText(" ".join(badges))
        self.sub.setToolTip(str(project.path) if project.path else "")
        parts = [f"rev {m.revision}"]
        if m.skill_tag:
            parts.append(m.skill_tag)
        parts.append(f"tag root {project.tree.tag_root}")
        if project.path:
            parts.append(str(project.path))
            parts.append(f"→ exports {project.export_path.name}")
        else:
            parts.append("not saved yet")
        self.sub.setText("   ·   ".join(parts))


def suggested_folder(meta: ProjectMeta) -> Path:
    safe = "".join(c for c in meta.name if c.isalnum() or c in "_-") or "NewTree"
    if meta.tree_type == "Passive":
        return config.TREES_DIR / "Passive" / safe
    return config.TREES_DIR / "Skills" / (meta.category or "Other") / safe


class ProjectSettingsDialog(QDialog):
    """Edits ProjectMeta. With new_project=True also chooses where the .sktree is saved."""

    def __init__(self, meta: ProjectMeta, catalog: TagCatalog, parent=None,
                 new_project: bool = False, path: Path | None = None) -> None:
        super().__init__(parent)
        self.meta = meta
        self.new_project = new_project
        self.setWindowTitle("New Tree Project" if new_project else "Project Settings")
        self.resize(620, 640)
        form = QFormLayout()
        form.setFieldGrowthPolicy(QFormLayout.FieldGrowthPolicy.AllNonFixedFieldsGrow)

        self.name = QLineEdit(meta.name)
        form.addRow("Tree name", self.name)

        self.tree_type = QComboBox()
        self.tree_type.addItems(TREE_TYPES)
        self.tree_type.setCurrentText(meta.tree_type)
        form.addRow("Tree type", self.tree_type)

        self.category = QComboBox()
        self.category.setEditable(True)
        self.category.addItems(["", "Physical", "Chemical", "Explosive", "Radiation", "Energy", "Other"])
        self.category.setCurrentText(meta.category)
        self.category.setToolTip("Skill element / grouping. Passive trees can leave this blank.")
        form.addRow("Category", self.category)

        self.skill_tag = QLineEdit(meta.skill_tag)
        self.skill_tag.setPlaceholderText("e.g. Skills.Impact (skill trees)")
        comp = QCompleter(catalog.with_prefix("Skills."), self.skill_tag)
        comp.setCaseSensitivity(Qt.CaseSensitivity.CaseInsensitive)
        comp.setFilterMode(Qt.MatchFlag.MatchContains)
        self.skill_tag.setCompleter(comp)
        form.addRow("Skill tag", self.skill_tag)

        self.description = QPlainTextEdit(meta.description)
        self.description.setFixedHeight(60)
        form.addRow("Description", self.description)

        self.author = QLineEdit(meta.author)
        form.addRow("Author", self.author)

        ver_row = QHBoxLayout()
        self.version = QLineEdit(meta.version)
        self.version.setValidator(QRegularExpressionValidator(QRegularExpression(r"^\d+\.\d+\.\d+$")))
        self.version.setMaximumWidth(110)
        ver_row.addWidget(self.version)
        for part in ("patch", "minor", "major"):
            b = QPushButton(f"+{part}")
            b.clicked.connect(lambda _c=False, p=part: self.version.setText(bump(self.version.text(), p)))
            ver_row.addWidget(b)
        ver_row.addStretch(1)
        form.addRow("Working version", ver_row)

        live = QLabel(f"v{meta.live_version}" if meta.live_version else "never released")
        live.setToolTip("Set by Project ▸ Mark Live Release.")
        form.addRow("Live version", live)

        self.tag_root = QLineEdit(meta.tag_root)
        self.tag_root.setPlaceholderText("derived from existing tags")
        self.tag_root.setToolTip("Prefix for nodes added on empty canvas, e.g. SkillTree.Impact")
        form.addRow("Tag root", self.tag_root)

        self.ue_folder = QLineEdit(meta.ue_asset_folder)
        self.ue_folder.setPlaceholderText("/Game/Blueprints/UI/SkillTrees/.../Nodes/Impact")
        self.ue_folder.setToolTip("Where node data assets live in UE (FP_SkillTreeImporter NodeAssetOutputPath).")
        form.addRow("UE asset folder", self.ue_folder)

        self.export_file = QLineEdit(meta.export_file)
        self.export_file.setPlaceholderText("<project name>.json (next to the project)")
        form.addRow("UE export file", self.export_file)

        self.notes = QPlainTextEdit(meta.notes)
        self.notes.setPlaceholderText("Design notes, TODOs, balance thoughts…")
        form.addRow("Notes", self.notes)

        if not new_project:
            info = QLabel(f"Revision {meta.revision}   ·   created {meta.created[:10]}"
                          f"   ·   modified {meta.modified[:16].replace('T', ' ') or '—'}")
            info.setStyleSheet("color:#9ca3af;")
            form.addRow("", info)

        self.location = None
        if new_project:
            loc_row = QHBoxLayout()
            self.location = QLineEdit(str(path.parent) if path else "")
            self._location_touched = bool(path)
            self.location.textEdited.connect(lambda _t: setattr(self, "_location_touched", True))
            browse = QPushButton("Browse…")
            browse.clicked.connect(self._browse)
            loc_row.addWidget(self.location, 1)
            loc_row.addWidget(browse)
            form.addRow("Save in folder", loc_row)
            for sig in (self.name.textEdited, self.tree_type.currentTextChanged, self.category.currentTextChanged):
                sig.connect(self._suggest_location)
            if not path:
                self._suggest_location()

        buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        layout = QVBoxLayout(self)
        layout.addLayout(form)
        layout.addWidget(buttons)

    def _collect(self, meta: ProjectMeta) -> None:
        meta.name = self.name.text().strip() or "New Tree"
        meta.tree_type = self.tree_type.currentText()
        meta.category = self.category.currentText().strip()
        meta.skill_tag = self.skill_tag.text().strip()
        meta.description = self.description.toPlainText().strip()
        meta.author = self.author.text().strip()
        if self.version.hasAcceptableInput():
            meta.version = self.version.text()
        meta.tag_root = self.tag_root.text().strip()
        meta.ue_asset_folder = self.ue_folder.text().strip()
        meta.export_file = self.export_file.text().strip()
        meta.notes = self.notes.toPlainText()

    def _suggest_location(self, *_a) -> None:
        if self.location is not None and not self._location_touched:
            probe = ProjectMeta()
            self._collect(probe)
            self.location.setText(str(suggested_folder(probe)))

    def _browse(self) -> None:
        d = QFileDialog.getExistingDirectory(self, "Project folder", self.location.text() or str(config.TREES_DIR))
        if d:
            self.location.setText(d)
            self._location_touched = True

    def accept(self) -> None:
        self._collect(self.meta)
        super().accept()

    def project_path(self) -> Path | None:
        if self.location is None:
            return None
        safe = "".join(c for c in self.meta.name if c.isalnum() or c in "_-") or "NewTree"
        return Path(self.location.text()) / (safe + PROJECT_EXT)


class ReleaseDialog(QDialog):
    def __init__(self, project: Project, parent=None) -> None:
        super().__init__(parent)
        m = project.meta
        self.setWindowTitle("Mark Live Release")
        self.resize(560, 420)
        layout = QVBoxLayout(self)
        prev = f"v{m.live_version}" if m.live_version else "nothing (first release)"
        msg = QLabel(f"<b>{m.name}</b> v{m.version} will be marked as the version players have.<br>"
                     f"Currently live: {prev}.<br><br>"
                     f"A frozen copy is saved to <code>releases/{project.path.stem}_v{m.version}.json</code>. "
                     "From then on, validation flags edits that would break players' allocated points "
                     "(removed/renamed nodes, removed links).")
        msg.setWordWrap(True)
        layout.addWidget(msg)
        layout.addWidget(QLabel("Release notes:"))
        self.notes = QPlainTextEdit()
        layout.addWidget(self.notes, 1)
        self.bump = QCheckBox(f"Afterwards, move the working version to v{bump(m.version, 'patch')}")
        self.bump.setChecked(True)
        layout.addWidget(self.bump)
        buttons = QDialogButtonBox()
        buttons.addButton("Mark Live", QDialogButtonBox.ButtonRole.AcceptRole)
        buttons.addButton(QDialogButtonBox.StandardButton.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        layout.addWidget(buttons)


class ReleaseHistoryDialog(QDialog):
    def __init__(self, project: Project, parent=None) -> None:
        super().__init__(parent)
        self.setWindowTitle(f"Release History — {project.meta.name}")
        self.resize(760, 380)
        tree = QTreeWidget()
        tree.setHeaderLabels(["Version", "Date", "Nodes", "File", "Notes"])
        tree.setRootIsDecorated(False)
        for r in reversed(project.meta.releases):
            item = QTreeWidgetItem([f"v{r.version}", r.date[:16].replace("T", " "), str(r.node_count),
                                    r.file, r.notes.replace("\n", "  ")])
            item.setToolTip(4, r.notes)
            tree.addTopLevelItem(item)
        for col, w in enumerate((80, 140, 60, 240)):
            tree.setColumnWidth(col, w)
        layout = QVBoxLayout(self)
        if not project.meta.releases:
            layout.addWidget(QLabel("No releases yet. Use Project ▸ Mark Live Release when this tree ships."))
        layout.addWidget(tree)
        close = QDialogButtonBox(QDialogButtonBox.StandardButton.Close)
        close.rejected.connect(self.reject)
        layout.addWidget(close)
