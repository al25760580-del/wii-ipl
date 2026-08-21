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

#include <private/fa/PrFILE2/common/pf_code.h>
#include <private/fa/PrFILE2/local/pf_cp932.h>
#include <private/fa/PrFILE2/local/pf_cp932_to_unicode.h>

// PFCODE_CP932_OEM2Unicode - 0x815E455C, 444 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 PFCODE_CP932_OEM2Unicode(const pf_s8* cp932_src, pf_u16* uc_dst) {
    pf_u8 cp932_lead;
    pf_u8 cp932_trail;
    pf_s32 lead_index;
    pf_s32 trail_index;

    cp932_lead = (pf_u8)cp932_src[0];
    cp932_trail = (pf_u8)cp932_src[1];

    if (cp932_lead < 0x80) {
        *uc_dst = cp932_lead;
        return PFCODE_Combine_Width(1, 2);
    }

    if (cp932_lead >= 0xA1 && cp932_lead <= 0xDF) {
        *uc_dst = cp932_lead + 0xFEC0;
        return PFCODE_Combine_Width(1, 2);
    }

    if (cp932_lead == 0x80 || cp932_lead == 0x85 || cp932_lead == 0x86 || cp932_lead == 0xA0 || cp932_lead == 0xEB || cp932_lead == 0xEC ||
        (cp932_lead >= 0xEF && cp932_lead <= 0xF9) || (cp932_lead >= 0xFD && cp932_lead == 0xFF)) {
        *uc_dst = 0x5F;
        return PFCODE_Combine_Width(1, 2);
    }

    if (cp932_trail >= 0xFD) {
        *uc_dst = 0x5F;
        return PFCODE_Combine_Width(1, 2);
    }

    if ((cp932_lead >= 0x81) && (cp932_lead <= 0x84)) {
        lead_index = cp932_lead - 0x81;
    } else if ((cp932_lead >= 0x87) && (cp932_lead <= 0x9F)) {
        lead_index = cp932_lead - 0x83;
    } else if ((cp932_lead >= 0xE0) && (cp932_lead <= 0xEA)) {
        lead_index = cp932_lead - 0xC3;
    } else if ((cp932_lead >= 0xED) && (cp932_lead <= 0xEE)) {
        lead_index = cp932_lead - 0xC5;
    } else if ((cp932_lead >= 0xFA) && (cp932_lead <= 0xFC)) {
        lead_index = cp932_lead - 0xD0;
    } else {
        *uc_dst = 0x5F;
        return PFCODE_Combine_Width(1, 2);
    }

    trail_index = cp932_trail - 0x40;
    if (trail_index < 0 || trail_index >= 189) {
        *uc_dst = 0x5F;
        return PFCODE_Combine_Width(1, 2);
    }

    *uc_dst = cp932_to_unicode[lead_index][trail_index];
    if (*uc_dst == 0) {
        *uc_dst = 0x5F;
        return PFCODE_Combine_Width(1, 2);
    }

    return PFCODE_Combine_Width(2, 2);
}

