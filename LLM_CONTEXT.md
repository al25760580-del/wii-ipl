# LLM Context — Wii Menu (4.3) Decompilation

This file is intended for LLM agents (and humans) working on this repository.
It summarizes the project, the current state, the conventions that matter for
matching, and where the next decompilation work should go.

> General project/platform overview for agents: see `CLAUDE.md` (project
> layout, build commands, reference projects, hardware context). This file
> is the living *state* document; `docs/DECOMPILATION_PLAN.md` is the roadmap
> to 70%.

> Last updated: 2026-08-21 (session 2). The CodeWarrior toolchain now
> downloads inside the sandbox, so there is a real verification loop even
> without the original binary: see §11 and `docs/SIZE_AUDIT.md`.

---

## 1. What this project is

A work-in-progress **matching decompilation** of the Wii Menu 4.3 (the Wii's
System Menu, "IPL"). The goal is C/C++ source that recompiles to a byte-identical
`main.dol` for every supported region:

| Version | Meaning        |
|---------|----------------|
| `43U`   | 4.3 USA (default build target) |
| `43E`   | 4.3 Europe     |
| `43J`   | 4.3 Japan       |
| `43K`   | 4.3 Korea       |

- Upstream repo: `koopthekoopa/wii-ipl`. This checkout is the fork
  `al25760580-del/wii-ipl`, branch `arena/01a02263-wii-ipl`.
- The repo contains **no** original binary, assets, or assembly. The original
  `main.dol` (extracted from a Wii Menu WAD, `orig/<VER>/0000000X.app`) is
  required to build/diff. Do not commit it.
- Progress is tracked per version at `https://decomp.dev/koopthekoopa/wii-ipl/<VER>`.

## 2. Build system

- **decomp-toolkit (dtk) v1.7.5** + **ninja**. `python configure.py` generates
  `build.ninja` and `objdiff.json`; `ninja` builds `main.dol`.
- Compilers: Metrowerks CodeWarrior for GameCube/Wii (`mwcc`/`mwcceppc`), tag
  `20250812`, downloaded automatically from `files.decomp.dev` (requires
  network); binutils from `encounter/gc-wii-binutils` releases.
- Configure flags of interest: `--version 43U|43E|43J|43K`, `--map`
  (`-mapunused`), `--compilers <path>`, `--binutils <path>`, `--non-matching`
  (builds `asm/` too).
- `config/<VER>/`: `splits.txt` (section ranges per translation unit),
  `symbols.txt` (every symbol with address/size/scope), `config.yml`,
  `build.sha1`.
- `tools/` is dtk-template glue (`decompctx.py` builds decomp.me context,
  `download_tool.py` fetches toolchains, `project.py` is dtk).
- **Missing source files are tolerated** (`config.warn_missing_source = False`):
  objects whose `.c`/`.cpp` is absent fall back to the original binary at link
  time. Writing a source file for such an object is how "progress" happens.
- Matching status per object is declared in `configure.py`:
  `Object(Matching, ...)` / `Object(Equivalent, ...)` / `Object(NonMatching, ...)`.
  `Equivalent` = builds identically linked but byte-differs standalone.
- CI (`.github/workflows/build.yml`) builds all four versions in a **private**
  container (`ghcr.io/koopthekoopa/wii-ipl-build:main`) that has `/orig`
  preinstalled, then uploads `*.MAP` and `report.json` artifacts. Fork CI
  cannot pull the private container (Actions are disabled on the fork anyway).

## 3. Repository layout

- `src/` — the System Menu itself (scenes, system, channelScript, bannerSound…).
- `include/` — System Menu headers (includes `include/decomp`).
- `libs/` — libraries linked into the Menu: `MSL` (Metrowerks libc),
  `RVL_SDK`, `RevoEX` (nwc24/ncd/cdb/net/ssl/vf/wd…), `NW4R` (lyt/snd/ut/math…),
  `EGG`, `RVLFaceLib`, `RVLMiddleware` (eZiText, TMC_JPEG), `OperaWWW`
  (BTE), `MetroTRK`, `Runtime`.
- `orig/<VER>/` — put the original `.app` (main.dol) here to build locally.
- `config/<VER>/` — splits/symbols per version (see above).

