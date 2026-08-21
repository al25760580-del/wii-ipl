# CLAUDE.md — Contexto del proyecto para agentes LLM

> Guía de contexto general del proyecto. Para el estado de progreso,
> plan de trabajo y notas de sesión consulta **`LLM_CONTEXT.md`** (más
> actualizado) y **`docs/DECOMPILATION_PLAN.md`** (roadmap al 70%).

## Visión General del Proyecto

Decompilación del **Wii Menu (System Menu / IPL) versión 4.3**, impulsada por la comunidad.
El objetivo es producir código C/C++ que compile a un binario **byte-idéntico** al
`main.dol` original, usando el compilador original **Metrowerks CodeWarrior para PowerPC**.

> **IMPORTANTE:** este repositorio **NO** contiene assets ni ensamblado del ejecutable
> original. Se requiere un WAD existente del Wii Menu colocado en `orig/[Versión]`.

### Contexto de la Plataforma

- **Hardware objetivo:** Nintendo Wii (CPU Broadway / IBM PowerPC 750CL, GPU Hollywood)
- **Arquitectura:** PowerPC 32-bit **Big Endian**
- **Memoria:** MEM1 (24 MB) + MEM2 (64 MB, solo Wii)
- **OS subyacente:** IOS (coprocesador ARM "Starlet") — el Wii Menu corre como título PPC sobre IOS
- **Formato de salida:** `main.dol` (DOL executable)

## Versiones soportadas

| Versión | Región  | WAD esperado            |
|---------|---------|-------------------------|
| `43U`   | USA     | `orig/43U/00000008.app` |
| `43E`   | Europa  | `orig/43E/*.app`        |
| `43J`   | Japón   | `orig/43J/*.app`        |
| `43K`   | Corea   | `orig/43K/*.app`        |

> El número exacto del `.app` por versión está en `config/<VER>/config.yml`
> (`object_base` / `object`). Nunca commitear el binario original.

## Proyectos de Referencia Clave

### 1. devkitPro/libogc
- **Qué es:** Biblioteca C de abstracción de hardware para homebrew de GameCube y Wii.
- **Relevancia:** Define las APIs estándar de bajo nivel (GX, VIDEO, PAD, WPAD, IOS IPC,
  NAND, ES, ISFS…) análogas a las del SDK oficial que usa el Wii Menu.
- **URL:** https://github.com/devkitPro/libogc

### 2. Wii-Linux/wii-linux-ngx (branch wii-mainline)
- **Qué es:** Kernel Linux portado al hardware Wii/GameCube.
- **Relevancia:** Documentación de hardware real a nivel de registros (framebuffer,
  SDHC, USB, Wi-Fi, IRQs de Hollywood, boot chain `MINI/IOS -> bootloader -> kernel`).
- **URL:** https://github.com/Wii-Linux/wii-linux-ngx/tree/wii-mainline

### 3. ReflowOven/Wee_Wii
- **Qué es:** Recreación open-source del PCB del Wii.
- **Relevancia:** Esquemáticos y señales del board — útil para init sequences de
  bajo nivel (EXI, SI, DI).
- **URL:** https://github.com/ReflowOven/Wee_Wii

### 4. Proyectos de decompilación usados como referencia (verificados en sesiones previas)
- **doldecomp/ogws** (Wii Sports, **CC0**): RVL_SDK/VF — fuente del puerto de la
  librería `fa` (PrFILE2 FAT) de este repo.
- **kiwi515/open_rvl**: decompilación del SDK RVL (build de Wii Sports) — útil para
  cotejar estructura de librerías SDK.
- **WiiLink24/wii-no-ma-patches**: canal Wii no Ma; su RevoEX `net` (AES) coincide
  en símbolos y tamaños con el de este binario.
- **decomp.me / ghidra.decomp.dev**: herramientas de la comunidad (ver README).

## Arquitectura del Proyecto

```
wii-ipl/
├── src/               # Código del Wii Menu (escenas, sistema, channelScript…)
├── include/           # Headers del Wii Menu (incluye include/decomp)
├── libs/              # Librerías del SDK enlazadas en el Menu:
│   ├── RVL_SDK/       #   SDK Revolution (OS, GX, VI, WPAD, NAND, ES, ax, fa…)
│   ├── RevoEX/        #   RevoEX (nwc24, ncd, cdb, net, nhttp, vf, wd, so, ssl)
│   ├── MSL/           #   Metrowerks Standard Library (libc)
│   ├── NW4R/          #   NintendoWare4R (lyt, snd, ut, math, db)
│   ├── EGG/           #   Motor de desarrollo de EAD
│   ├── RVLFaceLib/    #   Biblioteca de Miis (RFL)
│   ├── RVLMiddleware/ #   eZiText (IME), TMC_JPEG
│   ├── OperaWWW/      #   Browser (BTE)
│   ├── MetroTRK/      #   Debugger
│   └── Runtime/       #   Runtime PPCEABI
├── config/<VER>/      # splits.txt, symbols.txt, config.yml, build.sha1
├── orig/<VER>/        # WADs originales (NO incluidos — el usuario los provee)
├── tools/             # dtk glue (download_tool.py, decompctx.py, strip_comment.py…)
├── configure.py       # Genera build.ninja y objdiff.json (decomp-toolkit)
├── docs/              # DECOMPILATION_PLAN.md (plan al 70%)
└── build/<VER>/       # Salida de compilación (main.dol, report.json)
```

