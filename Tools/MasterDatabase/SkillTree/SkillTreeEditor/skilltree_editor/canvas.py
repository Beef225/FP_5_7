"""Node-graph canvas: QGraphicsScene holding NodeItems/EdgeItems, and a pan/zoom view."""
from __future__ import annotations

import math
import zlib

from PySide6.QtCore import QLineF, QPointF, QRectF, Qt, Signal
from PySide6.QtGui import (QBrush, QColor, QFont, QPainter, QPainterPath, QPainterPathStroker,
                           QPen)
from PySide6.QtWidgets import (QGraphicsItem, QGraphicsLineItem, QGraphicsScene, QGraphicsView,
                               QStyleOptionGraphicsItem)

from . import config
from .icons import IconLibrary
from .model import Node, SkillTree

LABEL_W, LABEL_H = 180.0, 40.0

BG = QColor("#1b1d22")
CANVAS_BG = QColor("#1f2228")
GRID_MINOR_C = QColor("#2a2e36")
GRID_MAJOR_C = QColor("#3a404c")
CANVAS_C = QColor("#3b4150")
RING_C = QColor("#4b5466")
EDGE_RING_C = QColor("#5d687d")
EDGE_C = QColor("#6b7280")
EDGE_HI = QColor("#cbd5e1")
SELECT_C = QColor("#22d3ee")
START_C = QColor("#facc15")
ISSUE_C = QColor("#ef4444")
HIGHLIGHT_C = QColor("#f59e0b")   # Stat Report matches
LABEL_C = QColor("#d1d5db")


def group_color(key: str) -> QColor:
    hue = zlib.crc32(key.encode()) % 360
    return QColor.fromHsv(hue, 150, 210)


