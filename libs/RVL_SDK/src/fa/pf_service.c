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

// PF_GET_LE_U16 - 0x815C63F4, 16 bytes
// from ogws (Wii Sports VF, CC0)

pf_u16 PF_GET_LE_U16(const pf_u8* buf) {
    return buf[0] | buf[1] << 8;
}

// PF_LE16_TO_U16_STR - 0x815C6404, 296 bytes
// NOTE: size-diff(ogws=280) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

void PF_LE16_TO_U16_STR(pf_u8* sSrc, pf_u32 num) {
    pf_u32 cnt;
    pf_s8 tmp_src;

    for (cnt = 0; cnt < num; cnt += sizeof(pf_u16)) {
        tmp_src = sSrc[cnt];
        sSrc[cnt] = sSrc[cnt + 1];
        sSrc[cnt + 1] = tmp_src;
    }
}
