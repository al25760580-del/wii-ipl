#ifndef PRFILE2_VFMOD_PF_FAT_H
#define PRFILE2_VFMOD_PF_FAT_H

#include <private/fa/PrFILE2/common/pf_str.h>
#include <private/fa/PrFILE2/fatfs/pf_cache.h>
#include <private/fa/PrFILE2/pf_types.h>

typedef enum {
    FAT_12 = 0,
    FAT_16 = 1,
    FAT_32 = 2,
    FAT_ERR = -1,
} PF_FAT_TYPE;

#define FAT_MAX 3

typedef struct PF_LAST_CLUSTER {
    pf_u32 num_last_cluster;  // 0x00
    pf_u32 max_chain_index;   // 0x04
} PF_LAST_CLUSTER;

typedef struct PF_FAT_LAST_ACCESS {
    pf_u32 chain_index;  // 0x00
    pf_u32 cluster;      // 0x04
} PF_FAT_LAST_ACCESS;

typedef struct PF_CLUSTER_LINK {
    pf_u32* buffer;          // 0x00
    pf_u16 interval;         // 0x04
    pf_u16 interval_offset;  // 0x06
    pf_u32 position;         // 0x08
    pf_u32 max_count;        // 0x0C
    pf_u32 save_index;       // 0x10
} PF_CLUSTER_LINK;

typedef struct PF_FAT_HINT {
    pf_u32 chain_index;  // 0x00
    pf_u32 cluster;      // 0x04
} PF_FAT_HINT;

typedef struct PF_FFD {
    pf_u32 start_cluster;                    // 0x00
    pf_u32* p_start_cluster;                 // 0x04
    PF_LAST_CLUSTER last_cluster;            // 0x08
    PF_FAT_LAST_ACCESS last_access_cluster;  // 0x10
    PF_CLUSTER_LINK cluster_link;            // 0x18
    PF_FAT_HINT* p_hint;                     // 0x2C
    PF_VOLUME* p_vol;                        // 0x30
} PF_FFD;

typedef struct PF_CLUSTER_LINK_VOL {
    pf_u16 flag;      // 0x00
    pf_u16 interval;  // 0x02
    pf_u32* buffer;   // 0x04
    pf_u32 link_max;  // 0x08
} PF_CLUSTER_LINK_VOL;

pf_s32 PFFAT_UpdateFATEntry(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page);
pf_s32 PFFAT_UpdateAlternateFATEntry(PF_VOLUME* p_vol, pf_u8* p_buf, pf_u32 sector, pf_u32 size);
pf_s32 PFFAT_GetSectorSpecified(PF_FFD* p_ffd, pf_u32 file_sector_index, pf_u32 may_allocate, pf_u32* p_sector);
pf_s32 PFFAT_GetSectorAllocated(PF_FFD* p_ffd, pf_u32 file_sector_index, pf_u32 size, pf_u32* p_sector, pf_u32* p_num_sector);
pf_s32 PFFAT_GetContinuousSector(PF_FFD* p_ffd, pf_u32 file_sector_index, pf_u32 size, pf_u32* p_sector, pf_u32* p_num_sector);
pf_s32 PFFAT_CountAllocatedClusters(PF_FFD* p_ffd, pf_u32 size, pf_u32* p_num_alloc_clusters);
pf_s32 PFFAT_CountFreeClusters(PF_VOLUME* p_vol, pf_u32* p_num_free_clusters);
pf_s32 PFFAT_FreeChain(PF_FFD* p_ffd, pf_u32 start_cluster, pf_u32 chain_index, pf_u32 size);
pf_s32 PFFAT_GetBeforeChain(PF_VOLUME* p_vol, pf_u32 start_cluster, pf_u32 lActive, pf_u32* p_cluster);
pf_s32 PFFAT_WriteValueToSpecifiedCluster(PF_VOLUME* p_vol, pf_u32 cluster, pf_u32 value);
pf_s32 PFFAT_GetBeforeSector(pf_u32* p_befor_sector, PF_VOLUME* p_vo, pf_u32 current_sector);
pf_s32 PFFAT_InitFATRegion(PF_VOLUME* p_vol);
pf_s32 PFFAT_MakeRootDir(PF_VOLUME* p_vol);
pf_s32 PFFAT_RefreshFSINFO(PF_VOLUME* p_vol);
void PFFAT_InitHint(PF_FAT_HINT* p_hint);
void PFFAT_SetHint(PF_FFD* p_ffd, PF_FAT_HINT* p_hint);
pf_s32 PFFAT_TraceClustersChain(PF_FFD* p_ffd, pf_u32 start_clst, pf_u32 size, pf_u32* p_target_clst, pf_u32* p_next_clst);
pf_s32 PFFAT_ReadValueToSpecifiedCluster(PF_VOLUME* p_vol, pf_u32 cluster, pf_u32* value);
pf_s32 PFFAT_ResetFFD(PF_FFD* p_ffd, pf_u32* p_start_cluster);
pf_s32 PFFAT_InitFFD(PF_FFD* p_ffd, PF_FAT_HINT* p_hint, PF_VOLUME* p_vol, pf_u32* p_start_cluster);
pf_s32 PFFAT_FinalizeFFD(PF_FFD* p_ffd);
void PFFAT_SetLastAccess(PF_FFD* p_ffd, PF_FAT_HINT* last_access);
pf_u32 PFFAT_GetValueOfEOC2(PF_VOLUME* p_vol);

#endif  // PRFILE2_VFMOD_PF_FAT_H
