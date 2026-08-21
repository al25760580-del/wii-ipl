#!/usr/bin/env bash
# setup_local.sh - one-shot local setup for building and testing the Wii Menu
# decompilation on Linux.
#
#   ./tools/setup_local.sh                 # 43U, configure + build + report
#   ./tools/setup_local.sh --version 43J
#   ./tools/setup_local.sh --check         # only verify the environment
#   ./tools/setup_local.sh --sizes         # build, then run the size audit
#
# What it does:
#   1. checks python3 / ninja / git and tells you exactly what to install
#   2. checks orig/<VER>/<image>.app and verifies its SHA-1 against config.yml
#   3. runs configure.py, downloads the toolchain (CodeWarrior, binutils, dtk,
#      wibo) and builds main.dol + report.json
#
# The original image is NEVER downloaded, committed or uploaded by this
# script; you have to provide it yourself (dump it from your own console).

set -euo pipefail

VERSION="43U"
MODE="build"

while [ $# -gt 0 ]; do
    case "$1" in
        --version) VERSION="$2"; shift 2 ;;
        --check)   MODE="check"; shift ;;
        --sizes)   MODE="sizes"; shift ;;
        -h|--help) sed -n '2,20p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) echo "unknown argument: $1" >&2; exit 2 ;;
    esac
done

cd "$(dirname "$0")/.."
ROOT="$PWD"

bold() { printf '\033[1m%s\033[0m\n' "$*"; }
ok()   { printf '  \033[32mok\033[0m   %s\n' "$*"; }
bad()  { printf '  \033[31mFAIL\033[0m %s\n' "$*"; }
warn() { printf '  \033[33mwarn\033[0m %s\n' "$*"; }

# ---------------------------------------------------------------- 1. tooling

bold "1. Environment"
missing=0

if command -v python3 >/dev/null 2>&1; then
    ok "python3 $(python3 --version 2>&1 | cut -d' ' -f2)"
else
    bad "python3 not found"; missing=1
fi

if command -v ninja >/dev/null 2>&1; then
    ok "ninja $(ninja --version)"
else
    bad "ninja not found"
    echo "       Debian/Ubuntu : sudo apt install ninja-build"
    echo "       Fedora        : sudo dnf install ninja-build"
    echo "       Arch          : sudo pacman -S ninja"
    missing=1
fi

if command -v git >/dev/null 2>&1; then
    ok "git $(git --version | cut -d' ' -f3)"
else
    bad "git not found"; missing=1
fi

if command -v clang-format >/dev/null 2>&1; then
    ok "clang-format $(clang-format --version | grep -o '[0-9][0-9.]*' | head -1)"
else
    warn "clang-format not found (only needed to format your own patches: pip install clang-format)"
fi

[ "$missing" -eq 0 ] || { echo; bad "install the missing tools and run again"; exit 1; }

# ------------------------------------------------------- 2. original image

echo
bold "2. Original image (orig/$VERSION)"

CONFIG="config/$VERSION/config.yml"
[ -f "$CONFIG" ] || { bad "no such version: $VERSION (expected one of 43U 43E 43J 43K)"; exit 1; }

APP=$(grep -E "^object:" "$CONFIG" | head -1 | sed "s/.*'\(.*\)'.*/\1/")
WANT_SHA=$(grep -E "^hash:" "$CONFIG" | head -1 | awk '{print $2}')
IMAGE="orig/$VERSION/$APP"

mkdir -p "orig/$VERSION"

if [ ! -f "$IMAGE" ]; then
    bad "missing $IMAGE"
    cat <<EOF

       This is the Wii Menu $VERSION executable. It is NOT in the repository
       and must come from your own console / your own WAD:

         - dump it from your Wii's NAND, or extract it from a WAD you own,
         - the file must be the DECRYPTED content (a DOL), not the encrypted
           .app straight off NUS,
         - drop it at $IMAGE

       Verify it with:
         sha1sum $IMAGE
         # expected: $WANT_SHA

       It is covered by .gitignore (orig/*/*), so it can never be committed.
EOF
    exit 1
fi

GOT_SHA=$(sha1sum "$IMAGE" | awk '{print $1}')
if [ "$GOT_SHA" = "$WANT_SHA" ]; then
    ok "$IMAGE ($(stat -c%s "$IMAGE") bytes, sha1 matches)"
else
    bad "$IMAGE has the wrong SHA-1"
    echo "       got      $GOT_SHA"
    echo "       expected $WANT_SHA"
    echo "       (wrong region/version, or still encrypted)"
    exit 1
fi

if [ "$MODE" = "check" ]; then
    echo; ok "environment is ready - run '$0 --version $VERSION' to build"
    exit 0
fi

# --------------------------------------------------------------- 3. build

echo
bold "3. Configure (downloads the toolchain on first run)"
python3 configure.py --map --version "$VERSION"
ok "build.ninja and objdiff.json generated"

echo
bold "4. Toolchain"
ninja tools
ok "CodeWarrior, binutils, dtk and wibo in build/"

echo
bold "5. Build"
ninja all_source progress "build/$VERSION/report.json"

echo
ok "main.dol  -> build/$VERSION/main.dol"
ok "report    -> build/$VERSION/report.json"

if [ "$MODE" = "sizes" ]; then
    echo
    bold "6. Function size audit"
    python3 tools/size_check.py --all --summary --version "$VERSION" || true
fi

cat <<EOF

Next steps
  - objdiff : point it at $ROOT (it reads objdiff.json). Per-function diffs,
              live. https://github.com/encounter/objdiff
  - Dolphin : run build/$VERSION/main.dol. Note the SEL file is not generated,
              so the DOL is not fully shiftable: Wii Settings and a few channel
              banners misbehave. Everything else boots.
  - progress: ninja progress
  - a single unit: python tools/size_check.py libs/RevoEX/src/net/md5.c
EOF
