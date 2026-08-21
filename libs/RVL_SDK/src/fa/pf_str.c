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
#include <private/fa/PrFILE2/common/pf_clib.h>
#include <private/fa/PrFILE2/common/pf_w_clib.h>
#include <private/fa/PrFILE2/fatfs/pf_volume.h>
#include <private/fa/PrFILE2/common/pf_str.h>

// PFSTR_SetCodeMode - 0x815C652C, 8 bytes
// from ogws (Wii Sports VF, CC0)

void PFSTR_SetCodeMode(struct PF_STR* p_str, pf_u32 code_mode) {
    p_str->code_mode = code_mode;
}

// PFSTR_GetCodeMode - 0x815C6534, 8 bytes
// from ogws (Wii Sports VF, CC0)

pf_u32 PFSTR_GetCodeMode(struct PF_STR* p_str) {
    return p_str->code_mode;
}

// PFSTR_SetLocalStr - 0x815C653C, 40 bytes
// from ogws (Wii Sports VF, CC0)

void PFSTR_SetLocalStr(struct PF_STR* p_str, pf_s8* p_local) {
    if (p_str->code_mode == PF_STR_CODEMODE_OEM || (p_local == PF_NULL)) {
        p_str->p_local = p_str->p_head;
    } else {
        p_str->p_local = p_local;
    }
}

// PFSTR_GetStrPos - 0x815C6564, 40 bytes
// from ogws (Wii Sports VF, CC0)

pf_s8* PFSTR_GetStrPos(struct PF_STR* p_str, pf_u32 target) {
    pf_s8* p_pos;
    if (target == 1) {
        p_pos = (pf_s8*)p_str->p_head;
    } else if (target == 2) {
        p_pos = (pf_s8*)p_str->p_tail;
    } else {
        p_pos = (pf_s8*)p_str->p_local;
    }

    return p_pos;
}

// PFSTR_MoveStrPos - 0x815C658C, 228 bytes
// from ogws (Wii Sports VF, CC0)

void PFSTR_MoveStrPos(struct PF_STR* p_str, pf_s16 num_char) {
    pf_s16 cnt;
    pf_s16 offset;
    pf_s32 width;
    pf_s8* p;
    pf_u16* wp;

    offset = 0;

    if (PFSTR_GetCodeMode(p_str) == PF_STR_CODEMODE_OEM) {
        p = (pf_s8*)p_str->p_head;

        for (; num_char != 0; offset++, num_char--) {
            if (pf_vol_set.codeset.is_oem_mb_char(*p, PF_STR_CODEMODE_OEM)) {
                offset++;
            }
        }
    } else {
        wp = (pf_u16*)p_str->p_head;

        for (cnt = 0; cnt < num_char; cnt++) {
            width = pf_vol_set.codeset.unicode_char_width((pf_u16*)wp);
            wp += width;
            offset += (pf_s16)width;
        }
    }

    p_str->p_head += offset;
}

// PFSTR_InitStr - 0x815C6670, 136 bytes
// NOTE: size-diff(ogws=172) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_s32 PFSTR_InitStr(struct PF_STR* p_str, const pf_s8* s, pf_u32 code_mode) {
    if (p_str == PF_NULL || s == PF_NULL) {
        return 10;
    }

    if (code_mode == PF_STR_CODEMODE_OEM) {
        p_str->p_head = s;
        p_str->p_tail = s + pf_strlen(s);
    } else if (code_mode == PF_STR_CODEMODE_UNICODE) {
        p_str->p_head = s;
        p_str->p_tail = s + (2 * pf_w_strlen((const pf_u16*)s));
    } else {
        return 10;
    }

    p_str->code_mode = code_mode;
    return 0;
}

// PFSTR_StrLen - 0x815C66F8, 20 bytes
// from ogws (Wii Sports VF, CC0)

pf_u16 PFSTR_StrLen(struct PF_STR* p_str) {
    return (pf_u16)((pf_u8*)p_str->p_tail - (pf_u8*)p_str->p_head);
}

// PFSTR_StrNumChar - 0x815C670C, 224 bytes
// from ogws (Wii Sports VF, CC0)