class NodeItem(QGraphicsItem):
    def __init__(self, node: Node, scene_ref: "TreeScene") -> None:
        super().__init__()
        self.node = node
        self.tree_scene = scene_ref
        self.edges: list[EdgeItem] = []
        self.setFlags(QGraphicsItem.GraphicsItemFlag.ItemIsMovable
                      | QGraphicsItem.GraphicsItemFlag.ItemIsSelectable
                      | QGraphicsItem.GraphicsItemFlag.ItemSendsGeometryChanges)
        self.setAcceptHoverEvents(True)
        self.setZValue(10)
        self.setPos(node.x, node.y)
        self.refresh()

    @property
    def radius(self) -> float:
        return config.NODE_RADIUS.get(self.node.node_type, config.NODE_RADIUS["Minor"])

    def refresh(self) -> None:
        self.prepareGeometryChange()
        n = self.node
        lines = [f"<b>{n.name or '(unnamed)'}</b>", f"<span style='color:#9ca3af'>{n.tag}</span>"]
        if n.description:
            lines.append(n.description.replace("\n", "<br>"))
        for e in n.effects:
            lines.append(f"• {e.summary()}")
        self.setToolTip("<br>".join(lines))
        if self.pos() != QPointF(n.x, n.y):
            self.setPos(n.x, n.y)
        self.update()

    def boundingRect(self) -> QRectF:
        r = self.radius + 24              # room for start / selection rings + report halo
        half_w = max(r, LABEL_W / 2)
        return QRectF(-half_w, -r, half_w * 2, r * 2 + LABEL_H)

    def shape(self) -> QPainterPath:
        path = QPainterPath()
        r = self.radius + 4
        path.addEllipse(QPointF(0, 0), r, r)
        return path

    def paint(self, painter: QPainter, option: QStyleOptionGraphicsItem, widget=None) -> None:
        lod = QStyleOptionGraphicsItem.levelOfDetailFromTransform(painter.worldTransform())
        r = self.radius
        n = self.node
        ts = self.tree_scene
        base = group_color(n.group)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)
        highlighted = n.uid in ts.highlight_uids
        if ts.highlight_uids and not highlighted and not self.isSelected():
            painter.setOpacity(0.3)       # a Stat Report is active: dim non-matching nodes

        painter.setPen(Qt.PenStyle.NoPen)
        painter.setBrush(base.darker(260))
        painter.drawEllipse(QPointF(0, 0), r, r)

        if ts.show_icons and lod * r > 6:
            pm = ts.icons.pixmap(n.icon, 128)
            if pm is not None:
                painter.save()
                clip = QPainterPath()
                clip.addEllipse(QPointF(0, 0), r * 0.9, r * 0.9)
                painter.setClipPath(clip)
                # Icons are white-on-black: Screen keeps the fill and lifts the glyph.
                painter.setCompositionMode(QPainter.CompositionMode.CompositionMode_Screen)
                s = r * 1.45
                painter.drawPixmap(QRectF(-s / 2, -s / 2, s, s), pm, QRectF(pm.rect()))
                painter.restore()

        ring = {"Minor": 3.0, "Major": 5.0, "Mastery": 7.0}.get(n.node_type, 3.0)
        painter.setBrush(Qt.BrushStyle.NoBrush)
        painter.setPen(QPen(base, ring))
        painter.drawEllipse(QPointF(0, 0), r, r)

        outer = r + ring / 2
        if n.is_start:
            painter.setPen(QPen(START_C, 3, Qt.PenStyle.DashLine))
            outer += 4
            painter.drawEllipse(QPointF(0, 0), outer, outer)
        if self.isSelected():
            painter.setPen(QPen(SELECT_C, 3))
            outer += 4
            painter.drawEllipse(QPointF(0, 0), outer, outer)

        if highlighted:
            glow = QColor(HIGHLIGHT_C)
            glow.setAlpha(90)
            painter.setPen(QPen(glow, 10))
            painter.drawEllipse(QPointF(0, 0), outer + 7, outer + 7)
            painter.setPen(QPen(HIGHLIGHT_C, 3))
            painter.drawEllipse(QPointF(0, 0), outer + 7, outer + 7)

        if n.uid in ts.issue_uids:
            painter.setPen(Qt.PenStyle.NoPen)
            painter.setBrush(ISSUE_C)
            d = r * 0.707
            painter.drawEllipse(QPointF(d, -d), 5, 5)

        if ts.show_labels and lod > 0.45 and n.name:
            painter.setPen(LABEL_C)
            font = QFont()
            font.setPixelSize(11)
            painter.setFont(font)
            painter.drawText(QRectF(-LABEL_W / 2, r + 6, LABEL_W, LABEL_H - 6),
                             Qt.AlignmentFlag.AlignHCenter | Qt.AlignmentFlag.AlignTop
                             | Qt.TextFlag.TextWordWrap, n.name)

    def itemChange(self, change, value):
        if change == QGraphicsItem.GraphicsItemChange.ItemPositionChange and self.tree_scene.snap_enabled:
            s = self.tree_scene.grid_minor or config.GRID_MINOR
            return QPointF(round(value.x() / s) * s, round(value.y() / s) * s)
        if change == QGraphicsItem.GraphicsItemChange.ItemPositionHasChanged:
            self.node.x, self.node.y = self.pos().x(), self.pos().y()
            for edge in self.edges:
                edge.update_geometry()
        if change == QGraphicsItem.GraphicsItemChange.ItemSelectedHasChanged:
            for edge in self.edges:
                edge.update()
        return super().itemChange(change, value)


class EdgeItem(QGraphicsLineItem):
    def __init__(self, a: NodeItem, b: NodeItem) -> None:
        super().__init__()
        self.a, self.b = a, b
        a.edges.append(self)
        b.edges.append(self)
        self.setFlag(QGraphicsItem.GraphicsItemFlag.ItemIsSelectable)
        self.setZValue(0)
        self.update_geometry()

    @property
    def key(self) -> frozenset[int]:
        return frozenset((self.a.node.uid, self.b.node.uid))

    def detach(self) -> None:
        for end in (self.a, self.b):
            if self in end.edges:
                end.edges.remove(self)

    def update_geometry(self) -> None:
        self.setLine(QLineF(self.a.pos(), self.b.pos()))

    def shape(self) -> QPainterPath:
        path = QPainterPath(self.line().p1())
        path.lineTo(self.line().p2())
        stroker = QPainterPathStroker()
        stroker.setWidth(14)
        return stroker.createStroke(path)

    def boundingRect(self) -> QRectF:
        return super().boundingRect().adjusted(-8, -8, 8, 8)

    def paint(self, painter: QPainter, option, widget=None) -> None:
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)
        if self.isSelected():
            pen = QPen(SELECT_C, 5)
        elif self.a.isSelected() or self.b.isSelected():
            pen = QPen(EDGE_HI, 4)
        else:
            pen = QPen(EDGE_C, 4)
        pen.setCapStyle(Qt.PenCapStyle.RoundCap)
        painter.setPen(pen)
        painter.drawLine(self.line())


