# Plan de decompilación — Meta: 70% de código matcheado

> Documento vivo. Versión 1.0 — 2026-08-21.
> Métrica objetivo: **"Matched code" de decomp.dev para 43U ≥ 70%** (badge del README).
> Métricas secundarias: linked code, matched data, y paridad de símbolos en 43E/J/K.

---

## 1. Estado actual

Después de fusionar los PRs #38–#44 de `koopthekoopa/wii-ipl`:

| Métrica (43U) | Valor |
|---|---|
| Matched code (fuzzy, decomp.dev) | **≈59.2%** (esperado; a verificar en el primer build local) |
| Brecha al 70% | **≈10.8 pts ≈ 323 KiB de funciones** |
| Código total (.text + .init) | 2,995,188 B (2,925 KiB) |
| Objetos en `configure.py` | 651 Matching · 50 Equivalent · 338 NonMatching |
| Tufts con fuente propia | 99.7% del código (solo 0.3% cae al binario original) |

> Nota: el número "fuzzy" de decomp.dev cuenta funciones individuales al 100%;
> el proxy mecánico por-TU (objeto `Matching`) da 45.6%. El plan ataca **unidades
> enteras** para que ambas métricas suban juntas.

**Reparto de lo que falta** (bytes de código por librería, por-TU):

| Bloque | Librería(s) | KiB | % del total |
|---|---|---|---|
| IME/teclado | `keyboard` (211.8) + `zi8clib` (159.1) | **370.9** | 12.4% |
| SDK RVL | `fa` (164.4) + `wad` (24.5) + `wpad` (21.7) + `vi` (20.7) + `kpad` (13.1) + `nup` (13.6) + `kbd` (6.0) + `sdi` (5.9) + `kpr` (1.5) | **271.4** | 9.1% |
| Escenas | `setting` (82.3) + `address` (51.1) + `sdChannel*` (80.6) + `memoryCard` (30.3) + `BS2` (21.0) + `utility` (11.2) | **276.5** | 9.2% |
| RevoEX servicios | `nwc24` (62.6) + `wd` (48.6) + `nhttp` (30.0) + `cdb` (28.6) + `vf-dev` (28.3) + `so` (11.1) + `ncd` (4.4) | **213.6** | 7.1% |
| CHANSVm | `channelScript/CHANSVm.c` (53.6) | **53.6** | 1.8% |
| Otros | `system/odh.cpp` (14.7), `AXFX` (5.8), `net` (8.5), etc. | **~53** | 1.8% |

### Unidades prioritarias (mayor tamaño, top 25)

| KiB | Unidad | Observaciones |
|---:|---|---|
| 53.5 | `src/channelScript/CHANSVm.c` | 284+ funciones ya al 100%; ~26 KiB parciales al 92–99.98% |
| 50.7 | `src/keyboard/tiInputForm.cpp` | escena IME principal |
| 47.8 | `eZiText/clib/zi8cgetc.c` | motor zi8 |
| 37.9 | `src/scene/setting/iplSetting.cpp` | Wii Settings |
| 33.8 | `src/scene/sdChannelSelect/iplSDChannelSelect.cpp` | |
| 28.5 | `src/keyboard/tiPcKeyboard.cpp` | |
| 27.1 | `src/scene/address/iplAddressEdit.cpp` | |
| 24.5 | `RVL_SDK/wad/wad.c` | |
| 24.0 | `src/keyboard/tiCandidateBox.cpp` | |
| 24.0 | `src/scene/address/iplAddress.cpp` | |
| 21.7 | `RVL_SDK/wpad/WPADHIDParser.c` | |
| 21.7 | `eZiText/clib/zi8alpha.c` | |
| 21.5 | `eZiText/clib/zi81key.c` | |
| 20.9 | `src/scene/sdChannelMemory/iplSDMemory.cpp` | |
| 20.2 | `src/keyboard/MyTiManager.cpp` | |
| 19.2 | `src/scene/setting/ATERM.c` | |
| 19.0 | `src/keyboard/tiCellPhone.cpp` | |
| 19.0 | `RVL_SDK/fa/pf_dir.c` | |
| 18.6 | `src/scene/sdChannelTitle/iplSDChannelTitle.cpp` | |
| 18.0 | `RVL_SDK/fa/pf_volume.c` | |
| 17.8 | `RVL_SDK/fa/pf_file.c` | |
| 17.0 | `src/BS2/BS2Mach.c` | |
| 16.2 | `src/scene/setting/AOSS.c` | |
| 15.9 | `TMC_JPEG/texturecvtr/Texture_MCUtoY8U8V8.c` | |
| 14.7 | `src/system/odh.cpp` | |

