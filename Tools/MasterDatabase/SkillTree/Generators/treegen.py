"""Shared passive-tree generator: base travel path + auto-placed clusters, written through the editor's Project API."""
import math
import sys
from pathlib import Path

sys.path.insert(0, r"C:\Unreal\FP_5_7\Tools\MasterDatabase\SkillTree\SkillTreeEditor")
from skilltree_editor.model import Effect  # noqa: E402
from skilltree_editor.project import Project  # noqa: E402

ICON_ROOT = Path(r"C:\Unreal\FP_5_7\Content\Assets\UIElements\Icons\game-icons_net_png\icons\ffffff\000000\1x1")
ICON_UE = "/Game/Assets/UIElements/Icons/game-icons_net_png/icons/ffffff/000000/1x1"
RADIUS = {"Minor": 22.0, "Major": 36.0, "Mastery": 62.5}
CENTRE = (1500.0, 1500.0)
CANVAS_R = 1500.0


def icon(*names: str) -> str:
    for name in names:
        hits = sorted(ICON_ROOT.glob(f"*/{name}.uasset"))
        if hits:
            return f"{ICON_UE}/{hits[0].parent.name}/{name}.{name}"
    raise SystemExit(f"missing icons {names}")


def pct(v): return f"{abs(v) * 100:g}%"
def inc(a, v, w):   return (a, v, f"{pct(v)} Increased {w}")
def more(a, v, w):  return (a, v, f"{pct(v)} More {w}")
def red(a, v, w):   return (a, v, f"{pct(v)} Reduced {w}")
def flat(a, v, w):  return (a, v, f"+{v:g} {w}")
def plain(a, v, t): return (a, v, t)
def flag(tag, t):   return ("FLAG:" + tag, None, t)
# Multiply op (GAS MultiplyAdditive): magnitude is the raw multiplier, several sum as 1 + sum(m - 1),
# so they behave like the "increased" pool for that attribute. 1.10 = 10% increased, 0.90 = 10% reduced.
def incm(a, v, w):  return ("MUL:" + a, round(1 + v, 4), f"{pct(v)} Increased {w}")
def redm(a, v, w):  return ("MUL:" + a, round(1 - v, 4), f"{pct(v)} Reduced {w}")
def res(k, v):      return plain(f"Resistance.Damage.{k}", v, f"{pct(v)} {k} Resistance")
def maxres(k, v):   return plain(f"Resistance.Max.{k}", v, f"+{pct(v)} Maximum {k} Resistance")
def rep(n, name, effs): return [(name, effs)] * n
def alt(n, a, b):   return [a if i % 2 == 0 else b for i in range(n)]


def make_effects(effs):
    out = []
    for attr, mag, _ in effs:
        if attr == "FLAG_TEXT":      # description-only line (extra text for a flag above it)
            continue
        if attr.startswith("MUL:"):
            out.append(Effect("Stat Mod", {"EffectStatModAttributeType": attr[4:], "EffectStatModifierOp": "Multiply",
                                           "EffectStatMagnitude": mag}))
        elif attr.startswith("FLAG:"):
            out.append(Effect("Effect Flag", {"EffectFlagType": attr[5:]}))
        else:
            out.append(Effect("Stat Mod", {"EffectStatModAttributeType": attr, "EffectStatModifierOp": "Add",
                                           "EffectStatMagnitude": mag}))
    return out


def desc(effs): return "\n".join(t for _, _, t in effs)


# ---- geometry ---------------------------------------------------------------------------
def seg_dist(p, a, b):
    (px, py), (ax, ay), (bx, by) = p, a, b
    vx, vy = bx - ax, by - ay
    L = vx * vx + vy * vy
    t = 0 if L == 0 else max(0, min(1, ((px - ax) * vx + (py - ay) * vy) / L))
    return math.hypot(px - (ax + t * vx), py - (ay + t * vy))


def cross(a, b, c, d):
    def o(p, q, r): return (q[0] - p[0]) * (r[1] - p[1]) - (q[1] - p[1]) * (r[0] - p[0])
    return (o(a, b, c) * o(a, b, d) < 0) and (o(c, d, a) * o(c, d, b) < 0)


