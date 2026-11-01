"""Main window: wires canvas, inspector, node list, validation, file + tool actions."""
from __future__ import annotations

import json
import os
import subprocess
import sys
from pathlib import Path

from PySide6.QtCore import QEvent, QPointF, QSettings, Qt, QTimer
from PySide6.QtGui import QAction, QCloseEvent, QColor, QKeySequence
from PySide6.QtWidgets import (QApplication, QDialog, QDialogButtonBox, QDockWidget, QFileDialog,
                               QInputDialog, QLabel, QLineEdit, QListWidget, QListWidgetItem,
                               QMainWindow, QMenu, QMessageBox, QPlainTextEdit, QStackedWidget,
                               QTabBar, QTreeWidget, QTreeWidgetItem, QVBoxLayout, QWidget)

from . import config
from .canvas import NodeItem, TreeScene, TreeView
from .catalog import load_catalog
from .icons import IconLibrary
from .importers import import_node_data_xlsx
from .inspector import Inspector
from .project import PROJECT_EXT, Project, ProjectMeta, bump, other_project_tags
from .project_ui import (ProjectHeader, ProjectSettingsDialog, ReleaseDialog, ReleaseHistoryDialog,
                         suggested_folder)
from .validation import ERROR, INFO, WARNING, validate

PROJECT_FILTER = f"Skill tree project (*{PROJECT_EXT})"
LEGACY_FILTER = "Legacy tree JSON (*.json)"

CLIP_MIME = "application/x-fp-skilltree-fragment"


class History:
    def __init__(self, limit: int = 300) -> None:
        self.limit = limit
        self.states: list[tuple[str, tuple]] = []
        self.index = -1
        self.saved_index = -1

    def reset(self, snap) -> None:
        self.states = [("Open", snap)]
        self.index = 0
        self.saved_index = 0

    def push(self, label: str, snap) -> None:
        del self.states[self.index + 1:]
        if self.saved_index > self.index:
            self.saved_index = -1
        self.states.append((label, snap))
        if len(self.states) > self.limit:
            self.states.pop(0)
            self.saved_index -= 1
        self.index = len(self.states) - 1

    @property
    def dirty(self) -> bool:
        return self.index != self.saved_index

    def undo(self):
        if self.index > 0:
            label = self.states[self.index][0]
            self.index -= 1
            return label, self.states[self.index][1]
        return None

    def redo(self):
        if self.index < len(self.states) - 1:
            self.index += 1
            return self.states[self.index]
        return None


class Document:
    """One open tree project (one tab): its own scene, view, undo history and dirty state."""

    def __init__(self, project: Project, icons: IconLibrary, view_opts: dict) -> None:
        self.project = project
        self.history = History()
        self.meta_dirty = False
        self.other_tags: dict[str, str] = {}
        self.scene = TreeScene(icons)
        for key, value in view_opts.items():
            setattr(self.scene, key, value)
        self.view = TreeView(self.scene)
        project.apply_tag_root()
        self.scene.rebuild(project.tree)
        self.history.reset(project.tree.snapshot())
        self.refresh_other_tags()

    @property
    def path(self) -> Path | None:
        return self.project.path

    @property
    def dirty(self) -> bool:
        return self.history.dirty or self.meta_dirty

    @property
    def is_blank(self) -> bool:
        """An untouched 'Untitled' tab that can be replaced by the next opened project."""
        return self.path is None and not self.dirty and not self.project.tree.nodes

    def refresh_other_tags(self) -> None:
        """Tags owned by every other project, for the cross-tree collision check."""
        self.other_tags = other_project_tags(config.TREES_DIR, self.project.path)

    def tab_text(self) -> str:
        return self.project.meta.name + ("*" if self.dirty else "")


class OutputDialog(QDialog):
    def __init__(self, title: str, text: str, ok_label: str | None, parent=None) -> None:
        super().__init__(parent)
        self.setWindowTitle(title)
        self.resize(820, 520)
        out = QPlainTextEdit(text)
        out.setReadOnly(True)
        out.setStyleSheet("font-family: Consolas, monospace;")
        buttons = QDialogButtonBox()
        if ok_label:
            buttons.addButton(ok_label, QDialogButtonBox.ButtonRole.AcceptRole)
            buttons.addButton(QDialogButtonBox.StandardButton.Cancel)
        else:
            buttons.addButton(QDialogButtonBox.StandardButton.Close)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        layout = QVBoxLayout(self)
        layout.addWidget(out)
        layout.addWidget(buttons)