pf_u16 PFSTR_StrNumChar(struct PF_STR* p_str, pf_u32 target) {
    pf_s8* p;
    pf_u16 cnt;
    pf_s32 width;

    if (target == PF_STR_CODEMODE_OEM) {
        p = (pf_s8*)p_str->p_head;
    } else {
        p = (pf_s8*)p_str->p_tail;
    }

    if (p_str->code_mode == PF_STR_CODEMODE_OEM) {
        cnt = 0;
        for (; *p != '\0'; p++, cnt++) {
            if (pf_vol_set.codeset.is_oem_mb_char(*p, PF_STR_CODEMODE_OEM)) {
                p++;
            }
        }
    } else {
        cnt = 0;
        for (; p[0] != '\0' || p[1] != '\0'; p += width, cnt++) {
            width = pf_vol_set.codeset.unicode_char_width((pf_u16*)p);
        }
    }

    return cnt;
}

// PFSTR_StrCmp - 0x815C67EC, 164 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 PFSTR_StrCmp(const struct PF_STR* p_str, const pf_s8* s) {
    pf_u16 wc;
    const pf_u16* wp;
    const pf_s8* p1;  // Present in DWARF but unused here.
    const pf_s8* p2;  // Present in DWARF but unused here.
    pf_s32 ret;       // Present in DWARF but unused here.

    if (p_str->code_mode == PF_STR_CODEMODE_OEM) {
        return pf_strcmp(p_str->p_head, s);
    }

    wp = (const pf_u16*)p_str->p_head;
    do {
        pf_vol_set.codeset.oem2unicode((pf_s8*)s, &wc);
        s++;
    } while (*wp++ == wc && wp[-1] != L'\0' && wc != L'\0');

    return wp[-1] - wc;
}

// PFSTR_StrNCmp - 0x815C6890, 280 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 PFSTR_StrNCmp(struct PF_STR* p_str, const pf_s8* s, pf_u32 target, pf_s16 offset, pf_u16 num) {
    pf_u16 wc;
    const pf_u16* wp;
    const pf_s8* p1;
    const pf_s8* p2;  // Present in DWARF but unused here.
    pf_s32 ret;       // Present in DWARF but unused here.

    if (p_str->code_mode == PF_STR_CODEMODE_OEM || target == PF_STR_CODEMODE_LOCAL) {
        if (target == PF_STR_CODEMODE_OEM) {
            p1 = (const pf_s8*)p_str->p_head + offset;
        } else if (target == PF_STR_CODEMODE_UNICODE) {
            p1 = (const pf_s8*)p_str->p_tail + offset;
        } else {
            p1 = (const pf_s8*)p_str->p_local + offset;
        }

        return pf_strncmp(p1, s, num);
    }

    if (target == PF_STR_CODEMODE_OEM) {
        wp = (const pf_u16*)((pf_u8*)p_str->p_head + offset * sizeof(pf_u16));
    } else {
        wp = (const pf_u16*)((pf_u8*)p_str->p_tail + offset * sizeof(pf_u16));
    }

    do {
        pf_vol_set.codeset.oem2unicode((pf_s8*)s, &wc);
        s++;
        num--;
    } while (*wp++ == wc && num > 0 && wp[-1] != L'\0' && wc != L'\0');

    return wp[-1] - wc;
}

// PFSTR_ToUpperNStr - 0x815C69A8, 232 bytes
// from ogws (Wii Sports VF, CC0)

void PFSTR_ToUpperNStr(struct PF_STR* p_str, pf_u16 num, pf_s8* dest) {
    pf_u16 wc;
    const pf_u16* wp;
    const pf_s8* p;

    if (p_str->code_mode == PF_STR_CODEMODE_OEM) {
        for (p = p_str->p_head; num > 0 && *p != '\0'; p++, num--) {
            *dest++ = pf_toupper(*p);
        }
    } else {
        for (wp = (const pf_u16*)p_str->p_head; num > 0 && *wp != L'\0'; num--, wp++) {
            wc = (*wp >= L'a' && *wp <= L'z') ? *wp - (L'a' - L'A') : *wp;
            dest[0] = (wc >> 0) & 0xFF;
            dest[1] = (wc >> 8) & 0xFF;
            dest += sizeof(pf_u16);
        }

        *dest = '\0';
    }

    *dest = '\0';
}