---

## 2. Prerrequisito bloqueante (Fase 0) — bucle de verificación

Hoy la rama no se puede difear en el sandbox: no hay binario original, el fork no
tiene CI y los hosts de toolchains están bloqueados. **Ningún commit de
"matcheado" debe subirse sin verificación**, así que primero:

1. **Binario original**: extraer `main.dol` de un WAD 4.3U → `orig/43U/00000001.app`
   (legal: se obtiene de la consola del usuario; no se commitea).
2. **Build local**: `python configure.py --map --version 43U && ninja
   all_source progress build/43U/report.json` (descarga compilers/binutils de
   `files.decomp.dev`; fuera del sandbox funciona).
3. **objdiff** apuntado al repo (se configura solo desde `objdiff.json`).
4. **Ciclo por función**: editar → diff en objdiff → si 100%, subir commit con
   mensaje `Unit Name 100%` → revisar `report.json`.
5. Opcional: re-habilitar Actions en el fork con un runner propio (el contenedor
   upstream `ghcr.io/koopthekoopa/wii-ipl-build` es privado).

**Reglas de integridad** (de `CONTRIBUTING.md` y la práctica del proyecto):
- Solo `Object(Matching, ...)` cuando el objeto diffea al 100% en **las cuatro
  versiones** (o al menos no regresa ninguna); si no, `Equivalent`/`NonMatching`.
- Símbolos nuevos: entrada en `config/<43U,43E,43J,43K>/symbols.txt` con
  direcciones **del binario original** (nunca inventadas); splits solo si se
  re-splitea con datos reales.
- Nada de IA en contribuciones destinadas a upstream (este trabajo vive en el
  fork; para proponer algo upstream se requiere revisión humana total).
- Estilo: hex mayúsculas, `NULL` para punteros, llaves siempre, `clang-format`,
  guardas de header por librería.

---

## 3. Fases (con presupuesto de bytes y hitos de %)

Cada fase cierra unidades enteras; el orden maximiza bytes por esfuerzo
(ROI = % cercano a 100 · tamaño · reutilización externa).

### Fase 1 — Rematar lo casi-hecho (≈ +1.0 pts → **60%**)

Objetivo: `CHANSVm.c` completo (`Matching`).

- Las funciones al 92–99.98% listadas en el report del PR #38: `CHANSVmInit`
  (99.98%), `VmDateCommon` (99.88%), `VmBlobCalcRangeMD5Digest` (99.79%),
  `vmBlobParsePackFormatString` (99.70%), `VmPushFuncReturnInfo` (99.89%),
  `CHANSVmAddExe` (99.11%), `VmCallMethod` (96.57%), `VmStringSplit` (96.05%),
  `VmBlobUnpack` (92.08%), `VmBlobPackCommon` (92.60%), `CHANSVmStep` (94.79%)…
  Suma ≈ 26 KiB → convierte los 53.5 KiB de la unidad.
- Los "99.x%" casi siempre son 1–2 instrucciones fuera de orden o un registro
  distinto: objdiff muestra el diff exacto; suelen caer en minutos por función.
- Cierre: marcar `Object(Matching, "channelScript/CHANSVm.c")`.