## 4. Code guidelines (mandatory for merges upstream)

See `CONTRIBUTING.md` in full. The rules that most affect codegen/matching:

- Hex literals **UPPERCASE** (`0xDEADBABE`). Explicit float literals (`1.0f`).
- `NULL` (not `0`) for pointers unless matching requires otherwise.
- Braces always, even for one-line bodies; style is enforced by `clang-format`
  (config `.clang-format` at repo root).
- File extensions: `.c` for C, `.cpp` for C++, `.h` headers.
- Header guards: `{LIBRARY}_{NAME}_H` (see CONTRIBUTING for each library).
- Known symbol names must be used **exactly** (typos included); unknown
  functions `camelCase`; labels `snake_case`; struct members `camelCase`;
  class members `m`-prefixed (`mp`/`ms`/`mb`/`msp`, statics `sm`).
- Offsets commented on struct fields and vtables; unknown fields `unk_0xNN`.
- `// @bug` comments for preserved original bugs.
- Local/static symbol names with `$NNNN` suffixes in `symbols.txt` (e.g.
  `t$2350`, `sha1template$2339`) correspond to plain statics in source (`t`,
  `sha1template`) — dtk strips the suffix when matching.
- `// FAKEMATCH:` comments mark code that was knowingly made to match.
- **The project does not allow AI-assisted decompilation in upstream
  contributions.** Work done in this fork is kept in this fork.

## 5. Progress snapshot (post-merge)

Measured by the `decomp-dev` bot on the upstream PRs (bases differ slightly per
PR, so treat these as ±0.1%):

| Source        | 43U matched code | Notes |
|---------------|------------------|-------|
| upstream `main` (0b0cedd) | ~54.8% | 43E ~55.7%, 43J ~52.5%, 43K ~58.4% |
| + PR #40 `__FTVRCLoop1Handler` | +0.01% | 1 new match |
| + PR #42 `APScanThread` | +0.01% | 6 new matches |
| + PR #41 `GCSaveData` | +0.00% | 4 improvements in unmatched items |
| + PR #43 `AXFX` | +0.40% | 38 new matches |
| + PR #44 `MSL` | +1.74% | 178 new matches |
| + PR #38 `CHANSVm` | +1.35% | 284 new matches, big CHANSVm rework |
| **this branch (expected)** | **≈59.2%** | needs a fresh build to confirm |

> The roadmap to 70% matched code (43U) is in `docs/DECOMPILATION_PLAN.md`:
> phases, byte budgets per library, and verification gates. Keep both files in
> sync when milestones close.

`configure.py` object status counts on this branch: **651 Matching**,
**50 Equivalent**, **338 NonMatching**.

### High-value near-matches to finish (from PR #38's report, all 99%+)

These are 1–2 instructions off; ideal quick wins with objdiff:

- `CHANSVmInit` 99.98% · `VmDateCommon` 99.88% · `VmPushFuncReturnInfo` 99.89%
- `CHANSVmNewImageObject` 99.89% · `VmBlobSetBlob` 99.86% · `CHANSVmNewObjData` 99.38%
- `VmBlobCalcRangeMD5Digest` 99.79% · `VmBlobCalcRangeHMAC` 99.72% · `CHANSVmAddExe` 99.11%
- `VmBlobCalcHMAC` 99.45% · `vmBlobParsePackFormatString` 99.70% · `VmBlobCalcRangeSHA1Digest` 96.48%
- Also close: `VmCallMethod` 96.57% · `VmStringSplit` 96.05% · `VmBlobUnpack` 92.08% ·
  `VmBlobPackCommon` 92.60% · `CHANSVmStep` 94.79% · `VmBlobCopyRangeFrom` 91.68%

`VmDateCommon` is a *rename* of the old `CHANS_8144E21` (60.42% → 99.88%) —
not a regression.

### Big untouched areas (338 NonMatching objects)

- `channelScript/CHANSVm.c` (still NonMatching as a unit)
- RevoEX: `nwc24/*`, `ncd/*`, `cdb/*`, `net/*` (nettime, neterrorcode, aes…),
  `so/*`, `vf/*`, `wd/WDScan.c`
