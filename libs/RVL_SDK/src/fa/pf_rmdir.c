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

#include <private/fa/PrFILE2/standard/pf_rmdir.h>
#include <private/fa/PrFILE2/fatfs/pf_dir.h>
#include <private/fa/PrFILE2/standard/pf_api_util.h>
#include <private/fa/PrFILE2/common/pf_str.h>

// pf2_rmdir - 0x815E5A88, 80 bytes
// from RevoEX VF (this repo, Matching)

pf_int32 pf2_rmdir(const pf_ch8* sPath) {
    pf_s32 err;
    PF_STR path_str;

    err = PFSTR_InitStr(&path_str, (pf_s8*)sPath, 1U);
    if (err == 0) {
        err = PFDIR_rmdir(&path_str);
    } else {
        pf_vol_set.last_error = err;
    }
    err = PFAPI_convertReturnValue(err);
    return err;
}
