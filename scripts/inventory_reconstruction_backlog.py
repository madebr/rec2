#!/usr/bin/env python3
"""Conservative, reproducible rec2 source/backlog inventory (stdlib only).

This is a source heuristic, not a C preprocessor, a binary function discovery
tool, or a proof of semantic/assembly equivalence. FUNCTION annotations and
bodies without NOT_IMPLEMENTED are evidence, never completion certificates.
Run from any directory; outputs default to ignored build-audit/backlog/.
"""

import argparse
import collections
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess

ACTIVE_ROOTS = (
    "src/brender", "src/carma2/hooks", "src/carma2/newcommon",
    "src/carma2/platform", "src/carma2/s3", "external/dummyqt",
)
ANNOTATION = re.compile(r"^\s*//\s*(FUNCTION|STUB|GLOBAL):\s+(\w+)\s+(0x[0-9a-fA-F]+)(.*)$", re.M)
IDENTIFIER_COMMENT = re.compile(r"^\s*//\s*([A-Za-z_]\w*)\s*$", re.M)
HOOK_FUNCTION = re.compile(r"\bC2_HOOK_FUNCTION(?:_ORIGINAL)?\s*\(\s*(0x[0-9a-fA-F]+)\s*,\s*([A-Za-z_]\w*)")
HOOK_GLOBAL = re.compile(r"\bC2_HOOK_VARIABLE_IMPLEMENT(?:_ARRAY)?(?:_INIT)?\s*\(\s*[^,]+,\s*([A-Za-z_]\w*)\s*,\s*(?:\d+\s*,\s*)?(0x[0-9a-fA-F]+)")
FUNCTION_HEAD = re.compile(r"^[ \t]*([^;{}\n]*?\b([A-Za-z_]\w*)[ \t]*\([^;{}]*\))\s*\{", re.M)
CALL = re.compile(r"\b([A-Za-z_]\w*)\s*\(")
KEYWORDS = {"if", "for", "while", "switch", "sizeof", "return", "do", "else", "defined"}


def git(root, *args):
    return subprocess.check_output(["git", "-C", str(root), *args], text=True, encoding="utf-8", errors="replace")