class TreeScene(QGraphicsScene):
    committed = Signal(str)                    # a user edit finished → push undo state
    add_node_requested = Signal(QPointF)       # double-click on empty canvas

    def __init__(self, icons: IconLibrary) -> None:
        super().__init__()
        self.canvas_w = self.canvas_h = config.CANVAS_SIZE
        self.grid_minor, self.grid_major, self.ring_step = config.GRID_MINOR, config.GRID_MAJOR, config.RING_STEP
        self.set_canvas_size(config.CANVAS_SIZE, config.CANVAS_SIZE)
        self.setItemIndexMethod(QGraphicsScene.ItemIndexMethod.NoIndex)
        self.icons = icons
        self.tree = SkillTree()
        self.node_items: dict[int, NodeItem] = {}
        self.edge_items: dict[frozenset[int], EdgeItem] = {}
        self.issue_uids: set[int] = set()
        self.highlight_uids: set[int] = set()   # Stat Report matches (empty = no dimming)
        self.snap_enabled = False
        self.show_labels = True
        self.show_icons = True
        self.show_grid = True
        self._link_source: NodeItem | None = None
        self._link_line: QGraphicsLineItem | None = None
        self._press_positions: dict[int, tuple[float, float]] = {}

    def set_canvas_size(self, width: float, height: float) -> None:
        """Canvas = the project's NodePosition area (0,0)-(width,height); one canvas of pan margin."""
        self.canvas_w, self.canvas_h = float(width), float(height)
        self.setSceneRect(-self.canvas_w, -self.canvas_h, self.canvas_w * 3, self.canvas_h * 3)
        self.update()

    def apply_layout(self, meta) -> None:
        """Canvas size + grid settings from a ProjectMeta."""
        self.grid_minor, self.grid_major, self.ring_step = meta.grid_minor, meta.grid_major, meta.ring_step
        self.set_canvas_size(meta.canvas_width, meta.canvas_height)

    @property
    def canvas_rect(self) -> QRectF:
        return QRectF(0, 0, self.canvas_w, self.canvas_h)

    # ---- building ----------------------------------------------------------

    def rebuild(self, tree: SkillTree) -> None:
        self.blockSignals(True)
        self.clear()
        self.blockSignals(False)
        self.tree = tree
        self.node_items.clear()
        self.edge_items.clear()
        for node in tree.nodes.values():
            self._add_node_item(node)
        for key in tree.edges:
            self._add_edge_item(key)
        self.selectionChanged.emit()

    def _add_node_item(self, node: Node) -> NodeItem:
        item = NodeItem(node, self)
        self.addItem(item)
        self.node_items[node.uid] = item
        return item

    def _add_edge_item(self, key: frozenset[int]) -> None:
        a, b = tuple(key)
        edge = EdgeItem(self.node_items[a], self.node_items[b])
        self.addItem(edge)
        self.edge_items[key] = edge

    def _remove_edge_item(self, key: frozenset[int]) -> None:
        edge = self.edge_items.pop(key, None)
        if edge:
            edge.detach()
            self.removeItem(edge)

    # ---- queries -----------------------------------------------------------

    def node_at(self, pos: QPointF, exclude: NodeItem | None = None) -> NodeItem | None:
        for item in self.items(pos):
            if isinstance(item, NodeItem) and item is not exclude:
                return item
        return None

    def selected_nodes(self) -> list[Node]:
        return [i.node for i in self.selectedItems() if isinstance(i, NodeItem)]

    def selected_uids(self) -> list[int]:
        return [n.uid for n in self.selected_nodes()]

    def select_uids(self, uids, centre_view: QGraphicsView | None = None) -> None:
        self.blockSignals(True)
        self.clearSelection()
        for uid in uids:
            item = self.node_items.get(uid)
            if item:
                item.setSelected(True)
        self.blockSignals(False)
        self.selectionChanged.emit()
        if centre_view and uids and uids[0] in self.node_items:
            centre_view.centerOn(self.node_items[uids[0]])

    def refresh_nodes(self, uids=None) -> None:
        items = self.node_items.values() if uids is None else (self.node_items[u] for u in uids if u in self.node_items)
        for item in items:
            item.refresh()
            for e in item.edges:
                e.update_geometry()

    def set_issue_uids(self, uids: set[int]) -> None:
        changed = uids ^ self.issue_uids
        self.issue_uids = uids
        for uid in changed:
            if uid in self.node_items:
                self.node_items[uid].update()

    def set_highlight_uids(self, uids: set[int]) -> None:
        uids = set(uids)
        if uids == self.highlight_uids:
            return
        dim_changed = bool(uids) != bool(self.highlight_uids)
        changed = uids ^ self.highlight_uids
        self.highlight_uids = uids
        targets = self.node_items.keys() if dim_changed else changed
        for uid in targets:
            if uid in self.node_items:
                self.node_items[uid].update()

    # ---- edits (model + view together) ----------------------------------------

    def create_node(self, pos: QPointF, template: Node | None = None) -> Node:
        prefix = f"{self.tree.tag_root}.NewNode"
        if template is not None:
            from .model import split_tag
            prefix = split_tag(template.tag)[0] or prefix
        node = self.tree.add_node(tag=self.tree.next_free_tag(prefix), x=pos.x(), y=pos.y())
        if template is not None:
            node.apply_content_json(template.content_json())
        self._add_node_item(node)
        return node

    def delete_selection(self) -> bool:
        changed = False
        for item in list(self.selectedItems()):
            if isinstance(item, EdgeItem) and item.key in self.edge_items:
                a, b = tuple(item.key)
                self.tree.disconnect(a, b)
                self._remove_edge_item(item.key)
                changed = True
        for item in list(self.selectedItems()):
            if isinstance(item, NodeItem):
                uid = item.node.uid
                for e in list(item.edges):
                    self._remove_edge_item(e.key)
                self.tree.remove_node(uid)
                self.node_items.pop(uid, None)
                self.removeItem(item)
                changed = True
        return changed

    def toggle_link(self, a: int, b: int) -> bool:
        if a == b:
            return False
        key = frozenset((a, b))
        if self.tree.is_connected(a, b):
            self.tree.disconnect(a, b)
            self._remove_edge_item(key)
        else:
            self.tree.connect(a, b)
            self._add_edge_item(key)
        return True

    def link_chain(self, uids: list[int]) -> int:
        """Connect the given nodes in sequence (A-B, B-C, ...); returns number of new links."""
        made = 0
        for a, b in zip(uids, uids[1:]):
            if self.tree.connect(a, b):
                self._add_edge_item(frozenset((a, b)))
                made += 1
        return made

    def paste(self, fragment: dict, centre: QPointF) -> list[int]:
        xs = [n["X"] for n in fragment["nodes"]]
        ys = [n["Y"] for n in fragment["nodes"]]
        dx = centre.x() - (min(xs) + max(xs)) / 2
        dy = centre.y() - (min(ys) + max(ys)) / 2
        uids = self.tree.paste_fragment(fragment, dx, dy)
        for uid in uids:
            self._add_node_item(self.tree.nodes[uid])
        for i, j in fragment["edges"]:
            self._add_edge_item(frozenset((uids[i], uids[j])))
        return uids

    def nudge_selection(self, dx: float, dy: float) -> bool:
        items = [i for i in self.selectedItems() if isinstance(i, NodeItem)]
        for item in items:
            item.moveBy(dx, dy)
        return bool(items)

    # ---- mouse -------------------------------------------------------------

    def mousePressEvent(self, event) -> None:
        if (event.button() == Qt.MouseButton.LeftButton
                and event.modifiers() & Qt.KeyboardModifier.ShiftModifier):
            src = self.node_at(event.scenePos())
            if src:
                self._link_source = src
                self._link_line = self.addLine(QLineF(src.pos(), event.scenePos()),
                                               QPen(SELECT_C, 3, Qt.PenStyle.DashLine))
                self._link_line.setZValue(5)
                event.accept()
                return
        self._press_positions = {uid: (it.pos().x(), it.pos().y()) for uid, it in self.node_items.items()}
        super().mousePressEvent(event)

    def mouseMoveEvent(self, event) -> None:
        if self._link_source and self._link_line:
            self._link_line.setLine(QLineF(self._link_source.pos(), event.scenePos()))
            event.accept()
            return
        super().mouseMoveEvent(event)

    def mouseReleaseEvent(self, event) -> None:
        if self._link_source:
            target = self.node_at(event.scenePos(), exclude=self._link_source)
            src = self._link_source
            self.removeItem(self._link_line)
            self._link_source, self._link_line = None, None
            if target:
                linked = not self.tree.is_connected(src.node.uid, target.node.uid)
                self.toggle_link(src.node.uid, target.node.uid)
                self.committed.emit("Link" if linked else "Unlink")
            event.accept()
            return
        super().mouseReleaseEvent(event)
        moved = any(uid in self._press_positions
                    and self._press_positions[uid] != (it.pos().x(), it.pos().y())
                    for uid, it in self.node_items.items())
        self._press_positions = {}
        if moved:
            self.committed.emit("Move")

    def mouseDoubleClickEvent(self, event) -> None:
        if event.button() == Qt.MouseButton.LeftButton and not self.node_at(event.scenePos()):
            self.add_node_requested.emit(event.scenePos())
            event.accept()
            return
        super().mouseDoubleClickEvent(event)

    # ---- background --------------------------------------------------------

    def drawBackground(self, painter: QPainter, rect: QRectF) -> None:
        painter.fillRect(rect, BG)
        canvas = self.canvas_rect
        lod = QStyleOptionGraphicsItem.levelOfDetailFromTransform(painter.worldTransform())
        area = rect.intersected(canvas)          # grid only inside the canvas
        if self.show_grid and not area.isEmpty():
            painter.fillRect(area, CANVAS_BG)
            major = self.grid_major

            def on_major(v: float) -> bool:
                return major > 0 and abs(v - round(v / major) * major) < 1e-6

            # A line set is hidden while its lines would be < 5 screen px apart.
            for step, color, is_major in ((self.grid_minor, GRID_MINOR_C, False),
                                          (major, GRID_MAJOR_C, True)):
                if step <= 0 or step * lod < 5:
                    continue
                painter.setPen(QPen(color, 0))
                x = math.ceil(area.left() / step) * step
                while x <= area.right():
                    if is_major or not on_major(x):
                        painter.drawLine(QPointF(x, area.top()), QPointF(x, area.bottom()))
                    x += step
                y = math.ceil(area.top() / step) * step
                while y <= area.bottom():
                    if is_major or not on_major(y):
                        painter.drawLine(QPointF(area.left(), y), QPointF(area.right(), y))
                    y += step
        cx, cy = canvas.center().x(), canvas.center().y()
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)
        painter.setBrush(Qt.BrushStyle.NoBrush)
        painter.setPen(QPen(CANVAS_C, 0, Qt.PenStyle.DashLine))
        painter.drawRect(canvas)
        half_w, half_h = canvas.width() / 2, canvas.height() / 2
        if self.show_grid and self.ring_step > 0:
            # Guide rings: scaled copies of the edge oval, ring_step apart along the longer
            # half-axis (circles on a square canvas, ovals otherwise).
            ring_pen = QPen(RING_C, 1.5)
            ring_pen.setCosmetic(True)          # constant screen width at any zoom
            painter.setPen(ring_pen)
            longer = max(half_w, half_h)
            r = self.ring_step
            while r < longer - 1:
                t = r / longer
                painter.drawEllipse(QPointF(cx, cy), half_w * t, half_h * t)
                r += self.ring_step
        edge_pen = QPen(EDGE_RING_C, 2)
        edge_pen.setCosmetic(True)
        painter.setPen(edge_pen)
        painter.drawEllipse(QPointF(cx, cy), half_w, half_h)
        painter.drawLine(QPointF(cx - 25, cy), QPointF(cx + 25, cy))
        painter.drawLine(QPointF(cx, cy - 25), QPointF(cx, cy + 25))


