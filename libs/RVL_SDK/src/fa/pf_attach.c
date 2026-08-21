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

#include <private/fa/PrFILE2/fatfs/pf_volume.h>
#include <private/fa/PrFILE2/standard/pf_api_util.h>
#include <private/fa/PrFILE2/standard/pf_attach.h>

// pf2_attach - 0x815E4C1C, 176 bytes
// NOTE: size-diff(ogws=132) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_int32 pf2_attach(PF_DRV_TBL** drv_tbl) {
    pf_s32 err;
    if (drv_tbl == 0 || !*drv_tbl) {
        err = 10;
        pf_vol_set.last_error = err;
    } else {
        while (*drv_tbl) {
            err = PFVOL_attach(*drv_tbl);
            if (err != 0) {
                return PFAPI_convertReturnValue(err);
            }
            drv_tbl++;
        }
    }
    return PFAPI_convertReturnValue(err);
}
