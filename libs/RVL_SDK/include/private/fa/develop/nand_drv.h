#ifndef VF_NAND_DRIVER_H
#define VF_NAND_DRIVER_H

#include <private/fa/PrFILE2/dskmng/pdm_disk.h>

#include <revolution/nand.h>

extern int VF_nand_retry_max;
extern int VF_nand_sleep_msec;

s32 _NandCreate(const char* path, u8 perm, u8 attr);
s32 _NandPrivateCreate(const char* path, u8 perm, u8 attr);
s32 _NandDelete(const char* path);
s32 _NandPrivateDelete(const char* path);
s32 _NandClose(NANDFileInfo* info);
s32 _NandOpen(const char* path, NANDFileInfo* info, u8 accType);
s32 _NandPrivateOpen(const char* path, NANDFileInfo* info, u8 accType);
s32 _NandWrite(NANDFileInfo* info, void* buf, u32 length);
s32 _NandRead(NANDFileInfo* info, void* buf, u32 length);
s32 _NandCreateDir(const char* path, u8 perm, u8 attr);
s32 _NandPrivateCreateDir(const char* path, u8 perm, u8 attr);
s32 _NandSeek(NANDFileInfo* info, s32 offset, s32 whence);
s32 _NandGetLength(NANDFileInfo* info, u32* length);
s32 _NandCreateSp(const char* path, u8 perm, u8 attr, u32 handleIdx);
s32 _NandOpenSp(const char* path, void* info, u8 accType, u32 handleIdx);
s32 _NandDeleteSp(const char* path, u32 handleIdx);
void _NandSetNANDFuncNormal(u32 handleIdx);
void _NandSetNANDFuncEx(u32 handleIdx);

s32 NAND_CreatePrfFileEx(u32 fileSize, char* fullpath, u16 version);
s32 _NandFlushNANDFromHandleIdx(s32 i_handleIdx, int i_setLastDeviceError);

s32 _NandFlushNANDFromHandleIdx(s32 handleIdx, int setLastDeviceError);

s32 _nanddrv_init_drv_tbl(PDM_DISK_TBL* p_disk_tbl, u32 uext);

#endif  // VF_NAND_DRIVER_H
