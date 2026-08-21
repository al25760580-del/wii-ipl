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
#include <private/fa/PrFILE2/standard/pf_fopen.h>

// pf2_fopen - 0x815E53A0, 156 bytes
// from ogws (Wii Sports VF, CC0)

struct PF_FILE* pf2_fopen(const pf_ch8* path, const pf_ch8* mode) {
    pf_s32 open_mode;
    struct PF_FILE* p_file;
    struct PF_STR path_str;
    pf_s32 err;

    open_mode = PFAPI_ParseOpenModeString(mode);

    if (open_mode == 0) {
        pf_vol_set.last_error = 10;
        return PF_NULL;
    } else {
        err = PFSTR_InitStr(&path_str, (const pf_s8*)path, 1);
        if (err == 0) {
            err = PFFILE_fopen(&path_str, open_mode, &p_file);
        } else {
            pf_vol_set.last_error = err;
        }
    }
    return PFAPI_convertReturnValue2NULL(err, p_file);
}