def mask_c(text):
    """Preserve all offsets/newlines while hiding strings and comments."""
    pattern = r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\[\s\S]|[^"\\])*"|\'(?:\\[\s\S]|[^\'\\])*\''
    return re.sub(pattern, lambda m: re.sub(r"[^\n]", " ", m[0]), text)


def line_at(text, offset):
    return text.count("\n", 0, offset) + 1


def zero_regions(text):
    """Only recognize literal #if 0; other build conditions stay explicit."""
    stack, spans, start = [], [], None
    for m in re.finditer(r"^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b([^\n]*)", text, re.M):
        kind, expr = m[1], re.split(r"//|/\*", m[2], maxsplit=1)[0].strip()
        was = any(stack)
        if kind in {"if", "ifdef", "ifndef"}:
            stack.append(kind == "if" and expr == "0")
        elif kind in {"else", "elif"} and stack:
            # For literal #if 0, the else arm is available. Unknown branches
            # are still unknown, not declared inactive by this heuristic.
            stack[-1] = False
        elif kind == "endif" and stack:
            stack.pop()
        now = any(stack)
        if now and not was:
            start = m.end()
        elif was and not now:
            spans.append((start, m.start()))
    if start is not None and any(stack):
        spans.append((start, len(text)))
    return spans


def parse_source(path, text, ref):
    clean = mask_c(text)
    zeros = zero_regions(clean)
    disabled_clean = clean
    for start, end in reversed(zeros):
        clean = clean[:start] + re.sub(r"[^\n]", " ", clean[start:end]) + clean[end:]
    annotations = []
    for m in ANNOTATION.finditer(text):
        next_line = text[m.end():].lstrip().split("\n", 1)[0].strip()
        comment_name = re.fullmatch(r"//\s*([A-Za-z_]\w*)", next_line)
        annotations.append({
            "kind": m[1], "target": m[2], "address": f"0x{int(m[3], 16):08x}",
            "line": line_at(text, m.start()), "offset": m.start(),
            "name": comment_name[1] if comment_name else None,
            "literal_if0": any(a <= m.start() < b for a, b in zeros),
        })
    for pattern, kind in [(HOOK_FUNCTION, "FUNCTION"), (HOOK_GLOBAL, "GLOBAL")]:
        for m in pattern.finditer(disabled_clean):
            address, name = (m[1], m[2]) if kind == "FUNCTION" else (m[2], m[1])
            annotations.append({"kind": kind, "target": "CARMA2_HW", "address": f"0x{int(address, 16):08x}",
                                "line": line_at(text, m.start()), "offset": m.start(), "name": name,
                                "literal_if0": any(a <= m.start() < b for a, b in zeros), "evidence": "hook_macro"})
    annotations.sort(key=lambda a: a["offset"])
    functions = []
    ranges = []
    # Definitions are only accepted at lexical brace depth zero. This avoids
    # mistaking if/for expressions or calls in an existing function for defs.
    cursor, depth = 0, 0
    for m in FUNCTION_HEAD.finditer(clean):
        for char in clean[cursor:m.start()]:
            depth += (char == "{") - (char == "}")
        cursor = m.start()
        if depth or m[2] in KEYWORDS or m[1].lstrip().startswith("#"):
            continue
        open_brace = m.end() - 1
        end, nesting = open_brace + 1, 1
        while end < len(clean) and nesting:
            nesting += (clean[end] == "{") - (clean[end] == "}")
            end += 1
        if nesting:
            continue
        body = clean[open_brace + 1:end - 1]
        original_body = text[open_brace + 1:end - 1]
        statements = re.sub(r"^\s*#[^\n]*", "", body, flags=re.M).strip()
        preceding = [a for a in annotations if a["offset"] < m.start()]
        annotation = preceding[-1] if preceding else None
        if annotation:
            between = clean[annotation["offset"]:m.start()]
            if between.strip() or annotation["kind"] == "GLOBAL":
                annotation = None
        calls = sorted(set(CALL.findall(body)) - KEYWORDS)
        has_marker = "NOT_IMPLEMENTED" in calls
        record = {
            "ref": ref, "path": path, "line": line_at(text, m.start()),
            "name": m[2], "signature": " ".join(text[m.start():open_brace].strip().split()),
            "annotation": {k: v for k, v in annotation.items() if k != "offset"} if annotation else None,
            "literal_if0": any(a <= m.start() < b for a, b in zeros),
            "body_lines": original_body.count("\n") + 1,
            "body_sha256": hashlib.sha256(body.encode()).hexdigest(),
            "calls": calls, "identifiers": sorted(set(re.findall(r"\b[A-Za-z_]\w*\b", body))),
            "has_not_implemented": has_marker,
            "has_todo_comment": bool(re.search(r"\b(TODO|FIXME|XXX)\b", original_body)),
            "body_shape": "empty" if not statements else "constant_return" if re.fullmatch(r"return\s+(?:[-+]?\d+(?:\.\d*)?[fFuUlL]*|NULL)\s*;", statements) else "other",
            "calls_original_forwarder": any(n.endswith("_original") for n in calls),
            "classification": "body_with_not_implemented" if has_marker else "body_without_not_implemented",
        }
        if annotation:
            annotation["name"] = m[2]
            if annotation["kind"] == "STUB":
                record["classification"] = "annotated_stub"
        functions.append(record)
        ranges.append((m.start(), end))
    # Hook annotations usually follow the definition, so join them by name.
    for f in functions:
        if not f["annotation"]:
            hook = next((a for a in annotations if a.get("evidence") == "hook_macro" and a["kind"] == "FUNCTION" and a["name"] == f["name"]), None)
            if hook:
                f["annotation"] = {k: v for k, v in hook.items() if k != "offset"}
    identifiers = []
    if path.endswith(".c"):
        for m in IDENTIFIER_COMMENT.finditer(text):
            if any(a <= m.start() < b for a, b in ranges):
                continue
            # Every identifier stays a candidate. The later name join can
            # supply evidence it denotes a function, or leave it unresolved.
            identifiers.append({"ref": ref, "path": path, "line": line_at(text, m.start()),
                                "name": m[1], "literal_if0": any(a <= m.start() < b for a, b in zeros)})
    return functions, identifiers, [{k: v for k, v in a.items() if k != "offset"} for a in annotations]


def sources(root, ref, roots):
    files = git(root, "ls-tree", "-r", "--name-only", ref, "--", *roots).splitlines()
    files = [path for path in files if path.endswith((".c", ".h"))]
    blobs = subprocess.check_output(["git", "-C", str(root), "cat-file", "--batch"],
                                    input="".join(f"{ref}:{path}\n" for path in files).encode())
    cursor = 0
    for path in files:
        header_end = blobs.index(b"\n", cursor)
        size = int(blobs[cursor:header_end].split()[-1])
        cursor = header_end + 1
        text = blobs[cursor:cursor + size].decode("utf-8", errors="replace")
        cursor += size + 1
        yield path, text


def inventory(root, ref, roots):
    functions, comments, annotations = [], [], []
    source_hash = hashlib.sha256()
    actual_ref = "HEAD" if ref == "WORKTREE" else ref
    for path, text in sources(root, actual_ref, roots):
        if ref == "WORKTREE":
            text = (root / path).read_text(encoding="utf-8", errors="replace")
        source_hash.update(path.encode() + b"\0" + text.encode() + b"\0")
        f, c, a = parse_source(path, text, ref)
        functions.extend(f)
        comments.extend(c)
        for record in a:
            record.update(path=path, ref=ref)
        annotations.extend(a)
    return {"functions": functions, "identifier_comments": comments, "annotations": annotations,
            "source_sha256": source_hash.hexdigest()}


def count_inventory(data):
    return {"definitions": len(data["functions"]),
            "source_sha256": data["source_sha256"],
            "definition_classifications": dict(collections.Counter(f["classification"] for f in data["functions"])),
            "identifier_comments": len(data["identifier_comments"]),
            "annotations_by_kind": dict(collections.Counter(a["kind"] for a in data["annotations"])),
            "literal_if0_definitions": sum(f["literal_if0"] for f in data["functions"])}


def validate_parser():
    sample = '''// FUNCTION: CARMA2_HW 0x00401000
int C2_HOOK_FASTCALL MultiLine(
    int pA,
    int pB)
{
    // CommentOnlyIdentifier
    const char *s = "{ NOT_IMPLEMENTED(); }";
    /* NOT_IMPLEMENTED(); fake braces {{{ */
    if (pA) { return pB; }
    return 2;
}
// CandidateIdentifier
// STUB: CARMA2_HW 0x00402000
void C2_HOOK_FASTCALL RealStub(void) {
    NOT_IMPLEMENTED();
}
#if 0 // disabled declaration
// FUNCTION: CARMA2_HW 0x00403000
void C2_HOOK_FASTCALL DisabledDefinition(void) { NOT_IMPLEMENTED(); }
#else
void C2_HOOK_FASTCALL ConditionalDefinition(void) {
#if 0
    NOT_IMPLEMENTED();
#else
    RealStub();
#endif
}
#endif
int C2_HOOK_FASTCALL Prototype(int pValue);
void C2_HOOK_FASTCALL MacroAddress(void) { }
C2_HOOK_FUNCTION(0x00404000, MacroAddress);
/*
#if 0
*/
void C2_HOOK_FASTCALL AfterDirectiveComment(void) { return; }
#define FakeFunctionMacro(x) { return x; }
'''
    functions, comments, annotations = parse_source("validation.c", sample, "TEST")
    by_name = {f["name"]: f for f in functions}
    assert set(by_name) == {"MultiLine", "RealStub", "ConditionalDefinition", "MacroAddress", "AfterDirectiveComment"}, by_name.keys()
    assert not by_name["MultiLine"]["has_not_implemented"]
    assert by_name["MultiLine"]["annotation"]["address"] == "0x00401000"
    assert by_name["RealStub"]["classification"] == "annotated_stub"
    assert by_name["ConditionalDefinition"]["calls"] == ["RealStub"]
    assert by_name["MacroAddress"]["annotation"]["address"] == "0x00404000"
    assert by_name["MacroAddress"]["body_shape"] == "empty"
    assert [c["name"] for c in comments] == ["CandidateIdentifier"]
    assert any(a["literal_if0"] for a in annotations if a["address"] == "0x00403000")
    return "passed: multiline parameters, prototypes, literal #if 0/else, nested branches, comments, strings, hook macros"


def executable_sections(path):
    """Read PE32 section boundaries directly; no external parser required."""
    data = path.read_bytes()
    if data[:2] != b"MZ":
        raise ValueError(f"Not an MZ executable: {path}")
    pe = struct.unpack_from("<I", data, 0x3c)[0]
    if data[pe:pe + 4] != b"PE\0\0":
        raise ValueError(f"Not a PE executable: {path}")
    count = struct.unpack_from("<H", data, pe + 6)[0]
    optional_size = struct.unpack_from("<H", data, pe + 20)[0]
    optional = pe + 24
    if struct.unpack_from("<H", data, optional)[0] != 0x10b:
        raise ValueError("This script currently supports PE32 only")
    image_base = struct.unpack_from("<I", data, optional + 28)[0]
    sections = []
    for index in range(count):
        offset = optional + optional_size + index * 40
        name = data[offset:offset + 8].split(b"\0", 1)[0].decode("ascii", errors="replace")
        virtual_size, rva, raw_size = struct.unpack_from("<III", data, offset + 8)
        characteristics = struct.unpack_from("<I", data, offset + 36)[0]
        sections.append({"name": name, "start": image_base + rva,
                         "end_exclusive": image_base + rva + max(virtual_size, raw_size),
                         "executable": bool(characteristics & 0x20000000)})
    return sections


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--output", type=Path, default=Path("build-audit/backlog"))
    parser.add_argument("--report", type=Path, default=Path("build-audit/CARMA2_HW.published.json"))
    parser.add_argument("--binary", type=Path, default=Path("build-reference/originals/CARMA2_HW.EXE"))
    parser.add_argument("--original-catalog", type=Path, default=Path("build-audit/original-functions/functions.jsonl"),
                        help="Optional Ghidra function JSONL, discovery evidence, not a perfect symbol catalog")
    parser.add_argument("--original-total", type=int, default=5162,
                        help="Externally supplied estimate, NOT measured by this script")
    parser.add_argument("--branches", nargs="*", default=["origin/work", "origin/work-fixed", "origin/work-ped-work"])
    parser.add_argument("--self-test", action="store_true", help="Validate parser fixtures and exit")
    args = parser.parse_args()
    parser_validation = validate_parser()
    if args.self_test:
        print(parser_validation)
        return
    root = args.root.resolve()
    output = args.output if args.output.is_absolute() else root / args.output
    output.mkdir(parents=True, exist_ok=True)
    report_path = args.report if args.report.is_absolute() else root / args.report
    report = json.loads(report_path.read_text()) if report_path.exists() else None
    binary_path = args.binary if args.binary.is_absolute() else root / args.binary
    sections = executable_sections(binary_path) if binary_path.exists() else []
    catalog_path = args.original_catalog if args.original_catalog.is_absolute() else root / args.original_catalog
    original_catalog = [json.loads(line) for line in catalog_path.read_text().splitlines() if line.strip()] if catalog_path.exists() else []
    report_rows = report["data"] if report else []
    report_by_name = collections.defaultdict(list)
    for row in report_rows:
        report_by_name[row["name"]].append(row)
    active = inventory(root, "WORKTREE", ACTIVE_ROOTS)
    old = inventory(root, "WORKTREE", ("src/carma2/common",))
    branch_data = {}
    branch_errors = {}
    for ref in args.branches:
        try:
            branch_data[ref] = inventory(root, ref, ("src/carma2/common", "src/carma2/newcommon"))
        except subprocess.CalledProcessError as exc:
            branch_errors[ref] = str(exc)
    active_definitions = {f["name"]: f for f in active["functions"] if not f["literal_if0"]}
    available = {n for n, f in active_definitions.items() if f["classification"] == "body_without_not_implemented"}
    common_globals = {a["name"] for a in old["annotations"] if a["kind"] == "GLOBAL" and a["name"]}
    active_globals = {a["name"] for a in active["annotations"] if a["kind"] == "GLOBAL" and a["name"]}
    # Global declarations usually follow annotations; get their identifiers
    # separately from function identifiers to estimate migration coupling.
    for path, source in sources(root, "HEAD", ("src/carma2/common",)):
        for m in re.finditer(r"//\s*GLOBAL:[^\n]*\n\s*[^;\n]*?\b([A-Za-z_]\w*)\s*(?:\[[^;]*?\])?\s*(?:=|;)", source):
            common_globals.add(m[1])
    for path, source in sources(root, "HEAD", ACTIVE_ROOTS):
        source = (root / path).read_text(encoding="utf-8", errors="replace")
        for m in re.finditer(r"//\s*GLOBAL:[^\n]*\n\s*[^;\n]*?\b([A-Za-z_]\w*)\s*(?:\[[^;]*?\])?\s*(?:=|;)", source):
            active_globals.add(m[1])
    callable_index = collections.defaultdict(list)
    address_index = collections.defaultdict(set)
    address_evidence = collections.defaultdict(list)
    for data in [active, old, *branch_data.values()]:
        for f in data["functions"]:
            callable_index[f["name"]].append(f)
        for a in data["annotations"]:
            if a["name"] and a["kind"] in {"FUNCTION", "STUB"} and a["target"] == "CARMA2_HW":
                address_index[a["name"]].add(a["address"])
                address_evidence[a["name"]].append({k: a[k] for k in ("address", "kind", "path", "ref", "line", "literal_if0")})
    for row in report_rows:
        address_index[row["name"]].add(f"0x{int(row['address'], 16):08x}")
        address_evidence[row["name"]].append({"address": f"0x{int(row['address'], 16):08x}", "kind": "published_report"})
    report_min = min((int(row["address"], 16) for row in report_rows), default=0)
    report_max = max((int(row["address"], 16) for row in report_rows), default=0)
    comments = [c for c in active["identifier_comments"] if c["path"].startswith("src/carma2/newcommon/")]
    candidates = collections.defaultdict(list)
    for c in comments:
        candidates[c["name"]].append(c)
    for f in active["functions"]:
        if f["path"].startswith("src/carma2/newcommon/") and f["classification"] != "body_without_not_implemented":
            candidates[f["name"]].append({"path": f["path"], "line": f["line"], "name": f["name"], "kind": f["classification"]})
    mappings = []
    for name, locations in sorted(candidates.items()):
        matches = callable_index[name]
        alternatives = [f for f in matches if f["ref"] != "WORKTREE" or f["path"].startswith("src/carma2/common/")]
        usable = [f for f in alternatives if f["classification"] == "body_without_not_implemented" and not f["literal_if0"]]
        for f in usable:
            f["calls_not_in_active_body_names"] = sorted(set(f["calls"]) - available - {name})
            f["referenced_common_global_candidates"] = sorted(set(f["identifiers"]) & common_globals)
            f["common_global_candidates_missing_active_annotation"] = sorted(set(f["identifiers"]) & common_globals - active_globals)
        addresses = sorted(address_index[name])
        mappings.append({
            "name": name, "active_locations": locations,
            "identity_evidence": "function_definition_or_report" if matches or report_by_name[name] else "identifier_comment_only",
            "original_addresses": addresses, "address_conflict": len(addresses) > 1,
            "address_evidence": address_evidence[name],
            "addresses_outside_published_function_span": [a for a in addresses if report_rows and not report_min <= int(a, 16) <= report_max],
            "addresses_outside_executable_sections": [a for a in addresses if sections and not any(s["executable"] and s["start"] <= int(a, 16) < s["end_exclusive"] for s in sections)],
            "published_report_rows": report_by_name[name],
            "active_definition": active_definitions.get(name),
            "alternatives": alternatives, "migration_body_candidates": usable,
        })
    ready = sorted([m for m in mappings if m["migration_body_candidates"]], key=lambda m: (
        not bool(m["original_addresses"]),
        bool(m["addresses_outside_executable_sections"]),
        bool(m["addresses_outside_published_function_span"]),
        all(f["body_shape"] in {"empty", "constant_return"} for f in m["migration_body_candidates"]),
        min(len(f["calls_not_in_active_body_names"]) for f in m["migration_body_candidates"]),
        min(len(f["common_global_candidates_missing_active_annotation"]) for f in m["migration_body_candidates"]),
        min(f["body_lines"] for f in m["migration_body_candidates"]), m["name"]))
    summary = {
        "schema_version": 1, "head": git(root, "rev-parse", "HEAD").strip(),
        "working_tree_status": git(root, "status", "--short").splitlines(),
        "active_scope": list(ACTIVE_ROOTS), "common_is_excluded_from_default_build": True,
        "scope_warning": "Source presence is not link reachability or build-conditional availability. Bodies are not fidelity certificates.",
        "parser_warning": "Function-definition heuristic accepts multiline parameters; comments/strings masked; literal #if 0 recognized, other preprocessing not evaluated. Headers can contain macro definitions resembling functions.",
        "parser_validation": parser_validation,
        "active": count_inventory(active), "inactive_common": count_inventory(old),
        "branches": {ref: {"commit": git(root, "rev-parse", ref).strip(), **count_inventory(data)} for ref, data in branch_data.items()},
        "branch_errors": branch_errors,
        "newcommon_candidate_names": len(mappings),
        "newcommon_identifier_comment_occurrences": len(comments),
        "names_without_function_identity_evidence": [m["name"] for m in mappings if m["identity_evidence"] == "identifier_comment_only"],
        "names_with_original_address_evidence": sum(bool(m["original_addresses"]) for m in mappings),
        "names_with_migration_body_candidates": len(ready),
        "named_function_address_catalog_size": len(address_index),
        "distinct_annotated_function_addresses": len({a for addresses in address_index.values() for a in addresses}),
        "address_span_warning": "Outside published function min/max is a warning requiring binary section validation, not proof of invalidity.",
        "executable_sections": sections,
        "binary_sha256": hashlib.sha256(binary_path.read_bytes()).hexdigest() if sections else None,
        "names_with_addresses_outside_executable_sections": [m["name"] for m in mappings if m["addresses_outside_executable_sections"]],
        "original_catalog": {"path": str(catalog_path), "discovered_entries": len(original_catalog),
                             "sha256": hashlib.sha256(catalog_path.read_bytes()).hexdigest() if original_catalog else None,
                             "warning": "Ghidra discovery may split/merge/miss functions; rows are not authoritative original source identities."},
        "published_report": {
            "path": str(report_path), "sha256": hashlib.sha256(report_path.read_bytes()).hexdigest() if report else None,
            "timestamp": report.get("timestamp") if report else None, "identified_rows": len(report_rows),
            "nonstub_rows": sum(not r.get("stub", False) for r in report_rows),
            "stub_rows": sum(bool(r.get("stub")) for r in report_rows),
            "externally_supplied_original_total": args.original_total,
            "functions_absent_from_report_estimate": args.original_total - len(report_rows),
            "nonstub_does_not_mean_perfect_assembly_match": True,
        },
        "migration_order_names": [m["name"] for m in ready],
    }
    for filename, data in [("summary.json", summary), ("function_mapping.json", mappings),
                           ("active_source.json", active), ("inactive_common_source.json", old),
                           ("branch_sources.json", branch_data), ("migration_candidates.json", ready)]:
        (output / filename).write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")
    (output / "address_catalog.json").write_text(json.dumps(address_evidence, indent=2) + "\n", encoding="utf-8")
    report_by_address = {int(r["address"], 16): r for r in report_rows}
    names_by_address = collections.defaultdict(list)
    for name, addresses in address_index.items():
        for address in addresses:
            names_by_address[int(address, 16)].append(name)
    original_join = [{**row, "source_names": sorted(names_by_address[int(row["address"], 16)]),
                      "published_report_row": report_by_address.get(int(row["address"], 16))} for row in original_catalog]
    (output / "original_catalog_join.json").write_text(json.dumps(original_join, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))


if __name__ == "__main__":
    main()
