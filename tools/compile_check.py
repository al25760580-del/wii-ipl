#!/usr/bin/env python3
"""compile_check.py — compile single translation units with the real
CodeWarrior compiler *without* needing the original binary.

The normal build (`configure.py` + `ninja`) requires `orig/<VER>/*.app` so
that dtk can split the DOL and link. When that binary is unavailable (e.g.
in a CI sandbox or on a contributor machine that has no WAD yet) there is
still one very useful check that can be run: does the source even compile
with `mwcceppc`, with the exact flags the build would use?

This catches the majority of mistakes in newly written / ported TUs
(missing headers, wrong types, C++-isms in C files, CW-unsupported syntax)
long before objdiff enters the picture.

Usage:
    python tools/compile_check.py libs/RevoEX/src/net/md5.c ...
    python tools/compile_check.py --preset fa
    python tools/compile_check.py --list-presets

Exit code is non-zero if any unit fails to compile.
"""

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path
from typing import List, Tuple

ROOT = Path(__file__).resolve().parent.parent
BUILD = ROOT / "build"
COMPILERS = BUILD / "compilers"
BINUTILS_DIR = BUILD / "binutils"
TOOLS = BUILD / "tools"
OUT = BUILD / "compile_check"

MW_VERSION = "GC/3.0a5.2"
COMPILERS_TAG = "20250812"
WIBO_TAG = "1.0.0"

BTE_INC = "libs/RVL_SDK/include/private/bte"

INCLUDES = [
    "include",
    "include/global",
    "libs/MetroTRK/include",
    "libs/Runtime/include",
    "libs/MSL/include",
    "libs/RVL_SDK/include",
    "libs/RevoEX/include",
    "libs/NW4R/include",
    "libs/RVLMiddleware/eZiText/include",
    "libs/RVLMiddleware/TMC_JPEG/include",
    "libs/RVLFaceLib/include",
    "libs/EGG/include",
    "libs/OperaWWW/include",
]

# Mirrors cflags_base in configure.py (43U, release).
CFLAGS_BASE = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    "-O4,p",
    "-inline auto",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-DSDK_IPL",
    "-D_REVOLUTION",
    "-DMEM_MANAGER_DIRECT",
    "-DNDEBUG=1",
    "-DTARGET_RVL",
]

# cflags_sdk (RVL_SDK / RevoEX): base + ipa file + fp_contract off
CFLAGS_SDK = CFLAGS_BASE + ["-ipa file", "-fp_contract off"]

PRESETS = {
    "net": ["libs/RevoEX/src/net"],
    "fa": ["libs/RVL_SDK/src/fa"],
    "wpad": ["libs/RVL_SDK/src/wpad/WPADHIDParser.c"],
    "vi": ["libs/RVL_SDK/src/vi/i2c.c"],
}


def version_flags(version: str) -> List[str]:
    version_num = {"43U": 0, "43E": 1, "43J": 2, "43K": 3}[version]
    return [
        f"-DBUILD_VERSION={version_num}",
        f"-DVERSION_{version}",
    ]


def ensure_tools() -> None:
    dl = ROOT / "tools" / "download_tool.py"
    TOOLS.mkdir(parents=True, exist_ok=True)
    if not (COMPILERS / MW_VERSION / "mwcceppc.exe").exists():
        subprocess.check_call(
            [sys.executable, str(dl), "compilers", str(COMPILERS), "--tag", COMPILERS_TAG],
            cwd=ROOT,
        )
    if os.name != "nt" and not (TOOLS / "wibo").exists():
        subprocess.check_call(
            [sys.executable, str(dl), "wibo", str(TOOLS / "wibo"), "--tag", WIBO_TAG],
            cwd=ROOT,
        )


def expand(paths: List[str]) -> List[Path]:
    out: List[Path] = []
    for raw in paths:
        p = (ROOT / raw) if not os.path.isabs(raw) else Path(raw)
        if p.is_dir():
            for ext in ("*.c", "*.cpp"):
                out.extend(sorted(p.rglob(ext)))
        elif p.exists():
            out.append(p)
        else:
            print(f"warning: no such file or directory: {raw}", file=sys.stderr)
    return out


def build_command(src: Path, obj: Path, version: str, extra: List[str]) -> List[str]:
    cflags: List[str] = list(CFLAGS_SDK)
    cflags += [f"-i {inc}" for inc in INCLUDES]
    cflags.append(f"-ir {BTE_INC}")
    cflags.append(f"-i {BTE_INC}")
    cflags.append(f"-i build/{version}/include")
    cflags += version_flags(version)
    cflags += extra

    argv: List[str] = []
    if os.name != "nt":
        argv.append(str(TOOLS / "wibo"))
    argv.append(str(COMPILERS / MW_VERSION / "mwcceppc.exe"))
    for flag in cflags:
        # flags such as '-pragma "cats off"' carry an embedded argument
        argv.extend(split_flag(flag))
    argv += ["-c", "-o", str(obj), str(src.relative_to(ROOT))]
    return argv


