#ifndef PRFILE2_VFMOD_PF_FAT32_H
#define PRFILE2_VFMOD_PF_FAT32_H

#include <private/fa/PrFILE2/fatfs/pf_cache.h>
#include <private/fa/PrFILE2/pf_types.h>

pf_s32 PFFAT32_ReadFATEntry(PF_VOLUME* p_vol, pf_u32 cluster, pf_u32* p_value);
pf_s32 PFFAT32_ReadFATEntryPage(PF_VOLUME* p_vol, pf_u32 cluster, pf_u32* p_value, PF_CACHE_PAGE** pp_page);
pf_s32 PFFAT32_WriteFATEntry(PF_VOLUME* p_vol, pf_u32 cluster, pf_u32 value);
pf_s32 PFFAT32_WriteFATEntryPage(PF_VOLUME* p_vol, pf_u32 cluster, pf_u32 value, PF_CACHE_PAGE** pp_page);

#endif  // PRFILE2_VFMOD_PF_FAT32_H
