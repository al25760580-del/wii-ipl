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

#include <private/fa/PrFILE2/pf_types.h>

// PFCODE_Combine_Width - 0x815C63D8, 12 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 PFCODE_Combine_Width(pf_s16 oem_width, pf_s16 uni_width) {
    return (oem_width << 16) + uni_width;
}

// PFCODE_Divide_Width - 0x815C63E4, 16 bytes
// from ogws (Wii Sports VF, CC0)

void PFCODE_Divide_Width(pf_s32 width, pf_s16* oem_width, pf_s16* uni_width) {
    *oem_width = (pf_u32)width >> 16;
    *uni_width = width;
}
