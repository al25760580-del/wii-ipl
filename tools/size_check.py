#!/usr/bin/env python3
"""size_check.py — compare compiled function sizes against the original binary's
symbol table, without needing the original binary.

`config/<VER>/symbols.txt` records the address *and size* of every function in
the original `main.dol`. That information lives in the repo, so even when
`orig/<VER>/*.app` is unavailable (no WAD ⇒ no dtk split ⇒ no objdiff) a
newly written translation unit can still be graded:

  * compile the TU with the real CodeWarrior compiler,
  * read the sizes of the functions in the resulting object file,
  * diff them against the sizes recorded in `symbols.txt`.

A size mismatch is *proof* the function does not match. Equal sizes are a
necessary (not sufficient) condition for a match, and in practice a very good
signal for triaging which functions to attack first once objdiff is available.

Usage:
    python tools/size_check.py libs/RevoEX/src/net/md5.c
    python tools/size_check.py --preset net --preset fa
    python tools/size_check.py --preset fa --summary

Exit code is non-zero if any compared function differs in size.
"""

import argparse
import os
import re
import subprocess
import sys
from pathlib import Path
from typing import Dict, List, Optional, Tuple

sys.path.insert(0, str(Path(__file__).resolve().parent))

from compile_check import (  # noqa: E402
    BUILD,
    OUT,
    PRESETS,
    ROOT,
    build_command,
    ensure_tools,
    expand,
    load_project_config,
    object_command,
)

BINUTILS = BUILD / "binutils"
BINUTILS_TAG = "2.42-1"
NM = BINUTILS / "powerpc-eabi-nm"

SYMBOL_RE = re.compile(
    r"^\s*(?P<name>[^\s=]+)\s*=\s*(?P<section>[^:]+):(?P<addr>0x[0-9A-Fa-f]+);(?P<attrs>.*)$"
)
SIZE_RE = re.compile(r"size:(0x[0-9A-Fa-f]+)")
SUFFIX_RE = re.compile(r"\$\d+$")


def ensure_binutils() -> None:
    if NM.exists():
        return
    subprocess.check_call(
        [
            sys.executable,
            str(ROOT / "tools" / "download_tool.py"),
            "binutils",
            str(BINUTILS),
            "--tag",
            BINUTILS_TAG,
        ],
        cwd=ROOT,
    )


def strip_suffix(name: str) -> str:
    """`t$2350` in symbols.txt is the plain static `t` in source."""
    return SUFFIX_RE.sub("", name)


def load_symbols(version: str) -> Dict[str, List[Tuple[int, str]]]:
    """name (suffix-stripped) -> [(size, section)] from config/<VER>/symbols.txt"""
    table: Dict[str, List[Tuple[int, str]]] = {}
    path = ROOT / "config" / version / "symbols.txt"
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        m = SYMBOL_RE.match(line)
        if not m:
            continue
        attrs = m.group("attrs")
        sm = SIZE_RE.search(attrs)
        if not sm:
            continue
        if "type:function" not in attrs:
            continue
        name = strip_suffix(m.group("name"))
        table.setdefault(name, []).append((int(sm.group(1), 16), m.group("section")))
    return table


def object_functions(obj: Path) -> List[Tuple[str, int]]:
    """(name, size) of every function symbol defined in the object file."""
    out = subprocess.run(
        [str(NM), "--print-size", "--defined-only", str(obj)],
        capture_output=True,
        text=True,
        check=True,
    ).stdout
    funcs: List[Tuple[str, int]] = []
    for line in out.splitlines():
        parts = line.split()
        # <value> <size> <type> <name>
        if len(parts) != 4:
            continue
        _value, size, kind, name = parts
        if kind not in ("T", "t", "W", "w"):
            continue
        funcs.append((name, int(size, 16)))
    return funcs


def compile_unit(src: Path, version: str, extra: List[str], verbose: bool) -> Optional[Path]:
    OUT.mkdir(parents=True, exist_ok=True)
    (BUILD / version / "include").mkdir(parents=True, exist_ok=True)
    rel = src.relative_to(ROOT)
    obj = OUT / (str(rel).replace("/", "_") + ".o")
    cmd = build_command(src, obj, version, extra)
    if verbose:
        print(" ".join(cmd))
    proc = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    if proc.returncode != 0:
        print(f"FAIL (compile) {rel}")
        print((proc.stdout or "") + (proc.stderr or ""))
        return None
    return obj


def check_object(obj, out_dir: Path, symbols, verbose: bool):
    """Compile one dtk object and grade its function sizes.

    Returns (status, same, diff, unknown) where status is "ok", "diff",
    "compile-error" or "no-source".
    """
    src, dst, cmd = object_command(obj, out_dir)
    if not src.exists():
        return "no-source", [], [], []
    if verbose:
        print(" ".join(cmd))
    proc = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    if proc.returncode != 0:
        return "compile-error", [], [], []

    same, diff, unknown = [], [], []
    for name, size in object_functions(dst):
        entries = symbols.get(strip_suffix(name))
        if not entries:
            unknown.append(name)
            continue
        expected = entries[0][0]
        if len(entries) > 1 and any(e[0] == size for e in entries):
            expected = size
        if expected == size:
            same.append(name)
        else:
            diff.append((name, size, expected))
    return ("ok" if not diff else "diff"), same, diff, unknown