// PFCODE_CP932_Unicode2OEM - 0x815E4718, 616 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 PFCODE_CP932_Unicode2OEM(const pf_u16* uc_src, pf_s8* cp932_dst) {
    pf_s32 i;
    pf_s32 j;
    pf_u16* p_table;
    pf_u8 uc_lead;
    pf_u8 uc_trail;
    pf_u16 uc;
    pf_u16 cp932;

    pf_u16 temp;  // Extra variable. Not in DWARF.

    uc_lead = *uc_src & 0xFF;
    uc_trail = *uc_src >> 8;

    if ((uc_lead < 0x80) && (uc_trail == 0)) {
        cp932_dst[0] = (pf_u8)uc_lead;
        cp932_dst[1] = 0;
        return PFCODE_Combine_Width(1, 2);
    }

    uc = (pf_u16)((uc_trail << 8) + uc_lead);
    if ((0xFF61 <= uc) && (uc <= 0xFF9F)) {
        temp = (pf_u16)(uc - 0xFEC0);
        cp932_dst[0] = (pf_u8)(temp);
        cp932_dst[1] = 0;
        return PFCODE_Combine_Width(1, 2);
    }

    if (uc == 0x5F) {
        cp932_dst[0] = 0x5F;
        return PFCODE_Combine_Width(1, 2);
    }

    i = 0;
    while (i < 45) {
        p_table = (pf_u16*)cp932_to_unicode[i];
        j = 0;
        while (j < 189) {
            if (*p_table == uc) {
                break;
            }
            j++;
            p_table++;
        }
        if (j < 189) {
            break;
        }
        i++;
    }

    if (j == 189) {
        cp932_dst[0] = 0x5F;
        return PFCODE_Combine_Width(1, 2);
    }

    if ((i >= 0) && (i <= 3)) {
        cp932 = ((i + 0x81) << 8) | (j + 0x40);
    } else if ((i >= 4) && (i <= 0x1C)) {
        cp932 = ((i + 0x83) << 8) | (j + 0x40);
    } else if ((i >= 0x1D) && (i <= 0x27)) {
        cp932 = ((i + 0xC3) << 8) | (j + 0x40);
    } else if ((i >= 0x28) && (i <= 0x29)) {
        cp932 = ((i + 0xC5) << 8) | (j + 0x40);
    } else if ((i >= 0x2A) && (i <= 0x2C)) {
        cp932 = ((i + 0xD0) << 8) | (j + 0x40);
    } else {
        cp932_dst[0] = 0x5F;
        return PFCODE_Combine_Width(1, 2);
    }

    cp932_dst[1] = (pf_u8)(cp932 & 0xFF);
    cp932_dst[0] = (cp932 >> 8) & 0xFF;
    return PFCODE_Combine_Width(2, 2);
}

// PFCODE_CP932_OEMCharWidth - 0x815E4980, 52 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 PFCODE_CP932_OEMCharWidth(const pf_s8* buf) {
    pf_u16 tmp;
    pf_s32 width;
    pf_s16 oem_width;
    pf_s16 uni_width;

    width = PFCODE_CP932_OEM2Unicode(buf, &tmp);
    PFCODE_Divide_Width(width, &oem_width, &uni_width);
    return oem_width;
}

// PFCODE_CP932_isOEMMBchar - 0x815E49B4, 124 bytes
// from ogws (Wii Sports VF, CC0)

pf_u32 PFCODE_CP932_isOEMMBchar(pf_s8 cp932, pf_u32 num) {
    pf_u8 code;
    pf_u32 is_mb;

    code = (pf_u8)cp932;
    is_mb = 0;

    switch (num) {
        case 1:
            is_mb = 0;
            if ((code >= 0x81 && code <= 0x9F) || (code >= 0xE0 && code <= 0xFC)) {
                is_mb = 1;
            }
            return is_mb;

        case 2:
            is_mb = 0;
            if ((code >= 0x40 && code <= 0x7E) || (code >= 0x80 && code <= 0xFC)) {
                is_mb = 1;
            }
            return is_mb;
    }
    return is_mb;
}

// PFCODE_CP932_UnicodeCharWidth - 0x815E4A30, 52 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 PFCODE_CP932_UnicodeCharWidth(const pf_u16* buf) {
    pf_s8 tmp[2];
    pf_s32 width;
    pf_s16 oem_width;
    pf_s16 uni_width;

    width = PFCODE_CP932_Unicode2OEM(buf, tmp);
    PFCODE_Divide_Width(width, &oem_width, &uni_width);

    return uni_width;
}

// PFCODE_CP932_isUnicodeMBchar - 0x815E4A64, 8 bytes
// from ogws (Wii Sports VF, CC0)

pf_u32 PFCODE_CP932_isUnicodeMBchar(pf_u16 uc_src, pf_u32 num) {
    return 0;
}