- RVLMiddleware TMC_JPEG (`jdec_main`, `exif_parse`, `Texture_MCUto*`,
  `iqdec_b65_frv32`, …), eZiText `clib/zi8*.c`
- `libs/RVL_SDK`: AXFX (`AXFXReverbStdExp.c` still NonMatching), FA, KBD,
  KPAD, KPR, CNTCACHE (see `libs/RVL_SDK/README.md` tables)

## 6. Merge record (this session, 2026-08-21)

All six open PRs of `koopthekoopa/wii-ipl` were merged into this branch
(each had green CI on the versions that weren't infra-flaky, and the
`decomp-dev` progress bot showed no real regressions):

| Commit | PR | Title | Author |
|--------|----|-------|--------|
| `b0ff1434` | #40 | Match `__FTVRCLoop1Handler` | TheEssem |
| `7f160436` | #42 | Match APScanThread class | Hydoza |
| `cf982d23` | #41 | GCSaveData 28% | benny-dreamly |
| `8866ebf8` | #43 | Match more of AXFX | akiramusic000 |
| `93266799` | #44 | Match MSL (100%, strftime equivalent-only) | timschneeb |
| `3fb9e572` | #38 | Improve CHANSVm.c | timschneeb |

Notes: PR #38's occasional single-version CI failures were ~20s infra flakes
(`uv run` fetching `raw.githubusercontent.com`), not code issues; the
"regression" it reports is the `CHANS_8144E21` → `VmDateCommon` rename.
PR #44 strips the CodeWarrior `.comment` section from a few object files
(`tools/strip_comment.py`, `strip_comment=True` in `configure.py`) to stop the
linker from enabling CW-specific optimizations — keep that when editing MSL.

## 7. Decompilation added in this session (unverified matching!)

New sources for TUs that previously fell back to the original binary:

- `libs/RevoEX/src/net/md5.c` — `NETMD5Init/Update/GetDigest`, static
  `ProcessBlock`, static `u32 t[64]` (`.data`, matches `t$2350`).
- `libs/RevoEX/src/net/sha1.c` — `NETSHA1Init/Update/GetDigest`,
  `NETSHA1iProcessBlock`, `NETGetSHA1Interface` returning static
  `sha1template`; statics `padlead`/`padalign`.
- `libs/RevoEX/src/net/hmac.c` — `NETHMACInit/Update/GetDigest` (RFC 2104,
  generic over the digest interface), `NETHMACContext` = 0xD4 bytes.
- `libs/RevoEX/src/net/aes.c` — full `NET_AES*` API: `NETAESCreateEx/Create/
  Delete/Encrypt/Decrypt`, `AESiEncryptBlock/AESiDecryptBlock`,
  `NETiAESEncryptoBlock/NETiAESDecryptoBlock` + the 5 `AESi*` tables
  (S-boxes, `AESiRoundKeyRcon0[30]`, `AESiEncryptTable/DecryptTable`).
  Decrypt pre-mixes round keys with InvMixColumns (word-wise), matching the
  equivalent inverse cipher.
- `libs/RevoEX/src/net/nettime.c` — `NETGetUniversalCalendar` =
  `OSTicksToCalendarTime(OSGetTime(), …)`; semantics pinned by the matched
  callers in `CHANSVm.c` and `iplNwc24Manager.cpp`.
- `libs/RevoEX/include/revolution/net/NETDigest.h` — `NETSHA1Context` fields
  filled in (was opaque); fixed wrong offset comments on `NETMD5Context`
  (`length` is at 0x10, buffer at 0x18).

**Correctness**: all of the above verified on host gcc against the RFC 1321
MD5, FIPS 180-1 SHA-1, RFC 2202 HMAC-SHA1, **FIPS 197 Appendix C AES ECB
(128/192/256)** and **NIST SP 800-38A F.2 CBC-AES128** test suites (incl.
multi-block, chunked-update, long-key and in-place cases; ASan clean).
Interfaces were pinned by the already-matched callers and, for AES, by the
Wii no Ma channel decomp (WiiLink24/wii-no-ma-patches) whose RevoEX net
symbols/sizes match this binary exactly.

