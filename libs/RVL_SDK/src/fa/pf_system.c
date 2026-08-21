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

#include <private/fa/PrFILE2/system/pf_system.h>
#include <private/fa/develop/d_time.h>

// PFSYS_initializeSYS - 0x815E6DE8, 4 bytes
// from ogws (Wii Sports VF, CC0)

void PFSYS_initializeSYS() {
    return;
}

// PFSYS_GetCurrentContextID - 0x815E6DEC, 24 bytes
// NOTE: size-diff(ogws=16) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_s32 PFSYS_GetCurrentContextID(pf_s32* context_id) {
    pf_s32 err;

    err = 0;
    *context_id = 0;
    return err;
}

// PFSYS_TimeStamp - 0x815E6E04, 116 bytes
// NOTE: size-diff(ogws=100) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

void PFSYS_TimeStamp(struct PF_SYS_DATE* sdate, struct PF_SYS_TIME* stime) {
    sdate->sys_year = 2000;
    sdate->sys_month = 4;
    sdate->sys_day = 1;
    stime->sys_hour = 0;
    stime->sys_min = 0;
    stime->sys_sec = 0;
    stime->sys_ms = 1;
    dTM_GetNowTime(sdate, stime);
    stime->sys_ms = 1;
}
