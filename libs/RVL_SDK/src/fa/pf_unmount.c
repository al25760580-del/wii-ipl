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

#include <private/fa/PrFILE2/standard/pf_unmount.h>
#include <private/fa/PrFILE2/fatfs/pf_volume.h>
#include <private/fa/PrFILE2/standard/pf_api_util.h>

// pf2_unmount - 0x815E5CB8, 40 bytes
// from ogws (Wii Sports VF, CC0)

pf_int32 pf2_unmount(pf_ch8 drive, pf_u32 mode) {
    pf_s32 err;
    err = PFAPI_convertReturnValue4unmount(PFVOL_unmount(drive, mode));
    return err;
}