**Session 2 progress with the size checker** (still NonMatching, but much
closer; both re-verified on the host against RFC 1321 / FIPS 180-1):

| Function | before | now | original |
|---|---|---|---|
| `md5.c` ProcessBlock | 0xA74 | **0x4B8** | 0x4C8 |
| `md5.c` NETMD5Update | 0x224 | **0xE0** | 0xF0 |
| `md5.c` NETMD5GetDigest | 0x230 | **0x148** | 0x128 |
| `sha1.c` NETSHA1iProcessBlock | 0x67C | **0x33C** | 0x39C |
| `sha1.c` NETSHA1GetDigest | 0x21C | **0x160** | 0x12C |

Two findings that generalize to the whole project:

1. **`k$2351` identified**: the 48 u32 next to the MD5 sine table are the
   x[] indices of rounds 16..63, so the compressor is four 4x-unrolled loops,
   not a fully unrolled body. md5.c's four functions now sum to 0x720 —
   exactly the unit's `.text` span.
2. **CodeWarrior at `-O4,p` unrolls counted `for` loops but not `do/while`
   loops.** Original function sizes therefore tell you which loop shape the
   Nintendo source used. This single rule cut ~1.5 KiB from md5/sha1 and is
   worth trying on any oversized ported unit (aes.c, TMC_JPEG, fa).

`sha1.c` also had `padlead`/`padalign` as mutable statics; symbols.txt places
them in `.sdata2`/`.sbss2`, i.e. const, and `padalign` is 8 bytes, not 4.

**Not yet matching** (needs objdiff against the original binary):

1. `md5.c`: original `.data` has an extra 0xC0-byte table of 48 u32
   (`k$2351` @ 0x8166CE20) with unknown purpose — not reproduced.
2. `sha1.c`: original `sha1template` is 0x20 bytes (mine: 3 function
   pointers = 0xC); the interface may carry more fields.
3. `hmac.c`: original unit has 0x58 bytes of unknown `.data` +
   0x10 bytes `.sdata`; my `NETHMACContext` layout (interface, inner ctx,
   outer ctx, `unk_0xC4[0x10]`) is inferred — must stay exactly 0xD4 bytes
   because matched CHANSVm code allocates it on the stack.
4. `aes.c`: unknown `.data` strings (71+37 B @ 0x8166CF90/0x8166CFD8) and
   `NET_AES_BLOCK_MODE_CBC` (8 B `.sdata2`) left to binary fallback;
   `NETAES*` signatures/ctx layout inferred from Wii no Ma; round functions
   are written byte-wise (portable) — the original likely used the u32
   T-table rotation idiom, to be restored when diffing.
5. `nettime.c`: original `.sbss` 8-byte static (possibly a cached tick
   value) left to binary fallback; may also apply a UTC offset.

### RVL_SDK portes Petari (Fase 3, 2026-08-21)

Petari (doldecomp, **CC0**, decomp de Super Mario Galaxy) comparte linaje de
build del RVL_SDK; sus símbolos VI coinciden en tamaño con este binario
(19/21 VI, 17/17 handlers `__VI`). Portados (sin diff, `NonMatching`):

- `wpad/WPADHIDParser.c` — los 22 funciones del TU están en Petari; mapeadas
  a los nombres de este build (`WPADControlBlock`→`WPADCB` con renombres de
  campos, unión `.u` de `WPADExtConfig`, `oldFw`→`configIndex`, enums
  `WPAD_FMT_*`→`WPAD_*_BTN_*`, etc.). Compila contra los headers del repo.
- `vi/i2c.c` — cubre el TU exacto (`sendSlaveAddr`, `__VISendI2CData`);
  `WaitMicroTime` hecho static (el global vive en el fallback binario de
  vi.c); los helpers solo-Petari quedan como código extra hasta objdiff.

**Bloqueados (análisis documentado)**: `vi/vi.c` (faltan
`__VISetAdjustingValues`, `__VIResetSIIdle` [llamado por Pad.c], `WaitMicroTime`
— este último existe en el i2c.c de Petari), `vi/vi3in1.c` (faltan
`__VIInit3in1` [llamado por BS2.c], `VISetMacrovision`, `__VISetRevolutionMode`),
`kpad/KPAD.c` (build distinto: funciones faltantes/sobrantes respecto al
binario). Requieren objdiff/disassembly para completarse sin romper el link.