def resample(poly, ss):
    """Points at arc lengths ss along polyline poly."""
    cum = [0.0]
    for i in range(1, len(poly)):
        cum.append(cum[-1] + math.dist(poly[i - 1], poly[i]))
    out, j = [], 0
    for s in ss:
        while j < len(cum) - 2 and cum[j + 1] < s:
            j += 1
        seg = cum[j + 1] - cum[j] or 1
        t = (s - cum[j]) / seg
        (x0, y0), (x1, y1) = poly[j], poly[j + 1]
        out.append((x0 + (x1 - x0) * t, y0 + (y1 - y0) * t))
    return out, cum[-1]


# ---- the layout state ---------------------------------------------------------------------
class Layout:
    def __init__(self):
        self.nodes = {}      # tag -> dict(type, x, y, name, effs, icon, start, group)
        self.edges = []      # (tagA, tagB)
        self.cluster_centres = []
        self.anchor_use = {}
        self.cluster_nodes = set()

    def add(self, tag, ntype, pos, name, effs, ic, start=False):
        assert tag not in self.nodes, tag
        self.nodes[tag] = dict(type=ntype, x=pos[0], y=pos[1], name=name, effs=effs, icon=ic, start=start)

    def link(self, a, b):
        self.edges.append((a, b))

    def pos(self, t):
        n = self.nodes[t]
        return (n["x"], n["y"])

    # ---- validity of a candidate set of new nodes/edges ----
    def fits(self, new, new_edges, gap=14, cluster_gap=40):
        """new: {tag: (type, (x, y))}; new_edges: list of (a, b) where a/b may be existing tags."""
        P = {t: self.pos(t) for t in self.nodes}
        R = {t: RADIUS[self.nodes[t]["type"]] for t in self.nodes}
        for t, (ty, p) in new.items():
            if math.dist(p, CENTRE) + RADIUS[ty] > CANVAS_R - 12:
                return False
            for u, q in P.items():
                need = RADIUS[ty] + R[u] + gap + (cluster_gap if not self.nodes[u]["start"] and u in self.cluster_nodes else 0)
                if abs(p[0] - q[0]) < need and abs(p[1] - q[1]) < need and math.dist(p, q) < need:
                    return False
        allP = dict(P)
        allR = dict(R)
        for t, (ty, p) in new.items():
            allP[t], allR[t] = p, RADIUS[ty]
        # new nodes vs existing edges
        for a, b in self.edges:
            for t, (ty, p) in new.items():
                if seg_dist(p, allP[a], allP[b]) < RADIUS[ty] + 8:
                    return False
        # new edges vs all nodes / edges
        for a, b in new_edges:
            for t in allP:
                if t in (a, b):
                    continue
                if seg_dist(allP[t], allP[a], allP[b]) < allR[t] + 8:
                    return False
            for c, d in self.edges + [e for e in new_edges if e != (a, b)]:
                if {a, b} & {c, d}:
                    continue
                if cross(allP[a], allP[b], allP[c], allP[d]):
                    return False
        return True


# ---- cluster shapes -------------------------------------------------------------------------
def ring_points(n, centre, anchor_pos, cw=True):
    cx, cy = centre
    a0 = math.atan2(anchor_pos[1] - cy, anchor_pos[0] - cx)
    sign = 1 if cw else -1
    if n <= 3:
        r = 62
        step = math.radians(270) / n
        return [(cx + r * math.cos(a0 + sign * step * i), cy + r * math.sin(a0 + sign * step * i)) for i in range(n + 1)]
    r = 82 if n >= 6 else 74
    sweep = math.radians(280)
    pts = [(cx + r * math.cos(a0 + sign * sweep * i / (n - 1)), cy + r * math.sin(a0 + sign * sweep * i / (n - 1)))
           for i in range(n)]
    return pts + [(cx, cy)]


def line_points(n, anchor_pos, angle):
    ux, uy = math.cos(angle), math.sin(angle)
    ss = [110 + 80 * i for i in range(n)] + [110 + 80 * n + 12]
    return [(anchor_pos[0] + ux * s, anchor_pos[1] + uy * s) for s in ss]


