// Decompiled from the RVL_SDK "fa" library (File Allocation / PrFILE2 FAT).
//
// Reconstructed by porting the matching sibling implementation of the same
// pf code: the RevoEX VF library in this repo (Matching) and the Wii Sports
// VF decompilation (doldecomp/ogws, CC0), whose VFipf_* symbols were
// cross-checked against the fa symbols of this binary by size.
//
// NOT YET VERIFIED AGAINST THE ORIGINAL BINARY (no objdiff in this fork).
// Differences flagged below must be re-checked with objdiff; this unit is
// kept NonMatching until then.

#include <private/fa/PrFILE2/fatfs/pf_cache.h>
#include <private/fa/PrFILE2/fatfs/pf_sector.h>
#include <private/fa/PrFILE2/common/pf_clib.h>
#include <private/fa/PrFILE2/driver/pf_driver.h>
#include <private/fa/PrFILE2/fatfs/pf_volume.h>

// PFSEC_ReadFAT - 0x815DF774, 196 bytes
// NOTE: size-diff(ipl-vf=184) - verify against objdiff
// from RevoEX VF (this repo, Matching)

pf_s32 PFSEC_ReadFAT(PF_VOLUME* p_vol, pf_u8* p_buf, pf_u32 sector, pf_u16 offset, pf_u16 size) {
    PF_CACHE_PAGE* p_page;
    pf_s32 err;

    if (p_vol == PF_NULL) {
        return 10;
    }
    if (p_buf == PF_NULL) {
        return 10;
    }
    if ((p_vol->flags & 0x01) == 0 || PFDRV_IsInserted(p_vol) == PF_FALSE) {
        return 9;
    }
    err = PFCACHE_ReadFATPage(p_vol, sector, &p_page);
    if (err != 0) {
        return err;
    }
    pf_memcpy(p_buf, &p_page->p_buf[offset], size);
    return 0;
}

// PFSEC_ReadData - 0x815DF838, 756 bytes
// NOTE: size-diff(ipl-vf=236) - verify against objdiff
// from RevoEX VF (this repo, Matching)

pf_s32 PFSEC_ReadData(PF_VOLUME* p_vol, pf_u8* p_buf, pf_u32 sector, pf_u16 offset, pf_u32 size, pf_u32* p_success_size, pf_u32 set_sig) {
    PF_CACHE_PAGE* p_page;
    pf_u32 num_success;
    pf_u32 num_sector;
    pf_u32 adjust_sector;
    pf_u32 rest_sector;
    pf_s32 err;

    if (p_vol == PF_NULL || p_buf == PF_NULL || p_success_size == PF_NULL) {
        return 10;
    }
    if (size == 0 || size >= p_vol->bpb.bytes_per_sector) {
        return 10;
    }
    *p_success_size = 0;
    if ((p_vol->flags & 0x01) == 0 || PFDRV_IsInserted(p_vol) == PF_FALSE) {
        return 9;
    }
    err = PFCACHE_ReadDataPage(p_vol, sector, &p_page, set_sig);
    if (err != 0) {
        return err;
    }
    pf_memcpy(p_buf, &p_page->p_buf[offset], size);
    *p_success_size = size;

    return 0;
}

// PFSEC_WriteFAT - 0x815DFB2C, 232 bytes
// NOTE: size-diff(ipl-vf=240) - verify against objdiff
// from RevoEX VF (this repo, Matching)

pf_s32 PFSEC_WriteFAT(PF_VOLUME* p_vol, const pf_u8* p_buf, pf_u32 sector, pf_u16 offset, pf_u16 size) {
    PF_CACHE_PAGE* p_page;
    pf_s32 err;

    if (p_vol == PF_NULL) {
        return 10;
    }
    if (p_buf == PF_NULL) {
        return 10;
    }
    if ((p_vol->flags & 0x01) == 0 || !PFDRV_IsInserted(p_vol) || PFDRV_IsWProtected(p_vol)) {
        return 9;
    }
    err = PFCACHE_ReadFATPage(p_vol, sector, &p_page);
    if (err != 0) {
        return err;
    }
    pf_memcpy(&p_page->p_buf[offset], (pf_u8*)p_buf, size);
    PFCACHE_UpdateModifiedSector(p_vol, p_page, 1);
    err = PFCACHE_WriteFATPage(p_vol, p_page);
    if (err != 0) {
        return err;
    }
    return 0;
}

// PFSEC_WriteData - 0x815DFC14, 648 bytes
// NOTE: size-diff(ipl-vf=328) - verify against objdiff
// from RevoEX VF (this repo, Matching)

pf_s32 PFSEC_WriteData(PF_VOLUME* p_vol, const pf_u8* p_buf, pf_u32 sector, pf_u16 offset, pf_u32 size, pf_u32* p_success_size, pf_bool is_direct,
                          pf_bool set_sig) {
    pf_s32 err;
    PF_CACHE_PAGE* p_page;

    if (p_vol == PF_NULL || p_buf == PF_NULL || p_success_size == PF_NULL) {
        return 10;
    }
    if (size == 0 || size >= p_vol->bpb.bytes_per_sector) {
        return 10;
    }
    *p_success_size = 0;
    if ((p_vol->flags & 0x01) == 0 || !PFDRV_IsInserted(p_vol) || PFDRV_IsWProtected(p_vol)) {
        return 9;
    }
    if (is_direct != 0) {
        err = PFCACHE_AllocateDataPage(p_vol, sector, &p_page);
    } else {
        err = PFCACHE_ReadDataPageAndFlushIfNeeded(p_vol, sector, &p_page, set_sig);
    }
    if (err != 0) {
        return err;
    }
    pf_memcpy(&p_page->p_buf[offset], (void*)p_buf, size);
    PFCACHE_UpdateModifiedSector(p_vol, p_page, 1U);
    err = PFCACHE_WriteDataPage(p_vol, p_page, set_sig);
    if (err != 0) {
        return err;
    }
    *p_success_size = size;
    return 0;
}
