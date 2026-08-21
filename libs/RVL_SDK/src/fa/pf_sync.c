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

#include <private/fa/PrFILE2/standard/pf_sync.h>
#include <private/fa/PrFILE2/fatfs/pf_volume.h>
#include <private/fa/PrFILE2/standard/pf_api_util.h>

// pf2_sync - 0x815E5C6C, 40 bytes
// from RevoEX VF (this repo, Matching)

pf_int32 pf2_sync(pf_ch8 drv_char, pf_s32 mode) {
    pf_s32 err = PFVOL_sync(drv_char, mode);
    err = PFAPI_convertReturnValue(err);
    return err;
}