class TreeView(QGraphicsView):
    cursor_moved = Signal(QPointF)
    zoom_changed = Signal(float)
    context_menu = Signal(QPointF, object)   # scene pos, global QPoint
    nudge = Signal(float, float)

    def __init__(self, scene: TreeScene) -> None:
        super().__init__(scene)
        self.setRenderHints(QPainter.RenderHint.Antialiasing | QPainter.RenderHint.SmoothPixmapTransform)
        self.setViewportUpdateMode(QGraphicsView.ViewportUpdateMode.FullViewportUpdate)
        self.setTransformationAnchor(QGraphicsView.ViewportAnchor.AnchorUnderMouse)
        self.setDragMode(QGraphicsView.DragMode.RubberBandDrag)
        self.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        self.setVerticalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        # No view background brush: with one set, Qt never calls TreeScene.drawBackground
        # (grid, canvas border, centre ring). The scene fills BG itself.
        self.setMouseTracking(True)
        self._pan_from = None
        self._space_down = False

    @property
    def zoom(self) -> float:
        return self.transform().m11()

    def fit_all(self) -> None:
        items_rect = self.scene().itemsBoundingRect()
        rect = items_rect if not items_rect.isEmpty() else self.scene().canvas_rect
        self.fitInView(rect.adjusted(-80, -80, 80, 80), Qt.AspectRatioMode.KeepAspectRatio)
        self.zoom_changed.emit(self.zoom)

    def wheelEvent(self, event) -> None:
        factor = 1.15 ** (event.angleDelta().y() / 120)
        new_zoom = self.zoom * factor
        if 0.03 < new_zoom < 10:
            self.scale(factor, factor)
            self.zoom_changed.emit(self.zoom)

    def _start_pan(self, event) -> None:
        self._pan_from = event.position()
        self.viewport().setCursor(Qt.CursorShape.ClosedHandCursor)
        event.accept()

    def mousePressEvent(self, event) -> None:
        if event.button() == Qt.MouseButton.MiddleButton or (
                event.button() == Qt.MouseButton.LeftButton and self._space_down):
            self._start_pan(event)
            return
        super().mousePressEvent(event)

    def mouseMoveEvent(self, event) -> None:
        self.cursor_moved.emit(self.mapToScene(event.position().toPoint()))
        if self._pan_from is not None:
            delta = event.position() - self._pan_from
            self._pan_from = event.position()
            self.horizontalScrollBar().setValue(self.horizontalScrollBar().value() - int(delta.x()))
            self.verticalScrollBar().setValue(self.verticalScrollBar().value() - int(delta.y()))
            event.accept()
            return
        super().mouseMoveEvent(event)

    def mouseReleaseEvent(self, event) -> None:
        if self._pan_from is not None and event.button() in (Qt.MouseButton.MiddleButton, Qt.MouseButton.LeftButton):
            self._pan_from = None
            self.viewport().setCursor(Qt.CursorShape.OpenHandCursor if self._space_down else Qt.CursorShape.ArrowCursor)
            event.accept()
            return
        super().mouseReleaseEvent(event)

    def keyPressEvent(self, event) -> None:
        if event.key() == Qt.Key.Key_Space and not event.isAutoRepeat():
            self._space_down = True
            self.viewport().setCursor(Qt.CursorShape.OpenHandCursor)
            return
        arrows = {Qt.Key.Key_Left: (-1, 0), Qt.Key.Key_Right: (1, 0),
                  Qt.Key.Key_Up: (0, -1), Qt.Key.Key_Down: (0, 1)}
        if event.key() in arrows:
            step = 10 if event.modifiers() & Qt.KeyboardModifier.ShiftModifier else 1
            dx, dy = arrows[event.key()]
            self.nudge.emit(dx * step, dy * step)
            return
        super().keyPressEvent(event)

    def keyReleaseEvent(self, event) -> None:
        if event.key() == Qt.Key.Key_Space and not event.isAutoRepeat():
            self._space_down = False
            self.viewport().setCursor(Qt.CursorShape.ArrowCursor)
            return
        super().keyReleaseEvent(event)

    def contextMenuEvent(self, event) -> None:
        self.context_menu.emit(self.mapToScene(event.pos()), event.globalPos())