class MainWindow(QMainWindow):
    def __init__(self) -> None:
        super().__init__()
        self.settings = QSettings(config.ORG_NAME, config.APP_NAME)
        self.catalog = load_catalog()
        self.icons = IconLibrary()
        self.node_data_clipboard: dict | None = None
        self._restoring = False
        self._closing = False  # set once the window accepts close: scenes are torn down after this
        self._doc: Document | None = None
        self.view_opts = {"show_labels": True, "show_icons": True, "show_grid": True, "snap_enabled": False}

        self.tab_bar = QTabBar()
        self.tab_bar.setTabsClosable(True)
        self.tab_bar.setMovable(True)
        self.tab_bar.setDocumentMode(True)
        self.tab_bar.setExpanding(False)
        self.tab_bar.setElideMode(Qt.TextElideMode.ElideRight)
        self.tab_bar.setUsesScrollButtons(True)
        self.tab_bar.setStyleSheet(
            "QTabBar::tab { padding: 6px 14px; min-width: 90px; }"
            "QTabBar::tab:selected { background: #2b3445; border-bottom: 2px solid #60a5fa; }")
        self.tab_bar.currentChanged.connect(self._on_tab_changed)
        self.tab_bar.tabCloseRequested.connect(lambda i: self.close_tab(i))
        self.tab_bar.installEventFilter(self)
        self.stack = QStackedWidget()
        self.header = ProjectHeader()
        self.header.settings_requested.connect(self.project_settings)
        self.header.release_requested.connect(self.mark_live_release)
        central = QWidget()
        central_layout = QVBoxLayout(central)
        central_layout.setContentsMargins(0, 0, 0, 0)
        central_layout.setSpacing(0)
        central_layout.addWidget(self.tab_bar)
        central_layout.addWidget(self.header)
        central_layout.addWidget(self.stack, 1)
        self.setCentralWidget(central)

        self.inspector = Inspector(self.catalog, self.icons)
        self._dock("Inspector", self.inspector, Qt.DockWidgetArea.RightDockWidgetArea)

        self.node_filter = QLineEdit(placeholderText="Filter by tag, name, effect…")
        self.node_filter.setClearButtonEnabled(True)
        self.node_list = QListWidget()
        self.node_list.setSelectionMode(QListWidget.SelectionMode.ExtendedSelection)
        nodes_panel = QWidget()
        lay = QVBoxLayout(nodes_panel)
        lay.setContentsMargins(4, 4, 4, 4)
        lay.addWidget(self.node_filter)
        lay.addWidget(self.node_list)
        self.nodes_dock = self._dock("Nodes", nodes_panel, Qt.DockWidgetArea.LeftDockWidgetArea)

        self.issue_tree = QTreeWidget()
        self.issue_tree.setHeaderLabels(["Severity", "Node", "Message"])
        self.issue_tree.setRootIsDecorated(False)
        self.issue_tree.setColumnWidth(0, 80)
        self.issue_tree.setColumnWidth(1, 300)
        self.issues_dock = self._dock("Validation", self.issue_tree, Qt.DockWidgetArea.BottomDockWidgetArea)

        self.status_counts = QLabel()
        self.status_zoom = QLabel()
        self.status_pos = QLabel()
        for w in (self.status_counts, self.status_zoom, self.status_pos):
            self.statusBar().addPermanentWidget(w)

        self._validate_timer = QTimer(self, singleShot=True, interval=250, timeout=self.run_validation)
        self._list_timer = QTimer(self, singleShot=True, interval=150, timeout=self.refresh_node_list)

        self._build_actions()
        self._connect()
        self.restoreGeometry(self.settings.value("geometry", b""))
        self.restoreState(self.settings.value("windowState", b""))
        self._add_document(Project())

    # ---- setup ---------------------------------------------------------------

    def _dock(self, title: str, widget, area) -> QDockWidget:
        dock = QDockWidget(title, self)
        dock.setObjectName(title)
        dock.setWidget(widget)
        self.addDockWidget(area, dock)
        return dock

    def _act(self, menu: QMenu, text: str, slot, shortcut=None, checkable=False, checked=False,
             tip: str = "") -> QAction:
        a = QAction(text, self)
        if shortcut:
            a.setShortcut(QKeySequence(shortcut))
        if checkable:
            a.setCheckable(True)
            a.setChecked(checked)
            a.toggled.connect(slot)
        else:
            a.triggered.connect(slot)
        if tip:
            a.setStatusTip(tip)
        menu.addAction(a)
        self.addAction(a)
        return a

    def _build_actions(self) -> None:
        mb = self.menuBar()
        f = mb.addMenu("&File")
        self._act(f, "&New Tree Project…", self.new_project, "Ctrl+N")
        self._act(f, "&Open Project…", self.open_dialog, "Ctrl+O",
                  tip="Opens in a new tab (select several files to open them all)")
        self.recent_menu = f.addMenu("Open &Recent")
        f.addSeparator()
        self._act(f, "&Close Tab", lambda: self.close_tab(self.tab_bar.currentIndex()), "Ctrl+W")
        self._act(f, "Close &Other Tabs", self.close_other_tabs)
        self._act(f, "Next Tab", lambda: self._cycle_tab(1), "Ctrl+Tab")
        self._act(f, "Previous Tab", lambda: self._cycle_tab(-1), "Ctrl+Shift+Tab")
        f.addSeparator()
        self._act(f, "&Save", self.save, "Ctrl+S",
                  tip="Saves the .sktree and re-exports the UE JSON next to it")
        self._act(f, "Save &As…", self.save_as, "Ctrl+Shift+S")
        f.addSeparator()
        self._act(f, "Import Legacy Tree JSON as Project…", self.import_legacy_json)
        self._act(f, "Import NodeData Workbook (.xlsx) as Project…", self.import_xlsx,
                  tip="Migrate an old *_NodeData.xlsx (Nodes/Effects/Connections) into a project")
        self._act(f, "Show Project Folder", self.show_project_folder)
        f.addSeparator()
        self._act(f, "E&xit", self.close, "Ctrl+Q")

        e = mb.addMenu("&Edit")
        self.undo_act = self._act(e, "&Undo", self.undo, "Ctrl+Z")
        self.redo_act = self._act(e, "&Redo", self.redo, "Ctrl+Y")
        e.addSeparator()
        self._act(e, "&Copy Nodes", self.copy_nodes, "Ctrl+C")
        self._act(e, "&Paste Nodes", self.paste_nodes, "Ctrl+V")
        self._act(e, "&Duplicate", self.duplicate, "Ctrl+D")
        self._act(e, "&Delete", self.delete_selection, "Delete")
        e.addSeparator()
        self._act(e, "Copy Node &Data (type, name, text, icon, effects)", self.copy_node_data, "Ctrl+Shift+C")
        self._act(e, "Paste Node Data onto Selection", self.paste_node_data, "Ctrl+Shift+V")
        e.addSeparator()
        self._act(e, "&Link / Unlink Two Selected", self.toggle_link_selected, "L")
        self._act(e, "Select &All", self.select_all, "Ctrl+A")
        self._act(e, "Select Connected", self.select_connected, "Ctrl+Shift+A")

        v = mb.addMenu("&View")
        self._act(v, "&Fit All", lambda: self.view.fit_all(), "F")
        self._act(v, "Frame Selection", self.frame_selection, "Shift+F")
        v.addSeparator()
        self._act(v, "Show &Labels", self._set_labels, "Ctrl+1", True, True)
        self._act(v, "Show &Icons", self._set_icons, "Ctrl+2", True, True)
        self._act(v, "Show &Grid", self._set_grid, "Ctrl+3", True, True)
        self._act(v, "&Snap to Grid", self._set_snap, "G", True, False)
        v.addSeparator()
        for dock in (self.nodes_dock, self.issues_dock):
            v.addAction(dock.toggleViewAction())

        p = mb.addMenu("&Project")
        self._act(p, "Project &Settings…", self.project_settings, "Ctrl+,")
        bump_menu = p.addMenu("&Bump Working Version")
        for part in ("patch", "minor", "major"):
            self._act(bump_menu, part.capitalize(), lambda _c=False, part=part: self.bump_version(part))
        p.addSeparator()
        self._act(p, "Mark &Live Release…", self.mark_live_release)
        self._act(p, "Release &History…", self.release_history)

        t = mb.addMenu("&Tools")
        self._act(t, "&Validate", self.run_validation, "F5")
        self._act(t, "&Renumber Tags (fix placeholders / duplicates)", self.renumber)
        t.addSeparator()
        self._act(t, "Register Tags in C++ (node, SkillPassive, gated stat tags)…", self.register_tags)
        self._act(t, "Copy Export JSON Path (for UE importer)", self.copy_json_path)
        self._act(t, "Reload Tag Catalog", self.reload_catalog)

        h = mb.addMenu("&Help")
        self._act(h, "Controls…", self.show_controls, "F1")

        self._rebuild_recent_menu()

    def _connect(self) -> None:
        self.inspector.edited.connect(self._on_inspector_edit)
        self.node_filter.textChanged.connect(lambda _t: self._list_timer.start())
        self.node_list.itemSelectionChanged.connect(self._on_list_selection)
        self.issue_tree.itemActivated.connect(self._on_issue_activated)
        self.issue_tree.itemClicked.connect(self._on_issue_activated)

    def _wire_document(self, doc: Document) -> None:
        """Scene/view signals only act while their tab is the active one."""
        def active(fn):
            return lambda *a: fn(*a) if doc is self._doc and not self._closing else None
        doc.scene.committed.connect(active(self.commit))
        doc.scene.add_node_requested.connect(active(self.add_node_at))
        doc.scene.selectionChanged.connect(active(self._on_selection))
        doc.view.cursor_moved.connect(active(lambda p: self.status_pos.setText(f"X {p.x():.0f}  Y {p.y():.0f}")))
        doc.view.zoom_changed.connect(active(lambda z: self.status_zoom.setText(f"Zoom {z * 100:.0f}%")))
        doc.view.context_menu.connect(active(self._context_menu))
        doc.view.nudge.connect(active(self._nudge))

    # ---- active document (the current tab) ------------------------------------------

    @property
    def doc(self) -> Document:
        return self._doc

    @property
    def docs(self) -> list[Document]:
        return [self.tab_bar.tabData(i) for i in range(self.tab_bar.count())]

    @property
    def project(self) -> Project:
        return self._doc.project

    @property
    def history(self) -> History:
        return self._doc.history

    @property
    def scene(self) -> TreeScene:
        return self._doc.scene

    @property
    def view(self) -> TreeView:
        return self._doc.view

    @property
    def other_tags(self) -> dict[str, str]:
        return self._doc.other_tags

    @property
    def meta_dirty(self) -> bool:
        return self._doc.meta_dirty

    @meta_dirty.setter
    def meta_dirty(self, value: bool) -> None:
        self._doc.meta_dirty = value

    @property
    def path(self) -> Path | None:
        return self._doc.path

    @property
    def dirty(self) -> bool:
        return self._doc.dirty

    # ---- tabs ------------------------------------------------------------------------

    def _add_document(self, project: Project) -> Document:
        """Open a project in a new tab (reusing a blank Untitled tab) and make it current."""
        reuse = self._doc if self._doc is not None and self._doc.is_blank else None
        doc = Document(project, self.icons, self.view_opts)
        self._wire_document(doc)
        self.stack.addWidget(doc.view)
        index = self.tab_bar.addTab(doc.tab_text())
        self.tab_bar.setTabData(index, doc)
        self.tab_bar.setCurrentIndex(index)
        self._on_tab_changed(index)  # the first addTab fires currentChanged before setTabData
        if reuse is not None:
            self._remove_tab(self.docs.index(reuse))
        QTimer.singleShot(0, doc.view.fit_all)
        return doc

    def _find_open(self, path: Path) -> int:
        target = path.resolve()
        for i, d in enumerate(self.docs):
            if d.path and d.path.resolve() == target:
                return i
        return -1

    def _on_tab_changed(self, index: int) -> None:
        new = self.tab_bar.tabData(index) if index >= 0 else None
        if new is None or new is self._doc:
            return
        if self._doc is not None and self._doc in self.docs:
            self.inspector.flush()  # pending field edits commit to the tab being left
        self._doc = new
        self.stack.setCurrentWidget(new.view)
        self.inspector.set_nodes(new.scene.selected_nodes())
        self.status_zoom.setText(f"Zoom {new.view.zoom * 100:.0f}%")
        self._after_change(validate=False)
        self.run_validation()
        self.refresh_node_list()

    def _remove_tab(self, index: int) -> None:
        doc = self.tab_bar.tabData(index)
        self.tab_bar.removeTab(index)  # may switch current tab
        self.stack.removeWidget(doc.view)
        doc.view.deleteLater()
        doc.scene.deleteLater()

    def close_tab(self, index: int) -> bool:
        if index < 0:
            return False
        doc = self.tab_bar.tabData(index)
        if self.tab_bar.count() == 1 and doc.is_blank:
            return True
        if doc.dirty:
            self.tab_bar.setCurrentIndex(index)
            if not self.maybe_save():
                return False
        if self.tab_bar.count() == 1:
            self._add_document(Project())  # always keep one tab; the blank one replaces nothing
            index = self.docs.index(doc)
        self._remove_tab(index)
        self._save_session()
        return True

    def close_other_tabs(self) -> None:
        keep = self._doc
        for d in [d for d in self.docs if d is not keep]:
            if not self.close_tab(self.docs.index(d)):
                return
        self.tab_bar.setCurrentIndex(self.docs.index(keep))

    def _cycle_tab(self, step: int) -> None:
        n = self.tab_bar.count()
        if n > 1:
            self.tab_bar.setCurrentIndex((self.tab_bar.currentIndex() + step) % n)

    def _update_tab(self, doc: Document) -> None:
        i = self.docs.index(doc)
        self.tab_bar.setTabText(i, doc.tab_text())
        m = doc.project.meta
        self.tab_bar.setTabToolTip(i, f"{m.name} v{m.version} ({m.tree_type})\n{doc.path or 'not saved yet'}")

    def eventFilter(self, obj, event) -> bool:
        # Middle-click a tab to close it, like a browser.
        if obj is self.tab_bar and event.type() == QEvent.Type.MouseButtonRelease \
                and event.button() == Qt.MouseButton.MiddleButton:
            index = self.tab_bar.tabAt(event.position().toPoint())
            if index >= 0:
                self.close_tab(index)
                return True
        return super().eventFilter(obj, event)

    def _save_session(self) -> None:
        paths = [str(d.path) for d in self.docs if d.path]
        self.settings.setValue("openTabs", paths)
        self.settings.setValue("currentTab", str(self.path) if self.path else "")

    def restore_session(self) -> None:
        """Reopen the tabs from last time (falls back to the most recent project)."""
        v = self.settings.value("openTabs", [])
        paths = [v] if isinstance(v, str) else list(v or [])
        paths = [p for p in paths if p.endswith(PROJECT_EXT) and Path(p).is_file()]
        current = self.settings.value("currentTab", "")  # read first: each open_path re-saves it
        if not paths:
            self.open_last()
            return
        for p in paths:
            self.open_path(Path(p), quiet=True)
        if current and (i := self._find_open(Path(current))) >= 0:
            self.tab_bar.setCurrentIndex(i)
        self._save_session()

    # ---- document lifecycle ----------------------------------------------------

    def _refresh_other_tags(self) -> None:
        for d in self.docs:
            d.refresh_other_tags()

    def maybe_save(self) -> bool:
        """Ask to save the CURRENT tab if it has unsaved changes."""
        if not self.dirty:
            return True
        r = QMessageBox.question(self, "Unsaved changes", f"Save changes to {self.project.meta.name}?",
                                 QMessageBox.StandardButton.Save | QMessageBox.StandardButton.Discard
                                 | QMessageBox.StandardButton.Cancel)
        if r == QMessageBox.StandardButton.Save:
            return self.save()
        return r == QMessageBox.StandardButton.Discard

    def _ask_new_project_meta(self, meta: ProjectMeta, title_hint: str = "") -> Path | None:
        """Settings dialog in 'new project' mode; returns the chosen .sktree path or None."""
        dlg = ProjectSettingsDialog(meta, self.catalog, self, new_project=True)
        if title_hint:
            dlg.setWindowTitle(title_hint)
        if not dlg.exec():
            return None
        path = dlg.project_path()
        if path.exists() and QMessageBox.question(
                self, "Overwrite?", f"{path} already exists. Overwrite it?") != QMessageBox.StandardButton.Yes:
            return None
        return path

    def _finish_new_project(self, project: Project, path: Path) -> None:
        if (i := self._find_open(path)) >= 0:  # overwriting a project that's open: drop its tab
            self._remove_tab(i)
        project.apply_tag_root()
        self._add_document(project)
        project.path = path
        self.save()

    def new_project(self) -> None:
        meta = ProjectMeta(name="NewSkill", author=os.environ.get("USERNAME", ""))
        path = self._ask_new_project_meta(meta)
        if path:
            if not meta.tag_root:
                prefix = "PassiveTree" if meta.tree_type == "Passive" else f"SkillTree.{path.stem}"
                meta.tag_root = prefix
            self._finish_new_project(Project(meta=meta), path)

    def open_dialog(self) -> None:
        start = str(self.path.parent if self.path else config.TREES_DIR)
        paths, _chosen = QFileDialog.getOpenFileNames(self, "Open Skill Tree Project(s)", start,
                                                      f"{PROJECT_FILTER};;{LEGACY_FILTER}")
        for path in paths:
            if path.lower().endswith(".json"):
                self.import_legacy_json(Path(path))
            else:
                self.open_path(Path(path))

    def open_path(self, path: Path, confirm: bool = True, quiet: bool = False) -> bool:
        """Open a project in its own tab, or switch to its tab if it's already open.

        `confirm` is accepted for backwards compatibility; opening never replaces another tab.
        """
        if (i := self._find_open(path)) >= 0:
            self.tab_bar.setCurrentIndex(i)
            return True
        try:
            project = Project.load(path)
        except (OSError, ValueError, TypeError) as exc:
            if not quiet:
                QMessageBox.critical(self, "Open failed", f"{path}\n\n{exc}")
            return False
        self._add_document(project)
        self._add_recent(path)
        self._save_session()
        t = project.tree
        self.statusBar().showMessage(f"Opened {project.meta.name}: {len(t.nodes)} nodes, {len(t.edges)} links", 5000)
        return True

    def save(self) -> bool:
        if self.path is None:
            return self.save_as()
        self.inspector.flush()
        try:
            self.project.save()
        except OSError as exc:
            QMessageBox.critical(self, "Save failed", str(exc))
            return False
        self.history.saved_index = self.history.index
        self.meta_dirty = False
        self._refresh_other_tags()  # this tree's tags changed on disk: every tab's cross-tree check
        self._after_change()
        self._add_recent(self.path)
        self._save_session()
        self.statusBar().showMessage(
            f"Saved {self.path.name} (rev {self.project.meta.revision}) → exported {self.project.export_path.name}", 5000)
        return True

    def save_as(self) -> bool:
        start = str(self.path or suggested_folder(self.project.meta) / (self.project.meta.name + PROJECT_EXT))
        path, _ = QFileDialog.getSaveFileName(self, "Save Tree Project", start, PROJECT_FILTER)
        if not path:
            return False
        other = self._find_open(Path(path))
        if other >= 0 and self.tab_bar.tabData(other) is not self._doc:
            QMessageBox.warning(self, "Already open", f"{Path(path).name} is open in another tab. "
                                "Close that tab first or pick a different name.")
            return False
        self.project.path = Path(path)
        return self.save()

    def import_legacy_json(self, path: Path | None = None) -> None:
        if path is None:
            p, _ = QFileDialog.getOpenFileName(self, "Import Legacy Tree JSON", str(config.SKILLTREE_DIR), LEGACY_FILTER)
            if not p:
                return
            path = Path(p)
        try:
            with open(path, encoding="utf-8") as f:
                head = json.load(f)
        except (OSError, ValueError) as exc:
            QMessageBox.critical(self, "Import failed", f"{path}\n\n{exc}")
            return
        name = path.stem.replace("PassiveTree", "").replace("Tree", "") or path.stem
        meta = ProjectMeta(name=name, tree_type="Passive" if "Passive" in path.stem else "Skill",
                           author=os.environ.get("USERNAME", ""))
        if "treeInfo" in head:
            QMessageBox.information(self, "Exported file", "This JSON was exported by the editor — open its "
                                    f"{PROJECT_EXT} project instead if you have it. Importing anyway.")
        new_path = self._ask_new_project_meta(meta, "Create Project from Legacy JSON")
        if new_path:
            self._finish_new_project(Project.from_tree_json(path, meta), new_path)

    def import_xlsx(self) -> None:
        path, _ = QFileDialog.getOpenFileName(self, "Import NodeData Workbook", str(config.SKILLTREE_DIR),
                                              "Excel workbook (*.xlsx)")
        if not path:
            return
        try:
            tree, warnings = import_node_data_xlsx(path)
        except Exception as exc:  # openpyxl raises a wide variety
            QMessageBox.critical(self, "Import failed", f"{path}\n\n{exc}")
            return
        if warnings:
            QMessageBox.information(self, "Import warnings", "\n".join(warnings[:30]))
        stem = Path(path).stem.replace("_NodeData", "").replace("PassiveTree", "").replace("Tree", "")
        meta = ProjectMeta(name=stem or "Imported", author=os.environ.get("USERNAME", ""),
                           tree_type="Passive" if "Passive" in Path(path).stem else "Skill")
        new_path = self._ask_new_project_meta(meta, "Create Project from Workbook")
        if new_path:
            self._finish_new_project(Project(tree, meta), new_path)

    def show_project_folder(self) -> None:
        if self.project.folder:
            os.startfile(self.project.folder)

    # ---- project metadata -------------------------------------------------------

    def _meta_changed(self) -> None:
        self.meta_dirty = True
        self.project.apply_tag_root()
        self._after_change()

    def project_settings(self) -> None:
        dlg = ProjectSettingsDialog(self.project.meta, self.catalog, self)
        if dlg.exec():
            self._meta_changed()

    def bump_version(self, part: str) -> None:
        m = self.project.meta
        old = m.version
        m.version = bump(m.version, part)
        self._meta_changed()
        self.statusBar().showMessage(f"Working version v{old} → v{m.version}", 4000)

    def mark_live_release(self) -> None:
        if self.path is None or self.dirty:
            if QMessageBox.question(self, "Save first", "Save the project before releasing?") \
                    != QMessageBox.StandardButton.Yes or not self.save():
                return
        issues = [i for i in validate(self.project.tree, self.catalog, self.icons,
                                      self.project.live_snapshot(), self.project.meta.live_version)
                  if i.severity == ERROR]
        if issues and QMessageBox.warning(
                self, "Validation errors", f"{len(issues)} validation error(s). Release anyway?",
                QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No) != QMessageBox.StandardButton.Yes:
            return
        dlg = ReleaseDialog(self.project, self)
        if not dlg.exec():
            return
        release = self.project.mark_live(dlg.notes.toPlainText().strip(), dlg.bump.isChecked())
        self.meta_dirty = True
        self.save()
        QMessageBox.information(self, "Released", f"{self.project.meta.name} v{release.version} is now the live "
                                f"version.\nSnapshot: {release.file}\nWorking version: v{self.project.meta.version}")

    def release_history(self) -> None:
        ReleaseHistoryDialog(self.project, self).exec()

    # ---- recent files --------------------------------------------------------

    def _recent(self) -> list[str]:
        v = self.settings.value("recent", [])
        items = [v] if isinstance(v, str) else list(v or [])
        return [p for p in items if p.endswith(PROJECT_EXT)]

    def _add_recent(self, path: Path) -> None:
        items = [str(path)] + [p for p in self._recent() if p != str(path)]
        self.settings.setValue("recent", items[:10])
        self._rebuild_recent_menu()

    def _rebuild_recent_menu(self) -> None:
        self.recent_menu.clear()
        for p in self._recent():
            a = self.recent_menu.addAction(p)
            a.triggered.connect(lambda _c=False, p=p: self.open_path(Path(p)))
        if self._recent():
            self.recent_menu.addSeparator()
        self.recent_menu.addAction("Open All Projects in Trees\\", self.open_all_trees)

    def open_last(self) -> None:
        for p in self._recent():
            if p.endswith(PROJECT_EXT) and Path(p).is_file():
                self.open_path(Path(p), quiet=True)
                return

    def open_all_trees(self) -> None:
        for p in sorted(config.TREES_DIR.rglob("*" + PROJECT_EXT)):
            self.open_path(p, quiet=True)

    # ---- change tracking -----------------------------------------------------

    def commit(self, label: str) -> None:
        if self._restoring:
            return
        self.history.push(label, self.scene.tree.snapshot())
        self._after_change()
        if label in ("Move",):
            self._sync_inspector_position()

    def _after_change(self, validate: bool = True) -> None:
        self._update_title()
        self._update_counts()
        if validate:
            self._validate_timer.start()
        self._list_timer.start()

    def _update_title(self) -> None:
        m = self.project.meta
        star = "*" if self.dirty else ""
        self.setWindowTitle(f"{m.name} v{m.version}{star} — {config.APP_NAME}")
        self.header.update_from(self.project, self.dirty)
        self._update_tab(self._doc)
        self.undo_act.setEnabled(self.history.index > 0)
        self.redo_act.setEnabled(self.history.index < len(self.history.states) - 1)

    def _update_counts(self) -> None:
        t = self.scene.tree
        self.status_counts.setText(f"{len(t.nodes)} nodes · {len(t.edges)} links")

    def _restore(self, snap) -> None:
        selected = self.scene.selected_uids()
        self._restoring = True
        tree = self.scene.tree
        tree.restore(snap)
        self.scene.rebuild(tree)
        self.scene.select_uids([u for u in selected if u in tree.nodes])
        self._restoring = False
        self._after_change()

    def undo(self) -> None:
        self.inspector.flush()
        r = self.history.undo()
        if r:
            self._restore(r[1])
            self.statusBar().showMessage(f"Undo {r[0]}", 2000)

    def redo(self) -> None:
        r = self.history.redo()
        if r:
            self._restore(r[1])
            self.statusBar().showMessage(f"Redo {r[0]}", 2000)

    # ---- selection / inspector -----------------------------------------------

    def _on_selection(self) -> None:
        self.inspector.flush()
        self.inspector.set_nodes(self.scene.selected_nodes())

    def _on_inspector_edit(self, label: str, uids: list) -> None:
        self.scene.refresh_nodes(uids)
        self.commit(label)

    def _sync_inspector_position(self) -> None:
        if len(self.inspector.nodes) == 1:
            self.inspector.refresh_fields()

    # ---- node list -------------------------------------------------------------

    def refresh_node_list(self) -> None:
        text = self.node_filter.text().strip().lower()
        selected = set(self.scene.selected_uids())
        self.node_list.blockSignals(True)
        self.node_list.clear()
        nodes = sorted(self.scene.tree.nodes.values(), key=lambda n: n.tag)
        for n in nodes:
            hay = " ".join([n.tag, n.name, n.description] + [e.summary() for e in n.effects]).lower()
            if text and text not in hay:
                continue
            item = QListWidgetItem(f"{n.tag}   —   {n.name}")
            item.setData(Qt.ItemDataRole.UserRole, n.uid)
            if n.uid in self.scene.issue_uids:
                item.setForeground(Qt.GlobalColor.red)
            self.node_list.addItem(item)
            if n.uid in selected:
                item.setSelected(True)
        self.node_list.blockSignals(False)

    def _on_list_selection(self) -> None:
        uids = [i.data(Qt.ItemDataRole.UserRole) for i in self.node_list.selectedItems()]
        if uids:
            self.scene.select_uids(uids, centre_view=self.view if len(uids) == 1 else None)
            if len(uids) > 1:
                self.frame_selection()

    # ---- validation --------------------------------------------------------------

    def run_validation(self) -> None:
        tree = self.scene.tree
        issues = validate(tree, self.catalog, self.icons,
                          self.project.live_snapshot(), self.project.meta.live_version,
                          self.other_tags)
        self.issue_tree.clear()
        flagged: set[int] = set()
        counts = {ERROR: 0, WARNING: 0, INFO: 0}
        for issue in issues:
            counts[issue.severity] += 1
            node = tree.nodes.get(issue.uid) if issue.uid else None
            item = QTreeWidgetItem([issue.severity, node.tag if node else "(tree)", issue.message])
            item.setData(0, Qt.ItemDataRole.UserRole, issue.uid)
            color = {ERROR: "#f87171", WARNING: "#fbbf24", INFO: "#9ca3af"}[issue.severity]
            item.setForeground(0, QColor(color))
            self.issue_tree.addTopLevelItem(item)
            if issue.uid and issue.severity in (ERROR, WARNING):
                flagged.add(issue.uid)
        self.scene.set_issue_uids(flagged)
        self.issues_dock.setWindowTitle(
            f"Validation — {counts[ERROR]} errors, {counts[WARNING]} warnings, {counts[INFO]} info")

    def _on_issue_activated(self, item: QTreeWidgetItem, _col: int = 0) -> None:
        uid = item.data(0, Qt.ItemDataRole.UserRole)
        if uid:
            self.scene.select_uids([uid], centre_view=self.view)

    # ---- edit actions ---------------------------------------------------------------

    def add_node_at(self, pos: QPointF) -> None:
        sel = self.scene.selected_nodes()
        template = sel[0] if len(sel) == 1 else None
        node = self.scene.create_node(pos, template)
        if template is not None:  # auto-link from the node you were working from
            self.scene.toggle_link(template.uid, node.uid)
        self.scene.select_uids([node.uid])
        self.commit("Add node")

    def delete_selection(self) -> None:
        if self.scene.delete_selection():
            self.commit("Delete")

    def toggle_link_selected(self) -> None:
        uids = self.scene.selected_uids()
        if len(uids) != 2:
            self.statusBar().showMessage("Select exactly two nodes to link/unlink (or Shift+drag between nodes).", 4000)
            return
        self.scene.toggle_link(*uids)
        self.commit("Link")

    def _fragment_from_selection(self) -> dict | None:
        uids = self.scene.selected_uids()
        return self.scene.tree.fragment(uids) if uids else None

    def copy_nodes(self) -> None:
        if self._focus_in_text_field():
            return
        frag = self._fragment_from_selection()
        if frag:
            QApplication.clipboard().setText(json.dumps({CLIP_MIME: frag}))
            self.statusBar().showMessage(f"Copied {len(frag['nodes'])} node(s)", 2000)

    def paste_nodes(self) -> None:
        if self._focus_in_text_field():
            return
        try:
            frag = json.loads(QApplication.clipboard().text())[CLIP_MIME]
        except (ValueError, KeyError, TypeError):
            return
        centre = self.view.mapToScene(self.view.mapFromGlobal(self.cursor().pos()))
        if not self.view.viewport().rect().contains(self.view.mapFromGlobal(self.cursor().pos())):
            centre = self.view.mapToScene(self.view.viewport().rect().center())
        uids = self.scene.paste(frag, centre)
        self.scene.select_uids(uids)
        self.commit("Paste")

    def duplicate(self) -> None:
        frag = self._fragment_from_selection()
        if not frag:
            return
        xs = [n["X"] for n in frag["nodes"]]
        ys = [n["Y"] for n in frag["nodes"]]
        centre = QPointF((min(xs) + max(xs)) / 2 + 60, (min(ys) + max(ys)) / 2 + 60)
        uids = self.scene.paste(frag, centre)
        self.scene.select_uids(uids)
        self.commit("Duplicate")

    def copy_node_data(self) -> None:
        nodes = self.scene.selected_nodes()
        if len(nodes) != 1:
            self.statusBar().showMessage("Select one node to copy its data from.", 3000)
            return
        self.node_data_clipboard = nodes[0].content_json()
        self.statusBar().showMessage(f"Copied data from {nodes[0].tag}", 3000)

    def paste_node_data(self) -> None:
        nodes = self.scene.selected_nodes()
        if not self.node_data_clipboard or not nodes:
            return
        for n in nodes:
            n.apply_content_json(self.node_data_clipboard)
        self.scene.refresh_nodes([n.uid for n in nodes])
        self.inspector.set_nodes(nodes)
        self.commit("Paste node data")
        self.statusBar().showMessage(f"Pasted node data onto {len(nodes)} node(s)", 3000)

    def select_all(self) -> None:
        if self._focus_in_text_field():
            w = QApplication.focusWidget()
            w.selectAll()
            return
        self.scene.select_uids(list(self.scene.node_items))

    def select_connected(self) -> None:
        tree = self.scene.tree
        seen = set(self.scene.selected_uids())
        frontier = list(seen)
        while frontier:
            for o in tree.neighbors(frontier.pop()):
                if o not in seen:
                    seen.add(o)
                    frontier.append(o)
        self.scene.select_uids(list(seen))

    def frame_selection(self) -> None:
        items = [i for i in self.scene.selectedItems() if isinstance(i, NodeItem)]
        if not items:
            return
        rect = items[0].sceneBoundingRect()
        for i in items[1:]:
            rect = rect.united(i.sceneBoundingRect())
        self.view.fitInView(rect.adjusted(-150, -150, 150, 150), Qt.AspectRatioMode.KeepAspectRatio)
        self.view.zoom_changed.emit(self.view.zoom)

    def _nudge(self, dx: float, dy: float) -> None:
        if self.scene.nudge_selection(dx, dy):
            self.commit("Move")

    def _focus_in_text_field(self) -> bool:
        return isinstance(QApplication.focusWidget(), (QLineEdit, QPlainTextEdit))

    def _context_menu(self, scene_pos: QPointF, global_pos) -> None:
        hit = self.scene.node_at(scene_pos)
        if hit and not hit.isSelected():
            self.scene.select_uids([hit.node.uid])
        menu = QMenu(self)
        menu.addAction("Add Node Here", lambda: self.add_node_at(scene_pos))
        if self.scene.selectedItems():
            menu.addSeparator()
            menu.addAction("Duplicate", self.duplicate)
            menu.addAction("Copy Node Data", self.copy_node_data)
            if self.node_data_clipboard:
                menu.addAction("Paste Node Data onto Selection", self.paste_node_data)
            if len(self.scene.selected_uids()) == 2:
                menu.addAction("Link / Unlink", self.toggle_link_selected)
            menu.addAction("Select Connected", self.select_connected)
            menu.addSeparator()
            menu.addAction("Delete", self.delete_selection)
        menu.exec(global_pos)

    # ---- view toggles --------------------------------------------------------------

    def _set_view_opt(self, key: str, on: bool) -> None:
        """View toggles apply to every open tab (and to tabs opened later)."""
        self.view_opts[key] = on
        for d in self.docs:
            setattr(d.scene, key, on)
            d.scene.update()

    def _set_labels(self, on: bool) -> None:
        self._set_view_opt("show_labels", on)

    def _set_icons(self, on: bool) -> None:
        self._set_view_opt("show_icons", on)

    def _set_grid(self, on: bool) -> None:
        self._set_view_opt("show_grid", on)

    def _set_snap(self, on: bool) -> None:
        self._set_view_opt("snap_enabled", on)
        self.statusBar().showMessage(f"Snap to grid {'on' if on else 'off'} ({config.SNAP_SIZE:g} units)", 2000)

    # ---- tools ----------------------------------------------------------------------

    def renumber(self) -> None:
        renames = self.scene.tree.renumber()
        if not renames:
            self.statusBar().showMessage("All tags already unique and indexed.", 3000)
            return
        self.scene.refresh_nodes()
        self.inspector.refresh_fields()
        self.commit("Renumber tags")
        QMessageBox.information(self, "Renumbered", "\n".join(f"{a}  →  {b}" for a, b in renames[:60])
                                + (f"\n… and {len(renames) - 60} more" if len(renames) > 60 else ""))

    def reload_catalog(self) -> None:
        fresh = load_catalog()
        self.catalog.__dict__.update(fresh.__dict__)
        self.inspector.refresh_fields()
        self.run_validation()
        self.statusBar().showMessage(f"Tag catalog reloaded: {len(self.catalog.registered)} registered tags", 3000)

    def copy_json_path(self) -> None:
        if not self.path:
            self.statusBar().showMessage("Save the project first.", 3000)
            return
        QApplication.clipboard().setText(str(self.project.export_path))
        self.statusBar().showMessage(f"Copied {self.project.export_path}", 3000)

    def register_tags(self) -> None:
        if self.path is None or self.dirty:
            if QMessageBox.question(self, "Save first", "The tree must be saved before registering tags. Save now?") \
                    != QMessageBox.StandardButton.Yes or not self.save():
                return
        script = config.IMPORT_TREE_TAGS_SCRIPT
        if not script.is_file():
            QMessageBox.critical(self, "Script missing", f"Not found: {script}")
            return

        def run(*extra: str) -> str:
            env = dict(os.environ, PYTHONIOENCODING="utf-8", PYTHONUTF8="1")
            proc = subprocess.run([sys.executable, str(script), str(self.project.export_path), *extra],
                                  capture_output=True, text=True, encoding="utf-8", errors="replace",
                                  env=env, cwd=str(script.parent))
            return (proc.stdout or "") + (proc.stderr or "")

        preview = run("--dry-run")
        dlg = OutputDialog("Register Tags — preview (dry run)", preview, "Write to FP_GameplayTags", self)
        if dlg.exec():
            result = run()
            OutputDialog("Register Tags — result", result, None, self).exec()
            self.reload_catalog()

    def show_controls(self) -> None:
        QMessageBox.information(self, "Controls", (
            "Canvas\n"
            "  Mouse wheel ............ zoom\n"
            "  Middle drag / Space+drag  pan\n"
            "  Drag empty space ....... box select (Ctrl+click toggles)\n"
            "  Double-click empty ..... add node (copies + links from the selected node)\n"
            "  Shift+drag node→node ... link / unlink\n"
            "  L ...................... link/unlink two selected nodes\n"
            "  Arrows (Shift = x10) ... nudge selection\n"
            "  Delete ................. delete nodes / selected links\n"
            "  F / Shift+F ............ fit all / frame selection\n"
            "  G ...................... snap to grid\n\n"
            "Editing\n"
            "  Ctrl+C / Ctrl+V / Ctrl+D  copy / paste / duplicate nodes (with their links)\n"
            "  Ctrl+Shift+C / V ....... copy node data → paste onto every selected node\n"
            "  Ctrl+Z / Ctrl+Y ........ undo / redo\n"
            "  F5 ..................... validate\n\n"
            "Tabs\n"
            "  Ctrl+O ................. open project(s) in new tabs\n"
            "  Ctrl+Tab / Ctrl+Shift+Tab  next / previous tab\n"
            "  Ctrl+W / middle-click .. close tab (drag tabs to reorder)\n"
            "  Copy/paste nodes works between tabs"))

    # ---- shutdown ---------------------------------------------------------------------

    def closeEvent(self, event: QCloseEvent) -> None:
        self.inspector.flush()
        self._save_session()  # remember the tabs before any get closed by the prompts below
        session = (self.settings.value("openTabs", []), self.settings.value("currentTab", ""))
        for d in self.docs:
            if d.dirty:
                self.tab_bar.setCurrentIndex(self.docs.index(d))
                if not self.maybe_save():
                    event.ignore()
                    return
        self.settings.setValue("openTabs", session[0])
        self.settings.setValue("currentTab", session[1])
        self.settings.setValue("geometry", self.saveGeometry())
        self.settings.setValue("windowState", self.saveState())
        # Qt destroys the scenes after this, which clears their selection and emits
        # selectionChanged into half-deleted objects. Stop listening first.
        self._closing = True
        for d in self.docs:
            d.scene.blockSignals(True)
            d.view.blockSignals(True)
        self._validate_timer.stop()
        self._list_timer.stop()
        event.accept()