## Comandos de Build

### Requisitos previos
- Python 3, ninja en PATH
- Compiladores Metrowerks (`mwcc`/`mwcceppc`) y binutils: se descargan
  automáticamente (`tools/download_tool.py`) desde `files.decomp.dev` /
  `encounter/gc-wii-binutils` — requiere red
- En Linux/macOS: `wibo` (wrapper de binarios Win32) se descarga automáticamente

### Configuración y compilación
```bash
# Configuración inicial (por defecto: 43U)
python configure.py

# Otra versión: --version 43U | 43E | 43J | 43K
python configure.py --version 43J

# Build + reporte de progreso
ninja all_source progress build/43U/report.json

# Flags útiles: --map (-mapunused), --compilers <path>, --binutils <path>,
#               --non-matching (compila también asm/)
```

- Genera `main.dol` en `build/<VER>/`; **NO** genera un WAD.
- `report.json` contiene el % de match por objeto/función (fuente autoritativa).
- **El archivo SEL no se genera**, por lo que el binario no es 100% shiftable:
  casi todo funciona excepto Wii Settings y algunos Channel Banners (Forecast, etc.).
- En este fork: GitHub Actions no está disponible y los hosts de toolchains están
  bloqueados desde el sandbox; el build/objdiff se hace en una máquina con red.

### Diffing con objdiff
```bash
# Tras configure, objdiff.json existe en la raíz
# objdiff: https://github.com/encounter/objdiff → "Project directory" = raíz del repo
```

## Convenciones de Código

Las reglas completas están en **`CONTRIBUTING.md`** (resumen de las que afectan al matching):

- **Compilador:** CodeWarrior Metrowerks (C/C++ pre-C++11). Nada de C++ moderno.
- **Endianness:** Big Endian — toda manipulación de datos debe respetarlo.
- **Match byte-perfect:** un solo byte de diferencia es un fallo. Se verifica con
  objdiff antes de marcar `Object(Matching, …)` en `configure.py`.
- **Nombres:** usar los nombres originales del SDK/Nintendo **exactamente** cuando se
  conocen (símbolos de `symbols.txt`, DWARF, leaks de símbolos de otros juegos).
  - Sin nombre conocido: funciones `camelCase`, labels `snake_case`,
    campos de struct `camelCase`, miembros de clase con prefijos `m`/`mp`/`ms`/`mb`
    (`sm` para estáticos), campos desconocidos `unk_0xNN`.
  - Los `$NNNN` de `symbols.txt` (p. ej. `t$2350`) son estáticos locales: en el
    código van sin sufijo (`t`).
- **No "mejorar" el código original:** reproducir bugs con `// @bug`, trucos de match
  con `// FAKEMATCH:`, y no "limpiar" redundancias.
- **Orden:** las funciones deben quedar en el orden del binario (el linker depende de ello).
- **Literales:** hex en **MAYÚSCULAS** (`0xDEADBABE`), floats explícitos (`1.0f`),
  `NULL` (no `0`) para punteros salvo que afecte el match.
- **Estilo:** llaves siempre, formateo con `clang-format` (config en la raíz);
  extensiones `.c`/`.cpp`/`.h`; header guards `{LIBRARY}_{NAME}_H` (ver CONTRIBUTING).
- **Offsets comentados** en structs/vtables (`// 0x00`).
- **Registros de hardware:** `volatile` siempre; bases 0x0D000000 (física) /
  0xCD000000 (virtual) para Hollywood; accesos vía `OSReadXX`/`OSWriteXX` o
  punteros volatile.
- **IA:** el proyecto **no acepta decompilación asistida por IA en upstream**
  (CONTRIBUTING.md). Todo el trabajo de este fork se queda en el fork; cualquier
  PR a `koopthekoopa/wii-ipl` requiere revisión humana completa.

### Tipos de Datos del SDK
```c
typedef signed char         s8;
typedef signed short        s16;
typedef signed long         s32;
typedef signed long long    s64;
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned long       u32;
typedef unsigned long long  u64;
typedef float               f32;
typedef double              f64;
typedef int                 BOOL;

#define TRUE  1
#define FALSE 0
#define NULL  ((void*)0)
```
(Variantes por librería: `pf_s8/pf_u32…` en PrFILE2/`fa`, etc.)

## Flujo de Trabajo para Contribuciones

### Decompilar una función
1. **Identificar** la función objetivo en objdiff (izquierda = original, derecha = tu código)
2. **Analizar** el PowerPC del original instrucción a instrucción
3. **Escribir** el C/C++ equivalente en el TU correspondiente (si el TU no existe,
   crearlo + añadir sus splits/símbolos en las 4 versiones + listarlo en `configure.py`)
4. **Compilar** (`ninja`) y verificar **0% diff** en objdiff
5. **Iterar** — tipos, casts, orden de operaciones

