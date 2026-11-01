"""Tree validation — replaces Validate_Passive_Tree.py / validate_node_table.py checks."""
from __future__ import annotations

from collections import Counter
from dataclasses import dataclass

from .catalog import TagCatalog
from .icons import IconLibrary
from .model import EFFECT_FIELDS, NODE_TYPES, TAG_PATTERN, SkillTree, split_tag

ERROR, WARNING, INFO = "Error", "Warning", "Info"
SEVERITY_ORDER = {ERROR: 0, WARNING: 1, INFO: 2}


@dataclass
class Issue:
    severity: str
    uid: int | None
    message: str


def _check_against_live(tree: SkillTree, live: SkillTree, live_version: str, add) -> None:
    """Changes that would break players who allocated points in the live release."""
    current = {n.tag: n for n in tree.nodes.values()}
    live_tags = {n.tag: n for n in live.nodes.values()}
    for tag, old in live_tags.items():
        if tag not in current:
            add(ERROR, None, f"Live node '{tag}' (v{live_version}) was removed or renamed — players "
                             "who allocated it lose that point. Restore the tag or plan a refund.")
    live_edges = {frozenset((live.nodes[a].tag, live.nodes[b].tag)) for a, b in map(tuple, live.edges)}
    cur_edges = {frozenset((tree.nodes[a].tag, tree.nodes[b].tag)) for a, b in map(tuple, tree.edges)}
    for pair in live_edges - cur_edges:
        a, b = sorted(pair)
        if a in current and b in current:
            add(WARNING, current[a].uid, f"Live link {a} ↔ {b} removed — allocated paths through it may "
                                         "become disconnected for existing players.")
    for tag, old in live_tags.items():
        node = current.get(tag)
        if node and old.is_start and not node.is_start:
            add(WARNING, node.uid, f"Was a start node in live v{live_version}; no longer is.")


def validate(tree: SkillTree, catalog: TagCatalog, icons: IconLibrary,
             live: SkillTree | None = None, live_version: str = "",
             other_tags: dict[str, str] | None = None) -> list[Issue]:
    issues: list[Issue] = []
    add = lambda sev, uid, msg: issues.append(Issue(sev, uid, msg))  # noqa: E731

    if not tree.nodes:
        return issues

    tag_counts = Counter(n.tag for n in tree.nodes.values())
    starts = [n.uid for n in tree.nodes.values() if n.is_start]
    if not starts:
        add(ERROR, None, "No start node — nothing can be allocated. Tick 'Start node' on at least one node.")

    # Reachability from start nodes.
    reachable: set[int] = set(starts)
    frontier = list(starts)
    while frontier:
        uid = frontier.pop()
        for other in tree.neighbors(uid):
            if other not in reachable:
                reachable.add(other)
                frontier.append(other)

    unregistered_tags = 0
    for node in tree.nodes.values():
        u = node.uid
        if not node.tag:
            add(ERROR, u, "Empty NodeTag.")
        elif not TAG_PATTERN.match(node.tag):
            add(ERROR, u, f"NodeTag '{node.tag}' is not a valid gameplay tag (letters, digits, _ and dots).")
        else:
            if tag_counts[node.tag] > 1:
                add(ERROR, u, f"Duplicate NodeTag '{node.tag}' — run Tools ▸ Renumber Tags.")
            if other_tags and node.tag in other_tags:
                add(ERROR, u, f"NodeTag '{node.tag}' is also used by tree '{other_tags[node.tag]}' — "
                              "importing one would overwrite the other's UE node asset.")
            prefix, idx = split_tag(node.tag)
            if not idx:
                add(WARNING, u, f"NodeTag '{node.tag}' has no index — run Tools ▸ Renumber Tags.")
            if node.tag not in catalog.registered:
                unregistered_tags += 1

        if node.node_type not in NODE_TYPES:
            add(ERROR, u, f"Unknown NodeType '{node.node_type}'.")
        if not node.name.strip():
            add(WARNING, u, "Missing NodeName.")
        if not node.description.strip():
            add(WARNING, u, "Missing NodeDescription.")
        if not node.icon:
            add(WARNING, u, "No icon set.")
        elif icons.file_for(node.icon) is None:
            add(INFO, u, f"Icon source PNG not found for preview: {node.icon}")

        if not tree.neighbors(u):
            add(WARNING, u, "Not connected to anything.")
        elif u not in reachable and starts:
            add(WARNING, u, "Unreachable from any start node.")

        if not node.effects:
            add(WARNING, u, "Node has no effects.")
        for i, eff in enumerate(node.effects, 1):
            where = f"Effect {i} ({eff.effect_type})"
            fields = EFFECT_FIELDS.get(eff.effect_type)
            if fields is None:
                add(ERROR, u, f"{where}: unknown EffectType.")
                continue
            for key in fields:
                value = eff.values.get(key)
                if value in (None, ""):
                    add(ERROR, u, f"{where}: {key} is empty.")
                elif key == "EffectStatMagnitude":
                    if not isinstance(value, (int, float)):
                        add(ERROR, u, f"{where}: magnitude '{value}' is not a number.")
                elif key == "EffectStatModifierOp":
                    if value not in ("Add", "Multiply"):
                        add(ERROR, u, f"{where}: Op must be Add or Multiply.")
                elif not catalog.known(value):
                    if key == "EffectStatModAttributeType":
                        base, gates = catalog.resolve_gated(value)
                        if base:
                            add(WARNING, u, f"{where}: gated stat '{value}' ({base}, only for skills with "
                                            f"{' + '.join(gates)}) — tag not registered yet: "
                                            "Tools ▸ Register Tags in C++, then rebuild.")
                        elif " " in value:
                            add(WARNING, u, f"{where}: attribute '{value}' contains a space — typo?")
                        else:
                            add(WARNING, u, f"{where}: '{value}' is neither an attribute tag nor a gated stat "
                                            "(<attribute tag>.<SkillTagMod leaf>, e.g. Speed.Skill.Rifle).")
                    elif key == "EffectSkillPassiveTag":
                        add(WARNING, u, f"{where}: SkillPassive tag '{value}' is not registered — "
                                        "Tools ▸ Register Tags in C++ adds it.")
                    else:
                        add(WARNING, u, f"{where}: tag '{value}' is not registered in FP_GameplayTags.cpp "
                                        "(add it, plus the code that reacts to it, in C++ by hand).")

    if live is not None:
        _check_against_live(tree, live, live_version, add)

    if unregistered_tags:
        add(INFO, None, f"{unregistered_tags} NodeTag(s) not yet in FP_GameplayTags.cpp — "
                        "Tools ▸ Register Tags in C++ adds them.")

    issues.sort(key=lambda i: (SEVERITY_ORDER[i.severity], i.uid or 0))
    return issues