def place_cluster(L: Layout, c, anchors_pref, anchors_all, max_per_anchor=2):
    n = len(c["minors"])
    tags = [f"{c['group']}.{i + 1}" for i in range(n + 1)]
    best = None
    for pool in (anchors_pref, anchors_all):
        for a in pool:
            if L.anchor_use.get(a, 0) >= max_per_anchor:
                continue
            ap = L.pos(a)
            for k in range(24):
                ang = math.radians(k * 15)
                for d in ((150, 185, 220) if c["shape"] == "ring" else (0,)):
                    for cw in (True, False):
                        if c["shape"] == "ring":
                            centre = (ap[0] + d * math.cos(ang), ap[1] + d * math.sin(ang))
                            pts = ring_points(n, centre, ap, cw)
                        else:
                            if not cw:
                                continue
                            pts = line_points(n, ap, ang)
                            centre = pts[len(pts) // 2]
                        # spread score: distance to the nearest other cluster / base node
                        near = min([math.dist(centre, q) for q in L.cluster_centres] + [1e9])
                        nearb = min(math.dist(centre, L.pos(t)) for t in L.nodes if t != a)
                        score = min(near, 420) + 0.6 * min(nearb, 300) - 0.05 * math.dist(centre, CENTRE) * 0
                        if best and score <= best[0]:
                            continue
                        new = {t: ("Major" if i == n else "Minor", p) for i, (t, p) in enumerate(zip(tags, pts))}
                        new_edges = [(a, tags[0])] + [(tags[i], tags[i + 1]) for i in range(n)]
                        if L.fits(new, new_edges):
                            best = (score, a, pts, centre)
        if best:
            break
    if not best:
        raise SystemExit(f"could not place {c['group']}")
    _, a, pts, centre = best
    for i, (name, effs) in enumerate(c["minors"]):
        L.add(tags[i], "Minor", pts[i], name, effs, c["icon_minor"])
    L.add(tags[-1], "Major", pts[-1], c["major"][0], c["major"][1], c["icon_major"])
    L.cluster_nodes.update(tags)
    L.link(a, tags[0])
    for i in range(n):
        L.link(tags[i], tags[i + 1])
    L.cluster_centres.append(centre)
    L.anchor_use[a] = L.anchor_use.get(a, 0) + 1
    return a


def place_single(L: Layout, tag, ntype, links_pool, name, effs, ic, need_links=2, pref_pos=None):
    """Free-standing node linked to `need_links` base nodes from links_pool, placed near pref_pos."""
    best = None
    cands = []
    if pref_pos:
        for dx in range(-300, 301, 30):
            for dy in range(-300, 301, 30):
                cands.append((pref_pos[0] + dx, pref_pos[1] + dy))
    for p in cands:
        dists = sorted((math.dist(p, L.pos(t)), t) for t in links_pool)
        links = [t for _, t in dists[:need_links]]
        if any(math.dist(p, L.pos(t)) > 420 for t in links):
            continue
        new = {tag: (ntype, p)}
        new_edges = [(t, tag) for t in links]
        if L.fits(new, new_edges, cluster_gap=30):
            score = -math.dist(p, pref_pos)
            if not best or score > best[0]:
                best = (score, p, links)
    if not best:
        raise SystemExit(f"could not place {tag}")
    _, p, links = best
    L.add(tag, ntype, p, name, effs, ic)
    L.cluster_nodes.add(tag)
    for t in links:
        L.link(t, tag)


def write(L: Layout, sktree: Path, reset=True):
    proj = Project.load(sktree)
    tree = proj.tree
    if reset:
        for uid in list(tree.nodes):
            tree.remove_node(uid)
    by = {}
    for tag, n in L.nodes.items():
        node = tree.add_node(tag="PassiveTree." + tag, node_type=n["type"], is_start=n["start"],
                             x=round(n["x"], 1), y=round(n["y"], 1), name=n["name"], description=desc(n["effs"]),
                             icon=n["icon"], effects=make_effects(n["effs"]))
        by[tag] = node.uid
    for a, b in L.edges:
        assert tree.connect(by[a], by[b]), (a, b)
    # NodeTags must be unique across trees (UE asset names): shift colliding groups past the others.
    from skilltree_editor.model import split_tag
    from skilltree_editor.project import other_project_tags
    reserved = {}
    for t in other_project_tags(sktree.parents[2], sktree):
        prefix, idx = split_tag(t)
        if idx:
            reserved.setdefault(prefix, set()).add(idx)
    for old, new in tree.renumber(reserved):
        print(f"  renumbered {old} -> {new}")
    m = proj.meta
    m.canvas_width = m.canvas_height = 3000.0
    proj.save(sktree)
    return len(L.nodes), len(L.edges)
