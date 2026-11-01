"""Searchable icon browser over the PNG library in Content/Assets/UIElements/Icons."""
from __future__ import annotations

from pathlib import Path

from PySide6.QtCore import QAbstractListModel, QModelIndex, QSize, QSortFilterProxyModel, Qt
from PySide6.QtWidgets import (QDialog, QDialogButtonBox, QLabel, QLineEdit, QListView,
                               QVBoxLayout)

from .icons import IconLibrary


class _IconModel(QAbstractListModel):
    def __init__(self, icons: IconLibrary) -> None:
        super().__init__()
        self.icons = icons
        self.entries: list[tuple[str, Path]] = icons.entries()

    def rowCount(self, parent=QModelIndex()) -> int:
        return 0 if parent.isValid() else len(self.entries)

    def data(self, index, role=Qt.ItemDataRole.DisplayRole):
        if not index.isValid():
            return None
        ue_path, png = self.entries[index.row()]
        if role == Qt.ItemDataRole.DisplayRole:
            return png.stem
        if role == Qt.ItemDataRole.DecorationRole:
            return self.icons.pixmap(ue_path, 64)  # loaded lazily, only for visible rows
        if role == Qt.ItemDataRole.ToolTipRole:
            return ue_path
        if role == Qt.ItemDataRole.UserRole:
            return ue_path
        if role == Qt.ItemDataRole.UserRole + 1:
            return f"{png.parent.name}/{png.stem}"
        return None


class IconPickerDialog(QDialog):
    def __init__(self, icons: IconLibrary, current: str = "", parent=None) -> None:
        super().__init__(parent)
        self.setWindowTitle("Choose Node Icon")
        self.resize(900, 640)
        self.selected_path = current

        self.filter = QLineEdit(placeholderText="Search icons… (e.g. sword, shield, lightning)")
        self.filter.setClearButtonEnabled(True)

        self.model = _IconModel(icons)
        self.proxy = QSortFilterProxyModel(self)
        self.proxy.setSourceModel(self.model)
        self.proxy.setFilterRole(Qt.ItemDataRole.UserRole + 1)
        self.proxy.setFilterCaseSensitivity(Qt.CaseSensitivity.CaseInsensitive)
        self.filter.textChanged.connect(self.proxy.setFilterFixedString)

        self.view = QListView()
        self.view.setViewMode(QListView.ViewMode.IconMode)
        self.view.setResizeMode(QListView.ResizeMode.Adjust)
        self.view.setUniformItemSizes(True)
        self.view.setIconSize(QSize(64, 64))
        self.view.setGridSize(QSize(110, 100))
        self.view.setWordWrap(True)
        self.view.setModel(self.proxy)
        self.view.doubleClicked.connect(self.accept)
        self.view.selectionModel().currentChanged.connect(self._on_current)

        self.path_label = QLabel(current or "—")
        self.path_label.setTextInteractionFlags(Qt.TextInteractionFlag.TextSelectableByMouse)

        buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)

        layout = QVBoxLayout(self)
        layout.addWidget(self.filter)
        layout.addWidget(self.view, 1)
        layout.addWidget(self.path_label)
        layout.addWidget(buttons)

        if not self.model.entries:
            self.path_label.setText(f"No PNG icons found under {icons.library_dir}")
        self.filter.setFocus()

    def _on_current(self, current, _previous) -> None:
        if current.isValid():
            self.selected_path = current.data(Qt.ItemDataRole.UserRole)
            self.path_label.setText(self.selected_path)