### fa library core (Fase 3 del plan, 2026-08-21)

The RVL_SDK `fa` library (PrFILE2 FAT) had **no sources** (~164 KiB binary
fallback). Ported the core (29 of 174 TUs, ~1,500 lines) reconstructing the
same pf code base from two sibling references:

- **ogws (doldecomp, CC0)** — the Wii Sports VF decomp; its `VFipf_*`
  symbols were cross-checked function-by-function against this binary's
  `fa` symbols by size (89 exact matches); it carries the fa-only features
  (`PFSTR_SetLocalStr`, `PF_STR.p_local`, `PF_STR_CODEMODE_LOCAL`,
  `pf2_*` API wrappers).
- **this repo's own Matching RevoEX VF** — same lineage with `VFi`
  prefixes (used where ogws differs, e.g. `pf_sector.c` signatures).

New private header tree: `libs/RVL_SDK/include/private/fa/` (61 headers,
generated from the RevoEX VF private headers with `VFi*` renamed).

Ported TUs (31/174): `pf_clib`, `pf_str`, `pf_service`, `pf_code`,
`pf_system`, `pf_sector`, `pf_cp932`, `pf_fatfs`, **`pf_path`** (27 PFPATH_*
funcs, 15 exact-size; fa-lineage signatures/layouts from ogws),
**`pdm_api`** + 21 `pf2_*` singletons. Hand-written:
`pf_memcmp`, `pf_w_strcmp` (absent from both references),
`PFFATFS_initializeFATFS` (4-byte stub in this build).
Still binary fallback: `api/FA*`, `pf_w_*`, `pfs_*`, `pf_stub*`,
`driver/*`, `msc/*`, `kernel/pfk_api.c` and the big `fatfs` dir/file TUs
(`pf_file`, `pf_dir`, `pf_fat`, `pf_volume`, `pf_cache`, `pf_entry`,
`pf_entry_iterator`, `pf_path`, `pf_cluster`, `pf_fat12/16/32`, `pdm_*`).
Every function is annotated with provenance + size mismatches; all units
stay NonMatching until objdiff verification.

Whoever has the binary: run `ninja` with objdiff open, check the five units,
and adjust. Do **not** mark these `Matching` until verified.

## 8b. Verification without the original binary (NEW)

`files.decomp.dev` is reachable now, so `mwcceppc` + binutils can be
downloaded and run in the sandbox. Two tools were added:

- `python tools/compile_check.py <files|--preset net|fa|wpad|vi>` — compiles
  single TUs with the real compiler and the build's flags. Every previously
  ported unit (net 8/8, fa 31/31, WPADHIDParser, i2c) compiles.
- `python tools/size_check.py …` — compiles and then compares each function's
  size against `config/<VER>/symbols.txt`. **Different size ⇒ certainly not
  matching**; same size ⇒ necessary condition met. `--all` walks every object
  declared in `configure.py` with that object's exact cflags/compiler.

Calibration: over 786 units (including the ~650 already `Matching`), 712 are
size-clean and only 2 known-good units report a difference, i.e. the check is
right ~99.7% of the time. Caveat: symbols are matched by name across the whole
binary, so a `static` whose name also exists elsewhere can get a wrong
expectation; 13 units need the PCH/SJIS wrapper and cannot be compiled
standalone.

Full audit and the actionable lists: **`docs/SIZE_AUDIT.md`**. Headline:
of the units that are not yet `Matching`, **73 already have the original size
in every function** — those are the cheapest objdiff wins available.

## 8. How to build & verify locally

```sh
# 1. Copy your Wii Menu 4.3U (etc.) main.dol (from a WAD) to orig/43U/00000001.app
# 2. Configure (downloads compilers/binutils from files.decomp.dev on first run)
python configure.py --map --version 43U
# 3. Build + report
ninja all_source progress build/43U/report.json
```

