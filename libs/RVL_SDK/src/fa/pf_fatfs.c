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

#include <private/fa/PrFILE2/fatfs/pf_fatfs.h>
#include <private/fa/PrFILE2/fatfs/pf_volume.h>

// PFFATFS_initializeFATFS - 0x815D8374, 4 bytes (hand-written)
pf_s32 PFFATFS_initializeFATFS(pf_u32 config, void* param) {
    // @bug 4-byte stub in this build: returns without producing a result.
#pragma unused(config, param)
}