### Fase 2 — Cripto/red de RevoEX y retales SDK (≈ +1.0 pt → **61%**)

- **Verificar** `libs/RevoEX/src/net/md5.c`, `sha1.c`, `hmac.c` (escritos en
  esta sesión, funcionalmente verificados pero **sin diff**): corregir contra
  objdiff (`k$2351` de md5, `sha1template` 0x20, layout HMAC 0xD4) y pasar
  `net/md5.c`, `net/sha1.c`, `net/hmac.c` a `Matching` (8.5 KiB).
- ~~Escribir los TUs faltantes de `net`~~ — **hecho (sin diff): `aes.c`
  (API NETAES* completa + tablas, verificado FIPS 197/SP 800-38A) y
  `nettime.c` (NETGetUniversalCalendar). Falta `neterrorcode.c`** y la
  verificación objdiff de todo el bloque `net` (commit `4c148a4c`).
- `AXFXReverbStdExp.c` (5.8 KiB, único axfx sin match; el PR #43 dejó el patrón
  listo en los archivos hermanos).

### Fase 3 — Librerías SDK RVL grandes (≈ +4.0 pts → **65%**)

Orden sugerido: `fa` (164.4 KiB, 12 TUs) → `wad.c` (24.5) → `WPADHIDParser.c`
(21.7) → `vi.c` (20.7) → `nup.cpp` (13.6) → `KPAD.c` (13.1) → `kbd/sdi/kpr`
(13.4).

- `fa` (FAT/VFAT para SD/NAND/USB) es el mayor bloque suelto del proyecto
  (5.5% del código total) y es **código SDK compartido**: ogws/open_rvl y otros
  decomps tienen versiones cercanas para cotejar estructura (adaptando al
  build CW de la IPL, nunca copiando a ciegas).
- ~~`fa`~~ — **avance (sin diff)**: núcleo portado (29/174 TUs) desde
  ogws (CC0, tamaños cotejados por símbolo) + el VF matcheado del propio
  repo; headers privados creados (`include/private/fa`). Pendiente:
  `api/FA*`, `pf_w_*`, `pfs_*`, `pf_stub*`, drivers, `msc`, `kernel/pfk_api`
  y los fatfs grandes (`pf_file`, `pf_dir`, `pf_fat`, `pf_volume`,
  `pf_cache`, `pf_entry`, `pf_entry_iterator`, `pf_path`, `pf_cluster`,
  `pf_fat12/16/32`, `pdm_*`). Ver commits `a77d16e8`.
- Cada TU se marca `Matching` individualmente; la librería completa al final.

### Fase 4 — eZiText + teclado (≈ +2.5 pts → **67.5%**)

- `zi8clib` (159.1 KiB): clib C del middleware; los `zi8*.c` comparten tablas y
  helpers — atacar por clusters (`zi8cgetc/zi8getc/zi8getc2`, `zi8alpha/zi8alts`,
  `zi8cinfo/zi8initd`…).
- `keyboard` (211.8 KiB): escenas C++ (`tiInputForm`, `tiPcKeyboard`,
  `tiCandidateBox`, `MyTiManager`, `tiCellPhone`, `MyTiInputForm`). Empezar por
  las clases auxiliares pequeñas y luego los formularios grandes.

### Fase 5 — Escenas de la IPL (≈ +2.5 pts → **70%**) ✅ META

Con las fases 1–4 la barrera del 70% queda a un par de unidades de distancia.
Cerrar la brecha con la selección más rentable de:

