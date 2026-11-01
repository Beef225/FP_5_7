"""Skill Tree Editor — entry point.

    python main.py                      # reopens the last project
    python main.py path/to/Tree.sktree  # open a project
    python main.py path/to/Tree.json    # create a project from a legacy tree JSON
"""
from __future__ import annotations

import sys
from pathlib import Path

from PySide6.QtGui import QColor, QPalette
from PySide6.QtWidgets import QApplication

from skilltree_editor import config
from skilltree_editor.mainwindow import MainWindow


def apply_dark_theme(app: QApplication) -> None:
    app.setStyle("Fusion")
    p = QPalette()
    roles = {
        QPalette.ColorRole.Window: "#24262b", QPalette.ColorRole.WindowText: "#e5e7eb",
        QPalette.ColorRole.Base: "#1b1d22", QPalette.ColorRole.AlternateBase: "#24262b",
        QPalette.ColorRole.ToolTipBase: "#111318", QPalette.ColorRole.ToolTipText: "#e5e7eb",
        QPalette.ColorRole.Text: "#e5e7eb", QPalette.ColorRole.Button: "#2d3038",
        QPalette.ColorRole.ButtonText: "#e5e7eb", QPalette.ColorRole.Highlight: "#0e7490",
        QPalette.ColorRole.HighlightedText: "#ffffff", QPalette.ColorRole.PlaceholderText: "#6b7280",
        QPalette.ColorRole.Link: "#22d3ee",
    }
    for role, color in roles.items():
        p.setColor(role, QColor(color))
    p.setColor(QPalette.ColorGroup.Disabled, QPalette.ColorRole.Text, QColor("#6b7280"))
    p.setColor(QPalette.ColorGroup.Disabled, QPalette.ColorRole.ButtonText, QColor("#6b7280"))
    app.setPalette(p)


def main() -> int:
    app = QApplication(sys.argv)
    app.setOrganizationName(config.ORG_NAME)
    app.setApplicationName(config.APP_NAME)
    apply_dark_theme(app)

    win = MainWindow()
    win.resize(1600, 950)
    win.show()

    arg = Path(sys.argv[1]) if len(sys.argv) > 1 else None
    if arg and arg.is_file() and arg.suffix.lower() == ".json":
        win.import_legacy_json(arg)
    elif arg and arg.is_file():
        win.restore_session()
        win.open_path(arg)
    else:
        win.restore_session()
    return app.exec()


if __name__ == "__main__":
    sys.exit(main())