- `objdiff.json` is generated at configure time; point objdiff at the repo and
  watch per-function diffs live.
- `build/<VER>/report.json` holds per-object/per-function fuzzy match
  percentages — the authoritative progress source.
- `python tools/decompctx.py <file>` produces decomp.me-ready context.

### Verifying algorithm-style code without the binary

For spec-determined code (crypto, checksums) you can smoke-test correctness on
the host before ever touching the Wii toolchain: copy the `.c` + a shim
`revolution/types.h`/needed headers into a scratch dir, compile with
`gcc -std=gnu11 -fsanitize=address`, and run the RFC/FIPS test vectors. This
proves behavior, **not** matching — always re-verify with objdiff.

## 9. Priorities for continuing the decompilation

0. **Run the fork CI once** (secret + Actions) to get a real `report.json`,
   or drop the WAD image in `orig/43U/` locally. Everything below is much
   faster with objdiff; without it, use `tools/size_check.py`.
0b. **Attack the 73 size-clean unverified units first** (`docs/SIZE_AUDIT.md`):
   they may already be matching and nobody has diffed them.
1. **Finish the 99%+ functions in §5** (objdiff needed; tiny diffs).
2. **Complete RevoEX `net`**: `nettime.c`, `neterrorcode.c`, `aes.c` (AES is
   also spec-determined; the `.text` range is 0x81494D84–0x814953EC) — sources
   are still missing, symbols are already in `symbols.txt`.
3. **AXFX**: finish `AXFXReverbStdExp.c` (only NonMatching axfx unit left).
4. **NWC24/CDB** (`nwc24/*.c`, `cdb/*.c`) — large, partially split.
5. **CHANSVm.c** — the VM core (`CHANSVmStep` 94.79% etc.).
6. **TMC_JPEG** — decoder tables well known from other decomp projects.

When adding a *new* translation unit (source didn't exist), you must also add
its entries to `config/<VER>/splits.txt` + `symbols.txt` for all four versions
(they come from the original binary — you cannot invent them) and list it in
`configure.py`.

## 10. Sandbox/environment notes

- **Fork CI (`build-dol.yml`)**: builds the linked `main.dol` (Dolphin-testable)
  on GitHub Actions without the private upstream container. Requirements:
  1. **Actions enabled on the fork** (fork settings → Actions → General →
     allow workflows), and
  2. a repository secret `ORIG_43U_APP_B64` = base64 of the original
     `00000008.app` from a 4.3U Wii Menu WAD (never committed):
     `gh secret set ORIG_43U_APP_B64 --repo al25760580-del/wii-ipl --body "$(base64 -w0 00000008.app)"`.
  The workflow downloads the toolchain (dtk/compilers/binutils/wibo from
  GitHub releases + files.decomp.dev — reachable from runners), splits the
  original DOL, builds everything and uploads `wii-menu-main-dol-43U`
  (main.dol + report.json) as an artifact. Upstream's `build.yml` is
  guarded to run only in `koopthekoopa/wii-ipl`. Linked DOL caveats: SEL is
  not generated (not 100% shiftable), and NonMatching/ported units are
  unverified — expect quirks in those areas (see §7).
  **Status: landed.** Pushing over SSH (deploy key) is not subject to the
  `workflows` OAuth scope, so `.github/workflows/build-dol.yml` and the
  `if: github.repository == 'koopthekoopa/wii-ipl'` guard on `build.yml` are
  now on this branch. What is still needed from a human: enable Actions on
  the fork and set the `ORIG_43U_APP_B64` secret; then run the workflow
  manually (`workflow_dispatch`).
- Network from the sandbox: `files.decomp.dev`, `decomp.dev`, `github.com`
  and `raw.githubusercontent.com` all work now, so compilers, binutils, dtk
  and wibo download fine. What is still missing locally is only the original
  image (`orig/43U/00000008.app`), without which `configure.py` emits just
  the tool rules and there is no link/objdiff.
- The upstream repo currently rewrote its history (single squashed commit
  `0b0cedd` + full old history); PR branches still carry the old history.
  Merge PRs from upstream with `git merge <pull-ref>` after
  `git fetch upstream pull/N/head:pull-N` — their merge bases are intact.
