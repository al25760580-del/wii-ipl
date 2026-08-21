# Auditoría de tamaños de función (43U)

> Generado con `python tools/size_check.py --all` el 2026-08-21, con la
> toolchain real de CodeWarrior (`GC/3.0a5.2`, tag `20250812`) y sin el
> binario original.

## Qué mide

`config/43U/symbols.txt` guarda la dirección **y el tamaño** de cada función
del `main.dol` original. Esa información vive en el repositorio, así que
aunque no haya WAD (y por tanto no haya split ni objdiff) sí se puede
compilar cada unidad de traducción con el compilador real y comparar el
tamaño de cada función generada con el tamaño de la original.

- **Tamaño distinto ⇒ la función no matchea** (prueba negativa, definitiva).
- **Tamaño igual ⇒ puede matchear** (condición necesaria, no suficiente).

Es el bucle de verificación que faltaba en la Fase 0 del plan mientras no
haya binario: no sustituye a objdiff, pero ordena el trabajo y detecta al
instante los puertos que están lejos.

## Fiabilidad de la herramienta

Pasada completa, **incluyendo** los objetos ya marcados `Matching` (que por
definición deberían dar tamaños idénticos):

```
totals: 786 units checked, 712 size-clean, 61 with differences, 13 compile errors
```

Sólo **2** unidades ya verificadas como `Matching` reportan diferencia
(`libs/NW4R/src/lyt/lyt_textBox.cpp` y `libs/RVLFaceLib/src/RFL_Controller.c`),
es decir ~99.7% de precisión sobre código conocido-bueno. Ambas se explican
por la limitación conocida de la herramienta: los símbolos se buscan por
nombre en todo el binario, así que un `static` con un nombre repetido en otra
unidad puede tomar el tamaño equivocado.

Las 13 unidades que "no compilan standalone" no son errores de código: usan
cabeceras precompiladas o el envoltorio SJIS que sólo existen en el build
completo.

## Resultado sobre las unidades sin verificar (no `Matching`)

```
totals: 136 units checked, 73 size-clean, 59 with differences, 4 compile errors
```

### Unidades cuyas funciones ya tienen todas el tamaño original

Candidatas directas a objdiff: si alguna diffea al 100%, pasa a `Matching`
sin más trabajo.

