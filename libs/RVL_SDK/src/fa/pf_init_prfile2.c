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

#include <private/fa/PrFILE2/standard/pf_init_prfile2.h>
#include <private/fa/PrFILE2/fatfs/pf_fatfs.h>
#include <private/fa/PrFILE2/standard/pf_api_util.h>
#include <private/fa/PrFILE2/system/pf_system.h>

// pf2_init_prfile2 - 0x815E56B4, 64 bytes
// from ogws (Wii Sports VF, CC0)

pf_int32 pf2_init_prfile2(pf_s32 config, void* param) {
    pf_s32 err;
    err = PFFATFS_initializeFATFS(config, param);
    if (err == 0) {
        PFSYS_initializeSYS();
    }
    return PFAPI_convertReturnValue(err);
}
