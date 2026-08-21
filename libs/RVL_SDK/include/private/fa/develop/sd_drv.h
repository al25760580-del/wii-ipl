#ifndef VF_SD_DRIVER_H
#define VF_SD_DRIVER_H

#include <private/fa/PrFILE2/dskmng/pdm_disk.h>

#include <revolution/nand.h>

void _InitSDWrok();
s32 _sddrv_init_drv_tbl(PDM_DISK_TBL* p_disk_tbl, u32 ui_ext);

#endif  // VF_SD_DRIVER_H