def check_all(args, symbols) -> int:
    config = load_project_config(args.version)
    objects = config.objects()
    out_dir = OUT / "all"
    out_dir.mkdir(parents=True, exist_ok=True)
    (BUILD / args.version / "include").mkdir(parents=True, exist_ok=True)

    rows = []
    for name, obj in objects.items():
        lib = obj.options.get("lib") or ""
        if args.lib and not any(l.lower() in lib.lower() for l in args.lib):
            continue
        if obj.completed and not args.include_matching:
            continue
        status, same, diff, unknown = check_object(obj, out_dir, symbols, args.verbose)
        if status == "no-source":
            continue
        rows.append((name, lib, obj.completed, status, len(same), len(diff), len(unknown)))
        if not args.summary and status == "diff":
            print(f"\n=== {name} ({lib}) ===")
            for fname, size, expected in diff:
                print(f"  DIFF     {fname}: built 0x{size:X} vs orig 0x{expected:X} ({size - expected:+d})")

    width = max((len(r[0]) for r in rows), default=10)
    clean = [r for r in rows if r[3] == "ok"]
    dirty = [r for r in rows if r[3] == "diff"]
    broken = [r for r in rows if r[3] == "compile-error"]
    print("\n=== units whose every function already has the original size ===")
    for r in clean:
        print(f"  {r[0]:<{width}}  ({r[1]}, {r[4]} functions)")
    print("\n=== units with size differences ===")
    for r in dirty:
        print(f"  {r[0]:<{width}}  ({r[1]}) same={r[4]} diff={r[5]}")
    if broken:
        print("\n=== units that do not compile standalone ===")
        for r in broken:
            print(f"  {r[0]:<{width}}  ({r[1]})")
    print(
        f"\ntotals: {len(rows)} units checked, {len(clean)} size-clean, "
        f"{len(dirty)} with differences, {len(broken)} compile errors"
    )
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    ap.add_argument("sources", nargs="*")
    ap.add_argument("--preset", action="append", default=[], choices=sorted(PRESETS))
    ap.add_argument("--version", default="43U", choices=["43U", "43E", "43J", "43K"])
    ap.add_argument("--cflag", action="append", default=[])
    ap.add_argument("--summary", action="store_true", help="only print per-unit totals")
    ap.add_argument(
        "--all",
        action="store_true",
        help="check every object declared in configure.py (uses each object's real cflags)",
    )
    ap.add_argument("--lib", action="append", default=[], help="with --all, restrict to these libraries")
    ap.add_argument(
        "--include-matching",
        action="store_true",
        help="with --all, also check objects already marked Matching (tool calibration)",
    )
    ap.add_argument("--show-ok", action="store_true", help="also list functions whose size matches")
    ap.add_argument("-v", "--verbose", action="store_true")
    args = ap.parse_args()

    paths = list(args.sources)
    for preset in args.preset:
        paths.extend(PRESETS[preset])
    if not paths and not args.all:
        ap.error("nothing to check (pass sources, --preset or --all)")

    ensure_tools()
    ensure_binutils()
    symbols = load_symbols(args.version)

    if args.all:
        return check_all(args, symbols)

    total_same = total_diff = total_unknown = 0
    unit_rows: List[Tuple[str, int, int, int]] = []
    failed_units: List[str] = []

    for src in expand(paths):
        rel = str(src.relative_to(ROOT))
        obj = compile_unit(src, args.version, args.cflag, args.verbose)
        if obj is None:
            failed_units.append(rel)
            continue

        same: List[str] = []
        diff: List[Tuple[str, int, int]] = []
        unknown: List[str] = []
        for name, size in object_functions(obj):
            entries = symbols.get(strip_suffix(name))
            if not entries:
                unknown.append(name)
                continue
            expected = entries[0][0]
            if len(entries) > 1 and any(e[0] == size for e in entries):
                expected = size
            if expected == size:
                same.append(name)
            else:
                diff.append((name, size, expected))

        total_same += len(same)
        total_diff += len(diff)
        total_unknown += len(unknown)
        unit_rows.append((rel, len(same), len(diff), len(unknown)))

        if not args.summary:
            print(f"\n=== {rel} ===")
            if args.show_ok:
                for name in same:
                    print(f"  same     {name}")
            for name, size, expected in diff:
                delta = size - expected
                print(f"  DIFF     {name}: built 0x{size:X} vs orig 0x{expected:X} ({delta:+d})")
            for name in unknown:
                print(f"  unknown  {name} (not a function in symbols.txt)")
            if not diff and not unknown:
                print(f"  all {len(same)} functions match the original sizes")

    print("\n=== summary ===")
    width = max((len(r[0]) for r in unit_rows), default=10)
    for rel, s, d, u in unit_rows:
        flag = "OK " if d == 0 else "!! "
        print(f"{flag}{rel:<{width}}  same={s:<4} diff={d:<4} unknown={u}")
    print(f"\ntotals: same={total_same} diff={total_diff} unknown={total_unknown}")
    if failed_units:
        print(f"compile failures: {', '.join(failed_units)}")
    return 1 if (total_diff or failed_units) else 0


if __name__ == "__main__":
    sys.exit(main())
