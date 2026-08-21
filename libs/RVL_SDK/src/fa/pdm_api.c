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

#include <private/fa/PrFILE2/common/pf_clib.h>
#include <private/fa/PrFILE2/dskmng/pdm_disk.h>
#include <private/fa/PrFILE2/dskmng/pdm_dskmng.h>
#include <private/fa/PrFILE2/dskmng/pdm_partition.h>

// pdm_init_diskmanager - 0x815C7394, 4 bytes
// NOTE: size-diff(ogws=296) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_s32 pdm_init_diskmanager(pf_u32 config, void* param) {
    pf_u32 i;

    pf_memset(&pdm_disk_set, 0, 2916);
    for (i = 0; i < 0x1A; i++) {
        pdm_disk_set.disk[i].disk_lock_handle = 0;
    }

    for (i = 0; i < 0x1A; i++) {
        pdm_disk_set.partition[i].part_lock_handle = 0;
    }

    return 0;
}

// pdm_open_disk - 0x815C7398, 56 bytes
// NOTE: size-diff(ogws=72) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_s32 pdm_open_disk(struct PDM_INIT_DISK* p_init_disk_tbl, struct PDM_DISK** pp_disk) {
    pf_s32 err;

    if (!pp_disk) {
        return 1;
    }

    *pp_disk = 0;
    err = pdm_disk_open_disk(p_init_disk_tbl, pp_disk);

    if (!err) {
        return 0;
    }

    return err;
}

// pdm_close_disk - 0x815C73D0, 4 bytes
// NOTE: size-diff(ogws=24) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_s32 pdm_close_disk(struct PDM_DISK* p_disk) {
    pf_s32 err;

    if (!p_disk) {
        return 1;
    }

    err = pdm_disk_close_disk(p_disk);
    return err;
}

// pdm_open_partition - 0x815C73D4, 56 bytes
// NOTE: size-diff(ogws=92) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_s32 pdm_open_partition(struct PDM_DISK* p_disk, pf_s32 part_id, struct PDM_PARTITION** pp_part) {
    pf_s32 err;

    if (!p_disk) {
        return 1;
    }

    if (!pp_part) {
        return 1;
    }

    *pp_part = 0;
    part_id &= 0xFFFF;
    err = pdm_part_open_partition(p_disk, part_id, pp_part);
    if (!err) {
        return 0;
    }

    return err;
}

// pdm_close_partition - 0x815C740C, 48 bytes
// NOTE: size-diff(ogws=64) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_s32 pdm_close_partition(struct PDM_PARTITION* p_part) {
    pf_s32 err;

    if (!p_part) {
        return 1;
    }

    err = pdm_part_close_partition(p_part);
    if (!err) {
        return 0;
    }

    return err;
}
