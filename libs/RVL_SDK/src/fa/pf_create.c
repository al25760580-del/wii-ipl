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

#include <private/fa/PrFILE2/standard/pf_create.h>
#include <private/fa/PrFILE2/fatfs/pf_volume.h>
#include <private/fa/PrFILE2/standard/pf_api_util.h>

// pf2_create - 0x815E4FA0, 92 bytes
// NOTE: size-diff(ipl-vf=100) - verify against objdiff
// from RevoEX VF (this repo, Matching)

PF_FILE* pf2_create(const pf_ch8* path, pf_int32 mode) {
    pf_u32 open_mode_create;
    PF_FILE* p_file;
    PF_STR path_str;
    pf_s32 err;

    (void)mode;

    open_mode_create = 0x19;

    err = PFSTR_InitStr(&path_str, (pf_s8*)path, 1U);
    if (err == 0) {
        err = PFFILE_fopen(&path_str, open_mode_create, &p_file);
    } else {
        pf_vol_set.last_error = err;
        p_file = PF_NULL;
    }
    p_file = PFAPI_convertReturnValue2NULL(err, p_file);

    return p_file;
}
