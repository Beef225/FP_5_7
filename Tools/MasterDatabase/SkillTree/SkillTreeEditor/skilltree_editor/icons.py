"""Maps UE icon asset paths to the source PNGs under Content/ so nodes can show real icons.

UE turns dots in folder/asset names into underscores on import, e.g.
  Content/Assets/UIElements/Icons/game-icons.net.png/.../lorc/tesla-coil.png
  -> /Game/Assets/UIElements/Icons/game-icons_net_png/.../lorc/tesla-coil.tesla-coil
"""
from __future__ import annotations

from pathlib import Path

from PySide6.QtCore import Qt
from PySide6.QtGui import QPixmap

from . import config
from .model import normalize_ue_reference


def ue_path_for_file(file: Path, content_dir: Path = config.CONTENT_DIR) -> str:
    rel = file.resolve().relative_to(content_dir.resolve())
    dirs = [part.replace(".", "_") for part in rel.parent.parts]
    stem = rel.stem.replace(".", "_")
    return "/Game/" + "/".join(dirs + [stem]) + "." + stem


class IconLibrary:
    def __init__(self, library_dir: Path = config.ICON_LIBRARY_DIR,
                 content_dir: Path = config.CONTENT_DIR) -> None:
        self.library_dir = library_dir
        self.content_dir = content_dir
        self._index: dict[str, Path] | None = None
        self._pixmaps: dict[tuple[str, int], QPixmap | None] = {}

    def _ensure_index(self) -> dict[str, Path]:
        if self._index is None:
            self._index = {}
            if self.library_dir.is_dir():
                for png in sorted(self.library_dir.rglob("*.png")):
                    try:
                        self._index[ue_path_for_file(png, self.content_dir)] = png
                    except ValueError:
                        pass
        return self._index

    def entries(self) -> list[tuple[str, Path]]:
        """(ue_path, png) for every icon in the library, sorted by file name."""
        return sorted(self._ensure_index().items(), key=lambda kv: kv[1].stem)

    def file_for(self, ue_path: str) -> Path | None:
        ue_path = normalize_ue_reference(ue_path)
        if not ue_path:
            return None
        hit = self._ensure_index().get(ue_path)
        if hit:
            return hit
        if ue_path.startswith("/Game/"):
            rel = ue_path[len("/Game/"):].rsplit(".", 1)[0]
            guess = self.content_dir / (rel + ".png")
            if guess.is_file():
                return guess
        return None

    def pixmap(self, ue_path: str, size: int = 128) -> QPixmap | None:
        key = (ue_path, size)
        if key not in self._pixmaps:
            file = self.file_for(ue_path)
            pm = None
            if file:
                raw = QPixmap(str(file))
                if not raw.isNull():
                    pm = raw.scaled(size, size, Qt.AspectRatioMode.KeepAspectRatio,
                                    Qt.TransformationMode.SmoothTransformation)
            self._pixmaps[key] = pm
        return self._pixmaps[key]