- `iplSetting.cpp` (37.9), `ATERM.c` (19.2), `AOSS.c` (16.2)
- `iplAddress.cpp` (24.0) + `iplAddressEdit.cpp` (27.1)
- `iplSDChannelSelect.cpp` (33.8) + `iplSDMemory.cpp` (20.9) + `iplSDChannelTitle.cpp` (18.6)
- `iplGCSaveData.cpp`/`iplMemoryCard*` (30.3, continuando el PR #41)
- `BS2Mach.c` (17.0), `odh.cpp` (14.7), `iplESMisc.cpp` (11.2)

### Fase 6 — Sostenibilidad post-70% (opcional)

- `nwc24` (62.6), `wd/WDScan` (48.6), `nhttp` (30.0), `cdb` (28.6), `vf` dev
  (28.3), `so` (11.1) → camino hacia el 80%.
- Convertir `Equivalent` → `Matching` (359 KiB) cuando se resuelva el detalle
  que los separa (muchos son `# hmm` históricos).

---

## 4. Hitos de verificación (gate de cada fase)

| Hito | Métrica a verificar | Fuente |
|---|---|---|
| Arranque | 59.2% confirmado | `report.json` / decomp.dev |
| Fase 1 | ≥60.0% y CHANSVm `Matching` | objdiff + report |
| Fase 2 | ≥61.0% y `net/*` `Matching` | objdiff + report |
| Fase 3 | ≥65.0% y `fa` completa | objdiff + report |
| Fase 4 | ≥67.5% y `zi8clib` completa | objdiff + report |
| **Fase 5** | **≥70.0% en 43U** | **decomp.dev badge** |
| Cierre | símbolos/splits sincronizados 43E/J/K, sin regresiones | `ninja` 4 versiones |

Reglas de gate:
- No se pasa de fase si el % de la fase anterior no está verificado con un
  build limpio de las 4 versiones.
- Toda regresión detectada revierte el commit causante antes de continuar.
- Actualizar `LLM_CONTEXT.md` (sección de progreso) al cerrar cada hito.

---

## 5. Riesgos y mitigaciones

| Riesgo | Mitigación |
|---|---|
| Sin binario original → no se puede diffear | Fase 0 es prerrequisito; nada se marca `Matching` sin diff |
| Sandbox sin CI ni toolchain | Verificación en máquina con acceso a `files.decomp.dev`; commits de trabajo se suben igual pero se anotan como "sin verificar" |
| Política anti-IA del upstream | Trabajo confinado al fork; revisión humana antes de cualquier PR a `koopthekoopa/wii-ipl` |
| SEL no se genera en el build (código no 100% shiftable) | No bloquea matching por-TU; documentar desvíos con `// FAKEMATCH:` |
| Divergencia entre versiones (43E/J/K) | Toda unidad nueva sincroniza símbolos en las 4 versiones antes de cerrarse |
| `Equivalent` tentador de marcar `Matching` | Prohibido: `Equivalent` solo pasa a `Matching` cuando objdiff da 100% en el objeto |

---

## 6. Rutina diaria de trabajo (por función)

1. Abrir la unidad objetivo en objdiff; elegir la función con peor score.
2. Leer el diff instrucción a instrucción; corregir orden/registros/constantes
   (convenciones de `CONTRIBUTING.md`).
3. Si la función queda al 100%, pasar a la siguiente; si la unidad entera queda
   al 100%, actualizar su `Object(...)` en `configure.py`.
4. `ninja progress build/43U/report.json` para verificar el movimiento de %.
5. Commit atómico por unidad (`autor/committer MilaSoraki`, sin otros trailers),
   push a `arena/01a02263-wii-ipl`.
6. Al cerrar una fase: actualizar `LLM_CONTEXT.md` y este documento.

## 7. Estimación gruesa de esfuerzo

| Fase | Bytes a matchear | Esfuerzo relativo |
|---|---|---|
| 1 (CHANSVm) | ~26 KiB parciales | días (diffs mínimos) |
| 2 (crypto/net) | ~15 KiB | días (spec + verificación) |
| 3 (SDK RVL) | ~270 KiB | semanas (cotejo cross-project) |
| 4 (IME) | ~370 KiB | semanas (clusters zi8 + escenas) |
| 5 (escenas) | ~70–100 KiB | 1–2 semanas |

Total: **~2.9 MB → 2.1 MB matcheado (70%)** alcanzable en el orden propuesto.
