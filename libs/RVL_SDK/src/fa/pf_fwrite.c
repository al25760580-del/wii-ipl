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
#include <private/fa/PrFILE2/standard/pf_fwrite.h>

// pf2_fwrite - 0x815E5614, 40 bytes
// from ogws (Wii Sports VF, CC0)

pf_u32 pf2_fwrite(void* p_buf, pf_u32 size, pf_u32 count, struct PF_FILE* p_file) {
    pf_u32 count_written;
    PFFILE_fwrite(p_buf, size, count, p_file, &count_written);
    return count_written;
}
