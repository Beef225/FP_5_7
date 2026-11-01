import os, sys
os.environ["QT_QPA_PLATFORM"] = "offscreen"
sys.path.insert(0, r"C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\SkillTreeEditor")
from pathlib import Path
from PySide6.QtWidgets import QApplication
from PySide6.QtGui import QImage, QPainter
from PySide6.QtCore import QRectF
app = QApplication([])
from skilltree_editor.mainwindow import MainWindow
src, out = sys.argv[1], sys.argv[2]
w = MainWindow()
assert w.open_path(Path(src), confirm=False, quiet=True)
w.view_opts["show_labels"] = False
w.scene.show_labels = len(sys.argv) > 3
app.processEvents()
img = QImage(1500, 1500, QImage.Format.Format_ARGB32); img.fill(0xff000000)
p = QPainter(img); w.scene.render(p, QRectF(0, 0, 1500, 1500), w.scene.canvas_rect); p.end()
img.save(out)
w._closing = True