### Tips para matching
- CodeWarrior es muy sensible al **orden de declaración de variables locales**
- `register` puede afectar la asignación de registros
- El orden de evaluación de subexpresiones importa
- Comparar prologue/epilogue del stack frame para validar la firma
- `#pragma` de CodeWarrior para controlar inlining/optimización cuando haga falta
- Para código basado en especificaciones (crypto, checksums): verificar primero en
  el host con vectores RFC/FIPS (gcc + ASan) — prueba comportamiento, **no** matching

### Git y Branches
- Rama de trabajo: `arena/01a02263-wii-ipl` (única rama de esta sesión)
- Commits atómicos: un TU (o conjunto coherente) = un commit
- Ejemplos de mensaje: `Match <unidad> — <función>`, `Decompile <módulo>/<TU>`
- **Autoría:** commits de trabajo con autor y committer `MilaSoraki
  <mila.gastelum@autustici.org>` (`GIT_AUTHOR_NAME/EMAIL` + `GIT_COMMITTER_NAME/EMAIL`)
- No marcar `Matching` en `configure.py` sin verificación objdiff; los puertos
  sin verificar quedan `NonMatching` con la procedencia anotada en el código

## Subsistemas Clave del Wii Menu

| Subsistema | Descripción | Dónde está | Ref. libogc |
|---|---|---|---|
| **OS** | Inicialización, interrupciones, memoria, cache | `libs/RVL_SDK/src/os` | `gc/ogc/system.h` |
| **VI** | Video Interface — modo de video, framebuffer | `libs/RVL_SDK/src/vi` | `gc/ogc/video.h` |
| **GX** | Graphics — renderizado 3D/2D, TEV, texturas | `libs/RVL_SDK/src/gx` | `gc/ogc/gx.h` |
| **PAD** | GameCube controller input | `libs/RVL_SDK/src/pad` | `gc/ogc/pad.h` |
| **WPAD** | Wii Remote input (Bluetooth) | `libs/RVL_SDK/src/wpad` | `gc/wiiuse/wpad.h` |
| **NAND** | Filesystem en la NAND flash interna | `libs/RVL_SDK/src/nand` | `gc/ogc/isfs.h` |
| **ES** | Encrypted Storage — títulos, tickets, TMDs | `libs/RVL_SDK/src/es` | `gc/ogc/es.h` |
| **IOS IPC** | IPC con IOS (ARM) | `libs/RVL_SDK/src/ipc` | `gc/ogc/ipc.h` |
| **FA/VF** | FAT (SD/USB/NAND) — PrFILE2 | `libs/RVL_SDK/src/fa`, `libs/RevoEX/src/vf` | N/A (propietario) |
| **NWC24** | WiiConnect24 — mensajería, updates en standby | `libs/RevoEX/src/nwc24` | N/A (propietario) |
| **SC** | System Configuration — settings persistentes | `libs/RVL_SDK/src/sc` | N/A (propietario) |
| **NHTTP** | HTTP client para servicios online | `libs/RevoEX/src/nhttp` | N/A (propietario) |
| **CHANSVm** | VM del channelScript del Menu | `src/channelScript/CHANSVm.c` | N/A (propietario) |

## Mapeo de Memoria del Wii (Referencia Rápida)

```
Dirección Virtual    | Tamaño | Descripción
---------------------|--------|--------------------------------------------
0x80000000-0x817FFFFF| 24 MB  | MEM1 (RAM principal, accesible por GC y Wii)
0x90000000-0x93FFFFFF| 64 MB  | MEM2 (RAM extendida, solo Wii)
0xC0000000-0xC17FFFFF| 24 MB  | Espejo sin cache de MEM1 (alias de 0x80000000)
0xCC000000-0xCC00FFFF| 64 KB  | Registros de Flipper/Hollywood (MMIO)
0xCD000000-0xCD0008FF|  ~2 KB | Registros específicos de Hollywood (IPC, GPIO, …)
```

## Debugging y Herramientas

- **objdiff:** comparar funciones decompiladas vs original (herramienta principal)
- **Ghidra / IDA Pro:** análisis estático del `main.dol` (Ghidra: servidor compartido
  `ghidra.decomp.dev`, repo `Wii_IPL_Ghidra` — ver README)
- **Dolphin Emulator:** testing funcional (no para matching)
- **doltool:** extraer secciones del DOL
- **wibo:** wrapper de binarios Win32 para CodeWarrior en Linux
- **decomp.me:** diffs colaborativos (`python tools/decompctx.py <archivo>`)

## Limitaciones y Advertencias

- **SEL no se genera en el build** → el código no es 100% shiftable (Wii Settings y
  algunos Channel Banners fallan)
- **Windows nativo recomendado**; WSL rompe las notificaciones de filesystem de objdiff
- **No incluir assets originales de Nintendo** en commits — solo código decompilado
- **El compilador CodeWarrior es propietario** — no redistribuir; los colaboradores lo
  obtienen por sus propios medios
- **Progreso y estado real:** consultar `LLM_CONTEXT.md` (estado de la sesión,
  merges, puertos sin verificar) y `docs/DECOMPILATION_PLAN.md` (fases y gates)
