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

#include <private/fa/PrFILE2/common/pf_str.h>
#include <private/fa/PrFILE2/fatfs/pf_file.h>
#include <private/fa/PrFILE2/standard/pf_api_util.h>
#include <private/fa/PrFILE2/standard/pf_remove.h>

// pf2_remove - 0x815E5994, 80 bytes
// from ogws (Wii Sports VF, CC0)

pf_int32 pf2_remove(const pf_ch8* path) {
    pf_s32 err;
    struct PF_STR path_str;

    err = PFSTR_InitStr(&path_str, (const pf_s8*)path, 1);
    if (err == 0) {
        err = PFFILE_remove(&path_str);
    } else {
        pf_vol_set.last_error = err;
    }

    return PFAPI_convertReturnValue(err);
}