def load_project_config(version: str):
    """Import configure.py and return its fully built ProjectConfig.

    configure.py is a script, not a module: it parses argv and then calls
    generate_build(config) (which needs the original binary). Stub that call
    out and run it, so the exact per-object cflags/compiler of the real build
    become available here.
    """
    import runpy

    if str(ROOT) not in sys.path:
        sys.path.insert(0, str(ROOT))
    import tools.project as project

    captured = {}

    def capture(config):
        captured["config"] = config

    real_generate = project.generate_build
    real_argv = sys.argv
    project.generate_build = capture
    sys.argv = [
        "configure.py",
        "--version",
        version,
        "--compilers",
        str(COMPILERS),
        "--binutils",
        str(BINUTILS_DIR),
    ]
    cwd = os.getcwd()
    try:
        os.chdir(ROOT)
        runpy.run_path(str(ROOT / "configure.py"), run_name="__main__")
    finally:
        os.chdir(cwd)
        sys.argv = real_argv
        project.generate_build = real_generate
    return captured.get("config")


def object_command(obj, out_dir: Path) -> Tuple[Path, Path, List[str]]:
    """(source, object, argv) for a resolved dtk Object."""
    src = ROOT / obj.src_path
    rel = obj.src_path
    dst = out_dir / (str(rel).replace("/", "_") + ".o")
    mw_version = obj.options["mw_version"]
    cflags = obj.options["cflags"]
    if isinstance(cflags, str):
        cflags = [cflags]
    cflags = list(cflags) + list(obj.options.get("extra_cflags") or [])

    argv: List[str] = []
    if os.name != "nt":
        argv.append(str(TOOLS / "wibo"))
    argv.append(str(COMPILERS / mw_version / "mwcceppc.exe"))
    for flag in cflags:
        argv.extend(split_flag(flag))
    argv += ["-c", "-o", str(dst), str(rel)]
    return src, dst, argv


def split_flag(flag: str) -> List[str]:
    if '"' in flag:
        head, _, rest = flag.partition(" ")
        return [head, rest.strip('"')]
    if " " in flag:
        head, _, rest = flag.partition(" ")
        return [head, rest]
    return [flag]


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("sources", nargs="*", help="source files or directories to compile")
    ap.add_argument("--preset", action="append", default=[], choices=sorted(PRESETS), help="compile a known group of units")
    ap.add_argument("--list-presets", action="store_true")
    ap.add_argument("--version", default="43U", choices=["43U", "43E", "43J", "43K"])
    ap.add_argument("--cflag", action="append", default=[], help="extra flag passed to mwcceppc")
    ap.add_argument("-v", "--verbose", action="store_true")
    args = ap.parse_args()

    if args.list_presets:
        for name, paths in PRESETS.items():
            print(f"{name}: {', '.join(paths)}")
        return 0

    paths = list(args.sources)
    for preset in args.preset:
        paths.extend(PRESETS[preset])
    if not paths:
        ap.error("nothing to compile (pass sources or --preset)")

    ensure_tools()
    if OUT.exists():
        shutil.rmtree(OUT)
    OUT.mkdir(parents=True, exist_ok=True)
    # build/<VER>/include is generated by the real build; make sure the
    # include path exists so the compiler does not complain about it.
    (BUILD / args.version / "include").mkdir(parents=True, exist_ok=True)

    sources = expand(paths)
    failures = []
    for src in sources:
        rel = src.relative_to(ROOT)
        obj = OUT / (str(rel).replace("/", "_") + ".o")
        cmd = build_command(src, obj, args.version, args.cflag)
        if args.verbose:
            print(" ".join(cmd))
        proc = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
        log = (proc.stdout or "") + (proc.stderr or "")
        if proc.returncode != 0:
            failures.append((rel, log))
            print(f"FAIL  {rel}")
            if args.verbose:
                print(log)
        else:
            warn = " (warnings)" if "warning" in log.lower() else ""
            print(f"ok    {rel}{warn}")

    print(f"\n{len(sources) - len(failures)}/{len(sources)} units compiled")
    if failures:
        print("\n=== failures ===")
        for rel, log in failures:
            print(f"\n--- {rel} ---")
            print(log.strip()[:4000])
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