- `src/system/iplChannelManager.cpp`  (system, 74 functions)
- `src/system/iplException.cpp`  (system, 6 functions)
- `src/system/RsoSystem.cpp`  (system, 25 functions)
- `src/system/iplCdbBackup.cpp`  (system, 26 functions)
- `src/system/odh.cpp`  (system, 6 functions)
- `src/utility/iplGraphics.cpp`  (utility, 12 functions)
- `src/utility/iplESMisc.cpp`  (utility, 20 functions)
- `src/iplwww/www_browser.cpp`  (iplwww, 33 functions)
- `src/iplwww/www_surface.cpp`  (iplwww, 22 functions)
- `src/iplwww/www_window.cpp`  (iplwww, 18 functions)
- `src/iplwww/www_wiisetting.cpp`  (iplwww, 21 functions)
- `src/homebutton/HBMBase.cpp`  (homebutton, 84 functions)
- `src/BS2/BS2Mach.c`  (BS2, 4 functions)
- `src/scene/address/iplAddressEdit.cpp`  (address, 1 functions)
- `src/scene/button/iplButton.cpp`  (button, 73 functions)
- `src/scene/calendar/iplCalendar.cpp`  (calendar, 64 functions)
- `src/scene/calendar/iplDate.cpp`  (calendar, 40 functions)
- `src/scene/channelEdit/iplChanAppBase.cpp`  (channelEdit, 39 functions)
- `src/scene/channelEdit/iplChanAppBox.cpp`  (channelEdit, 33 functions)
- `src/scene/channelEdit/iplChanAppEdit.cpp`  (channelEdit, 56 functions)
- `src/scene/kitayamaTest/iplKitayamaTest.cpp`  (kitayamaTest, 23 functions)
- `src/scene/memoryCard/iplMemoryCard.cpp`  (memoryCard, 0 functions)
- `src/scene/memoryCard/iplGCWindow.cpp`  (memoryCard, 1 functions)
- `src/scene/setting/iplAPScanThread.cpp`  (setting, 5 functions)
- `src/scene/sceneSystem/iplSceneManager.cpp`  (sceneSystem, 39 functions)
- `src/scene/textWriter/iplTextWriter.cpp`  (textWriter, 60 functions)
- `src/bannerSound/AudioWavePlayer.cpp`  (bannerSound, 23 functions)
- `src/bannerSound/AxAdpcmPlayer.cpp`  (bannerSound, 15 functions)
- `src/keyboard/tiToolBar.cpp`  (keyboard, 54 functions)
- `src/keyboard/tiPredictLang.cpp`  (keyboard, 46 functions)
- `src/channelScript/systemmenu/iplCSPane.cpp`  (channelScript, 43 functions)
- `src/channelScript/systemmenu/iplCSAnim.cpp`  (channelScript, 21 functions)
- `libs/RevoEX/src/cdb/CDBConv.c`  (cdb, 39 functions)
- `libs/RevoEX/src/cdb/CDBFileSystemUtils.c`  (cdb, 11 functions)
- `libs/RevoEX/src/cdb/CDBRecordKey.c`  (cdb, 9 functions)
- `libs/RevoEX/src/cdb/CDBSystem.c`  (cdb, 27 functions)
- `libs/RevoEX/src/cdb/CDBAttr.c`  (cdb, 16 functions)
- `libs/RevoEX/src/nwc24/NWC24StdAPI.c`  (nwc24, 3 functions)
- `libs/RevoEX/src/nwc24/NWC24Manage.c`  (nwc24, 14 functions)
- `libs/RevoEX/src/nwc24/NWC24MsgObj.c`  (nwc24, 3 functions)
- `libs/RevoEX/src/nwc24/NWC24UserId.c`  (nwc24, 0 functions)
- `libs/RevoEX/src/nwc24/NWC24CHJump.c`  (nwc24, 4 functions)
- `libs/RVLMiddleware/TMC_JPEG/src/buffer/getcode.c`  (TMC_JPEG, 3 functions)
- `libs/RVLMiddleware/TMC_JPEG/src/reschange/iqdec_resolution_change_a3.c`  (TMC_JPEG, 2 functions)
- `libs/NW4R/src/ut/ut_ArchiveFontBase.cpp`  (ut, 23 functions)
- `libs/NW4R/src/ut/ut_ArchiveFont.cpp`  (ut, 7 functions)
- `libs/NW4R/src/lyt/lyt_window.cpp`  (lyt, 23 functions)
- `libs/RVL_SDK/src/axfx/AXFXReverbStdExp.c`  (axfx, 9 functions)
- `libs/RVL_SDK/src/axfx/AXFXChorusExp.c`  (axfx, 7 functions)
- `libs/RVL_SDK/src/fa/pf_code.c`  (fa, 2 functions)
- `libs/RVL_SDK/src/fa/pf_fatfs.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_cp932.c`  (fa, 6 functions)
- `libs/RVL_SDK/src/fa/pf_chdir.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_detach.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_errnum.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_fclose.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_finfo.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_fopen.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_format.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_fread.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_fseek.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_fsfirst.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_fsnext.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_fwrite.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_getdev.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_init_prfile2.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_mkdir.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_remove.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_rmdir.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_sync.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/fa/pf_unmount.c`  (fa, 1 functions)
- `libs/RVL_SDK/src/sdi/sdi_api.c`  (sdi, 11 functions)
- `libs/RVL_SDK/src/cntcache/cntcache.c`  (cntcache, 3 functions)

### Unidades con diferencias de tamaño

Aquí hay trabajo real pendiente; el detalle por función se obtiene con
`python tools/size_check.py <archivo>`.

