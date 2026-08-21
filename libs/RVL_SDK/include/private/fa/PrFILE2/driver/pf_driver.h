#ifndef PRFILE2_VFMOD_PF_DRIVER_H
#define PRFILE2_VFMOD_PF_DRIVER_H

#include <private/fa/PrFILE2/fatfs/pf_cache.h>

#include <private/fa/PrFILE2/pf_types.h>

typedef struct PF_DRV_TBL {
    void* p_part;             // 0x00
    PF_CACHE_SETTING* cache;  // 0x04
    pf_s8 drive;              // 0x08
    pf_u8 stat;               // 0x09
} PF_DRV_TBL;

pf_s32 PFDRV_GetFSINFOInformation(PF_VOLUME* p_vol);
pf_s32 PFDRV_StoreFreeCountToFSINFO(PF_VOLUME* p_vol);
pf_bool PFDRV_IsInserted(PF_VOLUME* p_vol);
pf_bool PFDRV_IsDetected(PF_VOLUME* p_vol);
pf_bool PFDRV_IsWProtected(PF_VOLUME* p_vol);
pf_s32 PFDRV_init(PF_VOLUME* p_vol);
pf_s32 PFDRV_finalize(PF_VOLUME* p_vol);
pf_s32 PFDRV_mount(PF_VOLUME* p_vol);
pf_s32 PFDRV_unmount(PF_VOLUME* p_vol, pf_u32 mode);
pf_s32 PFDRV_format(PF_VOLUME* p_vol, const pf_u8* param);
pf_s32 PFDRV_lread(PF_VOLUME* p_vol, pf_u8* buf, pf_u32 sector, pf_u32 num_sectors, pf_u32* p_num_success);
pf_s32 PFDRV_lwrite(PF_VOLUME* p_vol, const pf_u8* buf, pf_u32 sector, pf_u32 num_sectors, pf_u32* p_num_success);
pf_s32 PFDRV_lerase(PF_VOLUME* p_vol, pf_u32 sector, pf_u32 num_sectors);

#endif  // PRFILE2_VFMOD_PF_DRIVER_H