- `src/system/rvl_dec.c`  (system) same=1 diff=3
- `src/system/iplSystem.cpp`  (system) same=61 diff=2
- `src/system/iplController.cpp`  (system) same=32 diff=3
- `src/system/iplNandShared.cpp`  (system) same=9 diff=1
- `src/system/iplPlayTimeLog.cpp`  (system) same=18 diff=1
- `src/system/iplNwc24Manager.cpp`  (system) same=52 diff=1
- `src/system/iplKeyboard.cpp`  (system) same=34 diff=2
- `src/system/iplSaveDataManager.cpp`  (system) same=27 diff=2
- `src/system/TVRC.cpp`  (system) same=7 diff=1
- `src/utility/iplUtility.cpp`  (utility) same=48 diff=1
- `src/sound/iplSound.cpp`  (sound) same=0 diff=1
- `src/scene/board/iplBoard.cpp`  (board) same=97 diff=10
- `src/scene/board/iplUrlProcessor.cpp`  (board) same=17 diff=1
- `src/scene/channelSelect/iplChannelSelect.cpp`  (channelSelect) same=141 diff=2
- `src/scene/channelTitle/iplChannelTitle.cpp`  (channelTitle) same=136 diff=3
- `src/scene/letterWriter/iplLetterWriter.cpp`  (letterWriter) same=43 diff=1
- `src/scene/memoryCard/iplGCSaveData.cpp`  (memoryCard) same=0 diff=2
- `src/keyboard/tiGUIManager.cpp`  (keyboard) same=55 diff=3
- `src/keyboard/tiHwKeyboard.cpp`  (keyboard) same=8 diff=1
- `src/channelScript/CHANSVm.c`  (channelScript) same=225 diff=8
- `libs/RVLMiddleware/eZiText/src/clib/zaddress.c`  (zi8clib) same=6 diff=1
- `libs/RVLMiddleware/eZiText/src/clib/zi8ZHuwd.c`  (zi8clib) same=0 diff=1
- `libs/RevoEX/src/cdb/CDBCrypt.c`  (cdb) same=1 diff=2
- `libs/RevoEX/src/net/nettime.c`  (net) same=0 diff=1
- `libs/RevoEX/src/net/md5.c`  (net) same=1 diff=3
- `libs/RevoEX/src/net/sha1.c`  (net) same=1 diff=4
- `libs/RevoEX/src/net/hmac.c`  (net) same=0 diff=3
- `libs/RevoEX/src/net/aes.c`  (net) same=1 diff=6
- `libs/RevoEX/src/nwc24/NWC24FriendList.c`  (nwc24) same=16 diff=1
- `libs/RevoEX/src/vf/develop/d_vf_sys.c`  (vf) same=53 diff=1
- `libs/RevoEX/src/vf/develop/nand_drv.c`  (vf) same=32 diff=1
- `libs/RevoEX/src/vf/develop/sd_drv.c`  (vf) same=9 diff=6
- `libs/RevoEX/src/wd/WDScan.c`  (wd) same=4 diff=1
- `libs/RVLMiddleware/TMC_JPEG/src/buffer/mkhdec3.c`  (TMC_JPEG) same=1 diff=1
- `libs/RVLMiddleware/TMC_JPEG/src/buffer/idct_block_var.c`  (TMC_JPEG) same=0 diff=2
- `libs/RVLMiddleware/TMC_JPEG/src/jpegdec/jdec_main.c`  (TMC_JPEG) same=8 diff=5
- `libs/RVLMiddleware/TMC_JPEG/src/exif/exif_parse.c`  (TMC_JPEG) same=1 diff=5
- `libs/RVLMiddleware/TMC_JPEG/src/reschange/idct_resolution_change_var.c`  (TMC_JPEG) same=5 diff=1
- `libs/RVLMiddleware/TMC_JPEG/src/texturecvtr/Texture_MCUtoY8U8V8.c`  (TMC_JPEG) same=0 diff=1
- `libs/RVLMiddleware/TMC_JPEG/src/texturecvtr/Texture_MCUtoRGB565.c`  (TMC_JPEG) same=0 diff=1
- `libs/RVLMiddleware/TMC_JPEG/src/texturecvtr/Texture_MCUtoRGBA8.c`  (TMC_JPEG) same=0 diff=13
- `libs/RVLMiddleware/TMC_JPEG/src/b65/iqdec_b65_frv32.c`  (TMC_JPEG) same=0 diff=1
- `libs/RVL_SDK/src/vi/i2c.c`  (vi) same=0 diff=2
- `libs/RVL_SDK/src/card/CARDBlock.c`  (card) same=3 diff=3
- `libs/RVL_SDK/src/wpad/WPAD.c`  (wpad) same=66 diff=1
- `libs/RVL_SDK/src/wpad/WPADHIDParser.c`  (wpad) same=8 diff=14
- `libs/RVL_SDK/src/fa/pf_clib.c`  (fa) same=9 diff=3
- `libs/RVL_SDK/src/fa/pf_service.c`  (fa) same=1 diff=1
- `libs/RVL_SDK/src/fa/pf_str.c`  (fa) same=10 diff=1
- `libs/RVL_SDK/src/fa/pdm_api.c`  (fa) same=0 diff=5
- `libs/RVL_SDK/src/fa/pf_path.c`  (fa) same=14 diff=13
- `libs/RVL_SDK/src/fa/pf_sector.c`  (fa) same=0 diff=4
- `libs/RVL_SDK/src/fa/pf_attach.c`  (fa) same=0 diff=1
- `libs/RVL_SDK/src/fa/pf_create.c`  (fa) same=0 diff=1
- `libs/RVL_SDK/src/fa/pf_system.c`  (fa) same=1 diff=2
- `libs/EGG/src/core/eggAudioArcPlayerMgr.cpp`  (EGG) same=17 diff=3
- `libs/EGG/src/core/eggAudioExpMgr.cpp`  (EGG) same=18 diff=4
- `libs/EGG/src/core/eggAudioMgr.cpp`  (EGG) same=19 diff=5
- `libs/MSL/src/MSL_Common/time.c`  (MSL_C.PPCEABI.bare.H) same=9 diff=1

## Cómo reproducir

```sh
python tools/size_check.py --all --summary          # todo lo no-Matching
python tools/size_check.py --all --include-matching # + calibración
python tools/size_check.py --preset net --show-ok   # una librería
python tools/size_check.py libs/RevoEX/src/net/md5.c
python tools/compile_check.py --preset fa           # sólo "¿compila?"
```

La primera ejecución descarga compiladores y binutils (`files.decomp.dev`,
releases de GitHub); no hace falta el binario original.
